/*
*******************************************************************************
*
*   Copyright (C) 2004, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucase.c
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004aug30
*   created by: Markus W. Scherer
*
*   Low-level Unicode character/string case mapping code.
*   Much code moved here (and modified) from uchar.c.
*/

#include "unicode/utypes.h"
#include "unicode/uset.h"
#include "unicode/udata.h" /* UDataInfo */
#include "ucmndata.h" /* DataHeader */
#include "udatamem.h"
#include "umutex.h"
#include "uassert.h"
#include "cmemory.h"
#include "utrie.h"
#include "ucase.h"

struct UCaseProps {
    UDataMemory *mem;
    const int32_t *indexes;
    const uint16_t *exceptions;

    UTrie trie;
    uint8_t formatVersion[4];
};

/* data loading etc. -------------------------------------------------------- */

static UCaseProps *singletonCaseProps=NULL;
static UErrorCode singletonErrorCode=U_ZERO_ERROR;

static UBool U_CALLCONV
isAcceptable(void *context,
             const char *type, const char *name,
             const UDataInfo *pInfo) {
    if(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==UCASE_FMT_0 &&    /* dataFormat="cAsE" */
        pInfo->dataFormat[1]==UCASE_FMT_1 &&
        pInfo->dataFormat[2]==UCASE_FMT_2 &&
        pInfo->dataFormat[3]==UCASE_FMT_3 &&
        pInfo->formatVersion[0]==1 &&
        pInfo->formatVersion[2]==UTRIE_SHIFT &&
        pInfo->formatVersion[3]==UTRIE_INDEX_SHIFT
    ) {
        UCaseProps *csp=(UCaseProps *)context;
        uprv_memcpy(csp->formatVersion, pInfo->formatVersion, 4);
        return TRUE;
    } else {
        return FALSE;
    }
}

static UCaseProps *
ucase_openData(UCaseProps *cspProto,
               const uint8_t *bin, int32_t length, UErrorCode *pErrorCode) {
    UCaseProps *csp;
    int32_t size, trieSize;

    cspProto->indexes=(const int32_t *)bin;
    if( cspProto->indexes[UCASE_IX_INDEX_TOP]<16 ||
        (length>=0 && length<cspProto->indexes[UCASE_IX_LENGTH])
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    /* get the trie address, after indexes[] */
    size=cspProto->indexes[UCASE_IX_INDEX_TOP]*4;
    bin+=size;
    if(length>=0 && (length-=size)<16) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    /* unserialize the trie */
    trieSize=cspProto->indexes[UCASE_IX_TRIE_SIZE];
    trieSize=utrie_unserialize(&cspProto->trie, bin, length>=0 ? length : trieSize, pErrorCode);
    if(U_FAILURE(*pErrorCode)) {
        return NULL;
    }

    /* get exceptions[] */
    bin+=trieSize;
    if(length>=0 && (length-=trieSize)<2*cspProto->indexes[UCASE_IX_EXC_LENGTH]) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }
    cspProto->exceptions=(const uint16_t *)bin;

    /* allocate, copy, and return the new UCaseProps */
    csp=(UCaseProps *)uprv_malloc(sizeof(UCaseProps));
    if(csp==NULL) {
        *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
        return NULL;
    } else {
        uprv_memcpy(csp, cspProto, sizeof(UCaseProps));
        return csp;
    }
}

U_CAPI UCaseProps * U_EXPORT2
ucase_open(UErrorCode *pErrorCode) {
    UCaseProps cspProto={ NULL }, *csp;

    cspProto.mem=udata_openChoice(NULL, UCASE_DATA_TYPE, UCASE_DATA_NAME, isAcceptable, &cspProto, pErrorCode);
    if(U_FAILURE(*pErrorCode)) {
        return NULL;
    }

    csp=ucase_openData(
            &cspProto,
            udata_getMemory(cspProto.mem),
            udata_getLength(cspProto.mem),
            pErrorCode);
    if(U_FAILURE(*pErrorCode)) {
        udata_close(cspProto.mem);
        return NULL;
    } else {
        return csp;
    }
}

U_CAPI UCaseProps * U_EXPORT2
ucase_openBinary(const uint8_t *bin, int32_t length, UErrorCode *pErrorCode) {
    UCaseProps cspProto={ NULL };
    const DataHeader *hdr;

    if(U_FAILURE(*pErrorCode)) {
        return NULL;
    }
    if(bin==NULL) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        return NULL;
    }

    /* check the header */
    if(length>=0 && length<20) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }
    hdr=(const DataHeader *)bin;
    if(
        !(hdr->dataHeader.magic1==0xda && hdr->dataHeader.magic2==0x27 &&
          hdr->info.isBigEndian==U_IS_BIG_ENDIAN &&
          isAcceptable(&cspProto, UCASE_DATA_TYPE, UCASE_DATA_NAME, &hdr->info))
    ) {
        *pErrorCode=U_INVALID_FORMAT_ERROR;
        return NULL;
    }

    bin+=hdr->dataHeader.headerSize;
    if(length>=0) {
        length-=hdr->dataHeader.headerSize;
    }
    return ucase_openData(&cspProto, bin, length, pErrorCode);
}

U_CAPI void U_EXPORT2
ucase_close(UCaseProps *csp) {
    if(csp!=NULL) {
        udata_close(csp->mem);
        uprv_free(csp);
    }
}

/* UCaseProps singleton ----------------------------------------------------- */

static UCaseProps *gCsp=NULL;
static UErrorCode gErrorCode=U_ZERO_ERROR;
static int8_t gHaveData=0;

U_CAPI UCaseProps * U_EXPORT2
ucase_getSingleton(UErrorCode *pErrorCode) {
    int8_t haveData;

    if(U_FAILURE(*pErrorCode)) {
        return NULL;
    }

    UMTX_CHECK(NULL, gHaveData, haveData);

    if(haveData>0) {
        /* data was loaded */
        return gCsp;
    } else if(haveData<0) {
        /* data loading failed */
        *pErrorCode=gErrorCode;
        return NULL;
    } else /* haveData==0 */ {
        /* load the data */
        UCaseProps *csp=ucase_open(pErrorCode);
        if(U_FAILURE(*pErrorCode)) {
            gHaveData=-1;
            gErrorCode=*pErrorCode;
            return NULL;
        }

        /* set the static variables */
        umtx_lock(NULL);
        if(gCsp==NULL) {
            gCsp=csp;
            csp=NULL;
            gHaveData=1;
        }
        umtx_unlock(NULL);

        ucase_close(csp);
        return gCsp;
    }
}

U_CFUNC void
ucase_cleanup() {
    ucase_close(gCsp);
    gCsp=NULL;
    gErrorCode=U_ZERO_ERROR;
    gHaveData=0;
}

/* Unicode case mapping data swapping --------------------------------------- */

U_CAPI int32_t U_EXPORT2
ucase_swap(const UDataSwapper *ds,
           const void *inData, int32_t length, void *outData,
           UErrorCode *pErrorCode) {
    const UDataInfo *pInfo;
    int32_t headerSize;

    const uint8_t *inBytes;
    uint8_t *outBytes;

    const int32_t *inIndexes;
    int32_t indexes[16];

    int32_t i, offset, count, size;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    if(pErrorCode==NULL || U_FAILURE(*pErrorCode)) {
        return 0;
    }

    /* check data format and format version */
    pInfo=(const UDataInfo *)((const char *)inData+4);
    if(!(
        pInfo->dataFormat[0]==UCASE_FMT_0 &&    /* dataFormat="cAsE" */
        pInfo->dataFormat[1]==UCASE_FMT_1 &&
        pInfo->dataFormat[2]==UCASE_FMT_2 &&
        pInfo->dataFormat[3]==UCASE_FMT_3 &&
        pInfo->formatVersion[0]==1 &&
        pInfo->formatVersion[2]==UTRIE_SHIFT &&
        pInfo->formatVersion[3]==UTRIE_INDEX_SHIFT
    )) {
        udata_printError(ds, "ucase_swap(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as case mapping data\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        return 0;
    }

    inBytes=(const uint8_t *)inData+headerSize;
    outBytes=(uint8_t *)outData+headerSize;

    inIndexes=(const int32_t *)inBytes;

    if(length>=0) {
        length-=headerSize;
        if(length<16*4) {
            udata_printError(ds, "ucase_swap(): too few bytes (%d after header) for case mapping data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            return 0;
        }
    }

    /* read the first 16 indexes (ICU 3.2/format version 1: UCASE_IX_TOP==16, might grow) */
    for(i=0; i<16; ++i) {
        indexes[i]=udata_readInt32(ds, inIndexes[i]);
    }

    /* get the total length of the data */
    size=indexes[UCASE_IX_LENGTH];

    if(length>=0) {
        if(length<size) {
            udata_printError(ds, "ucase_swap(): too few bytes (%d after header) for all of case mapping data\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            return 0;
        }

        /* copy the data for inaccessible bytes */
        if(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, size);
        }

        offset=0;

        /* swap the int32_t indexes[] */
        count=indexes[UCASE_IX_INDEX_TOP]*4;
        ds->swapArray32(ds, inBytes, count, outBytes, pErrorCode);
        offset+=count;

        /* swap the UTrie */
        count=indexes[UCASE_IX_TRIE_SIZE];
        utrie_swap(ds, inBytes+offset, count, outBytes+offset, pErrorCode);
        offset+=count;

        /* swap the uint16_t exceptions[] */
        count=indexes[UCASE_IX_EXC_LENGTH]*2;
        ds->swapArray16(ds, inBytes+offset, count, outBytes+offset, pErrorCode);
        offset+=count;

        U_ASSERT(offset==size);
    }

    return headerSize+size;
}

/* set of property starts for UnicodeSet ------------------------------------ */

static UBool U_CALLCONV
_enumPropertyStartsRange(const void *context, UChar32 start, UChar32 limit, uint32_t value) {
    /* add the start code point to the USet */
    USetAdder *sa=(USetAdder *)context;
    sa->add(sa->set, start);
    return TRUE;
}

U_CAPI void U_EXPORT2
ucase_addPropertyStarts(const UCaseProps *csp, USetAdder *sa, UErrorCode *pErrorCode) {
    /* add the start code point of each same-value range of the trie */
    utrie_enum(&csp->trie, NULL, _enumPropertyStartsRange, sa);

    /* add code points with hardcoded properties, plus the ones following them */

    /* (none right now, see comment below) */

    /*
     * Omit code points with hardcoded specialcasing properties
     * because we do not build property UnicodeSets for them right now.
     */
}

/* data access primitives --------------------------------------------------- */

/* UTRIE_GET16() itself validates c */
#define GET_PROPS(csp, c, result) \
    UTRIE_GET16(&(csp)->trie, c, result);

#define GET_CASE_TYPE(props) ((props)&UCASE_TYPE_MASK)
#define GET_SIGNED_DELTA(props) ((int16_t)(props)>>UCASE_DELTA_SHIFT)
#define GET_EXCEPTIONS(csp, props) ((csp)->exceptions+((props)>>UCASE_EXC_SHIFT))

#define PROPS_HAS_EXCEPTION(props) ((props)&UCASE_EXCEPTION)

/* number of bits in an 8-bit integer value */
static const uint8_t flagsOffset[256]={
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

#define HAS_SLOT(flags, index) ((flags)&(1<<(index)))
#define SLOT_OFFSET(flags, index) flagsOffset[(flags)&((1<<(index))-1)]

/*
 * Get the value of an optional-value slot where HAS_SLOT(excWord, index).
 *
 * @param excWord (in) initial exceptions word
 * @param index (in) desired slot index
 * @param pExc16 (in/out) const uint16_t * after excWord=*pExc16++;
 *               moved to the last uint16_t of the value, use +1 for beginning of next slot
 * @param value (out) int32_t or uint32_t output if hasSlot, otherwise not modified
 */
#define GET_SLOT_VALUE(excWord, index, pExc16, value) \
    if(((excWord)&UCASE_EXC_DOUBLE_SLOTS)==0) { \
        (pExc16)+=SLOT_OFFSET(excWord, index); \
        (value)=*pExc16; \
    } else { \
        (pExc16)+=2*SLOT_OFFSET(excWord, index); \
        (value)=*pExc16++; \
        (value)=((value)<<16)|*pExc16; \
    }

/* simple case mappings ----------------------------------------------------- */

U_CAPI UChar32 U_EXPORT2
ucase_tolower(const UCaseProps *csp, UChar32 c) {
    uint16_t props;
    GET_PROPS(csp, c, props);
    if(!PROPS_HAS_EXCEPTION(props)) {
        if(GET_CASE_TYPE(props)>=UCASE_UPPER) {
            c+=GET_SIGNED_DELTA(props);
        }
    } else {
        const uint16_t *pe=GET_EXCEPTIONS(csp, props);
        uint16_t excWord=*pe++;
        if(HAS_SLOT(excWord, UCASE_EXC_LOWER)) {
            GET_SLOT_VALUE(excWord, UCASE_EXC_LOWER, pe, c);
        }
    }
    return c;
}

U_CAPI UChar32 U_EXPORT2
ucase_toupper(const UCaseProps *csp, UChar32 c) {
    uint16_t props;
    GET_PROPS(csp, c, props);
    if(!PROPS_HAS_EXCEPTION(props)) {
        if(GET_CASE_TYPE(props)==UCASE_LOWER) {
            c+=GET_SIGNED_DELTA(props);
        }
    } else {
        const uint16_t *pe=GET_EXCEPTIONS(csp, props);
        uint16_t excWord=*pe++;
        if(HAS_SLOT(excWord, UCASE_EXC_UPPER)) {
            GET_SLOT_VALUE(excWord, UCASE_EXC_UPPER, pe, c);
        }
    }
    return c;
}

U_CAPI UChar32 U_EXPORT2
ucase_totitle(const UCaseProps *csp, UChar32 c) {
    uint16_t props;
    GET_PROPS(csp, c, props);
    if(!PROPS_HAS_EXCEPTION(props)) {
        if(GET_CASE_TYPE(props)==UCASE_LOWER) {
            c+=GET_SIGNED_DELTA(props);
        }
    } else {
        const uint16_t *pe=GET_EXCEPTIONS(csp, props);
        uint16_t excWord=*pe++;
        int32_t index;
        if(HAS_SLOT(excWord, UCASE_EXC_TITLE)) {
            index=UCASE_EXC_TITLE;
        } else if(HAS_SLOT(excWord, UCASE_EXC_UPPER)) {
            index=UCASE_EXC_UPPER;
        } else {
            return c;
        }
        GET_SLOT_VALUE(excWord, index, pe, c);
    }
    return c;
}

/** @return UCASE_NONE, UCASE_LOWER, UCASE_UPPER, UCASE_TITLE */
U_CAPI int32_t U_EXPORT2
ucase_getType(const UCaseProps *csp, UChar32 c) {
    uint16_t props;
    GET_PROPS(csp, c, props);
    return GET_CASE_TYPE(props);
}

/** @return same as ucase_getType(), or <0 if c is case-ignorable */
U_CAPI int32_t U_EXPORT2
ucase_getTypeOrIgnorable(const UCaseProps *csp, UChar32 c) {
    int32_t type;
    uint16_t props;
    GET_PROPS(csp, c, props);
    type=GET_CASE_TYPE(props);
    if(type!=UCASE_NONE) {
        return type;
    } else if(
        c==0x307 ||
        (props&(UCASE_EXCEPTION|UCASE_CASE_IGNORABLE))==UCASE_CASE_IGNORABLE
    ) {
        return -1; /* case-ignorable */
    } else {
        return 0; /* c is neither cased nor case-ignorable */
    }
}

/** @return UCASE_NO_DOT, UCASE_SOFT_DOTTED, UCASE_ABOVE, UCASE_OTHER_ACCENT */
static U_INLINE int32_t
getDotType(const UCaseProps *csp, UChar32 c) {
    uint16_t props;
    GET_PROPS(csp, c, props);
    if(!PROPS_HAS_EXCEPTION(props)) {
        return props&UCASE_DOT_MASK;
    } else {
        const uint16_t *pe=GET_EXCEPTIONS(csp, props);
        return (*pe>>UCASE_EXC_DOT_SHIFT)&UCASE_DOT_MASK;
    }
}

U_CAPI UBool U_EXPORT2
ucase_isSoftDotted(const UCaseProps *csp, UChar32 c) {
    return (UBool)(getDotType(csp, c)==UCASE_SOFT_DOTTED);
}

U_CAPI UBool U_EXPORT2
ucase_isCaseSensitive(const UCaseProps *csp, UChar32 c) {
    uint16_t props;
    GET_PROPS(csp, c, props);
    return (UBool)((props&UCASE_SENSITIVE)!=0);
}

/* string casing ------------------------------------------------------------ */

/*
 * These internal functions form the core of string case mappings.
 * They map single code points to result code points or strings and take
 * all necessary conditions (context, locale ID, options) into account.
 *
 * They do not iterate over the source or write to the destination
 * so that the same functions are useful for non-standard string storage,
 * such as in a Replaceable (for Transliterator) or UTF-8/32 strings etc.
 * For the same reason, the "surrounding text" context is passed in as a
 * UCaseContextIterator which does not make any assumptions about
 * the underlying storage.
 *
 * This section contains helper functions that check for conditions
 * in the input text surrounding the current code point
 * according to SpecialCasing.txt.
 *
 * Each helper function gets the index
 * - after the current code point if it looks at following text
 * - before the current code point if it looks at preceding text
 *
 * Unicode 3.2 UAX 21 "Case Mappings" defines the conditions as follows:
 *
 * Final_Sigma
 *   C is preceded by a sequence consisting of
 *     a cased letter and a case-ignorable sequence,
 *   and C is not followed by a sequence consisting of
 *     an ignorable sequence and then a cased letter.
 *
 * More_Above
 *   C is followed by one or more characters of combining class 230 (ABOVE)
 *   in the combining character sequence.
 *
 * After_Soft_Dotted
 *   The last preceding character with combining class of zero before C
 *   was Soft_Dotted,
 *   and there is no intervening combining character class 230 (ABOVE).
 *
 * Before_Dot
 *   C is followed by combining dot above (U+0307).
 *   Any sequence of characters with a combining class that is neither 0 nor 230
 *   may intervene between the current character and the combining dot above.
 *
 * The erratum from 2002-10-31 adds the condition
 *
 * After_I
 *   The last preceding base character was an uppercase I, and there is no
 *   intervening combining character class 230 (ABOVE).
 *
 *   (See Jitterbug 2344 and the comments on After_I below.)
 *
 * Helper definitions in Unicode 3.2 UAX 21:
 *
 * D1. A character C is defined to be cased
 *     if it meets any of the following criteria:
 *
 *   - The general category of C is Titlecase Letter (Lt)
 *   - In [CoreProps], C has one of the properties Uppercase, or Lowercase
 *   - Given D = NFD(C), then it is not the case that:
 *     D = UCD_lower(D) = UCD_upper(D) = UCD_title(D)
 *     (This third criterium does not add any characters to the list
 *      for Unicode 3.2. Ignored.)
 *
 * D2. A character C is defined to be case-ignorable
 *     if it meets either of the following criteria:
 *
 *   - The general category of C is
 *     Nonspacing Mark (Mn), or Enclosing Mark (Me), or Format Control (Cf), or
 *     Letter Modifier (Lm), or Symbol Modifier (Sk)
 *   - C is one of the following characters 
 *     U+0027 APOSTROPHE
 *     U+00AD SOFT HYPHEN (SHY)
 *     U+2019 RIGHT SINGLE QUOTATION MARK
 *            (the preferred character for apostrophe)
 *
 * D3. A case-ignorable sequence is a sequence of
 *     zero or more case-ignorable characters.
 */

enum {
    LOC_UNKNOWN,
    LOC_ROOT,
    LOC_TURKISH,
    LOC_LITHUANIAN
};

#define is_a(c) ((c)=='a' || (c)=='A')
#define is_e(c) ((c)=='e' || (c)=='E')
#define is_i(c) ((c)=='i' || (c)=='I')
#define is_l(c) ((c)=='l' || (c)=='L')
#define is_r(c) ((c)=='r' || (c)=='R')
#define is_t(c) ((c)=='t' || (c)=='T')
#define is_u(c) ((c)=='u' || (c)=='U')
#define is_z(c) ((c)=='z' || (c)=='Z')

/* separator? */
#define is_sep(c) ((c)=='_' || (c)=='-' || (c)==0)

/*
 * Requires non-NULL locale ID but otherwise does the equivalent of
 * checking for language codes as if uloc_getLanguage() were called:
 * Accepts both 2- and 3-letter codes and accepts case variants.
 */
static int32_t
getCaseLocale(const char *locale, int32_t *locCache) {
    int32_t result;
    char c;

    if(locCache!=NULL && (result=*locCache)!=LOC_UNKNOWN) {
        return result;
    }

    result=LOC_ROOT;

    /*
     * This function used to use uloc_getLanguage(), but the current code
     * removes the dependency of this low-level code on uloc implementation code
     * and is faster because not the whole locale ID has to be
     * examined and copied/transformed.
     *
     * Because this code does not want to depend on uloc, the caller must
     * pass in a non-NULL locale, i.e., may need to call uloc_getDefault().
     */
    c=*locale++;
    if(is_t(c)) {
        /* tr or tur? */
        c=*locale++;
        if(is_u(c)) {
            c=*locale++;
        }
        if(is_r(c)) {
            c=*locale;
            if(is_sep(c)) {
                result=LOC_TURKISH;
            }
        }
    } else if(is_a(c)) {
        /* az or aze? */
        c=*locale++;
        if(is_z(c)) {
            c=*locale++;
            if(is_e(c)) {
                c=*locale;
            }
            if(is_sep(c)) {
                result=LOC_TURKISH;
            }
        }
    } else if(is_l(c)) {
        /* lt or lit? */
        c=*locale++;
        if(is_i(c)) {
            c=*locale++;
        }
        if(is_t(c)) {
            c=*locale;
            if(is_sep(c)) {
                result=LOC_LITHUANIAN;
            }
        }
    }

    if(locCache!=NULL) {
        *locCache=result;
    }
    return result;
}

/* Is followed by {case-ignorable}* cased  ? (dir determines looking forward/backward) */
static UBool
isFollowedByCasedLetter(const UCaseProps *csp, UCaseContextIterator *iter, void *context, int8_t dir) {
    UChar32 c;
    uint16_t props;

    if(iter==NULL) {
        return FALSE;
    }

    for(/* dir!=0 sets direction */; (c=iter(context, dir))>=0; dir=0) {
        GET_PROPS(csp, c, props);
        if(GET_CASE_TYPE(props)!=UCASE_NONE) {
            return TRUE; /* followed by cased letter */
        } else if(c==0x307 || (props&(UCASE_EXCEPTION|UCASE_CASE_IGNORABLE))==UCASE_CASE_IGNORABLE) {
            /* case-ignorable, continue with the loop */
        } else {
            return FALSE; /* not ignorable */
        }
    }

    return FALSE; /* not followed by cased letter */
}

/* Is preceded by Soft_Dotted character with no intervening cc=230 ? */
static UBool
isPrecededBySoftDotted(const UCaseProps *csp, UCaseContextIterator *iter, void *context) {
    UChar32 c;
    int32_t dotType;
    int8_t dir;

    if(iter==NULL) {
        return FALSE;
    }

    for(dir=-1; (c=iter(context, dir))>=0; dir=0) {
        dotType=getDotType(csp, c);
        if(dotType==UCASE_SOFT_DOTTED) {
            return TRUE; /* preceded by TYPE_i */
        } else if(dotType!=UCASE_OTHER_ACCENT) {
            return FALSE; /* preceded by different base character (not TYPE_i), or intervening cc==230 */
        }
    }

    return FALSE; /* not preceded by TYPE_i */
}

/*
 * See Jitterbug 2344:
 * The condition After_I for Turkic-lowercasing of U+0307 combining dot above
 * is checked in ICU 2.0, 2.1, 2.6 but was not in 2.2 & 2.4 because
 * we made those releases compatible with Unicode 3.2 which had not fixed
 * a related bug in SpecialCasing.txt.
 *
 * From the Jitterbug 2344 text:
 * ... this bug is listed as a Unicode erratum
 * from 2002-10-31 at http://www.unicode.org/uni2errata/UnicodeErrata.html
 * <quote>
 * There are two errors in SpecialCasing.txt.
 * 1. Missing semicolons on two lines. ... [irrelevant for ICU]
 * 2. An incorrect context definition. Correct as follows:
 * < 0307; ; 0307; 0307; tr After_Soft_Dotted; # COMBINING DOT ABOVE
 * < 0307; ; 0307; 0307; az After_Soft_Dotted; # COMBINING DOT ABOVE
 * ---
 * > 0307; ; 0307; 0307; tr After_I; # COMBINING DOT ABOVE
 * > 0307; ; 0307; 0307; az After_I; # COMBINING DOT ABOVE
 * where the context After_I is defined as:
 * The last preceding base character was an uppercase I, and there is no
 * intervening combining character class 230 (ABOVE).
 * </quote>
 *
 * Note that SpecialCasing.txt even in Unicode 3.2 described the condition as:
 *
 * # When lowercasing, remove dot_above in the sequence I + dot_above, which will turn into i.
 * # This matches the behavior of the canonically equivalent I-dot_above
 *
 * See also the description in this place in older versions of uchar.c (revision 1.100).
 *
 * Markus W. Scherer 2003-feb-15
 */

/* Is preceded by base character 'I' with no intervening cc=230 ? */
static UBool
isPrecededBy_I(const UCaseProps *csp, UCaseContextIterator *iter, void *context) {
    UChar32 c;
    int32_t dotType;
    int8_t dir;

    if(iter==NULL) {
        return FALSE;
    }

    for(dir=-1; (c=iter(context, dir))>=0; dir=0) {
        if(c==0x49) {
            return TRUE; /* preceded by I */
        }
        dotType=getDotType(csp, c);
        if(dotType!=UCASE_OTHER_ACCENT) {
            return FALSE; /* preceded by different base character (not I), or intervening cc==230 */
        }
    }

    return FALSE; /* not preceded by I */
}

/* Is followed by one or more cc==230 ? */
static UBool
isFollowedByMoreAbove(const UCaseProps *csp, UCaseContextIterator *iter, void *context) {
    UChar32 c;
    int32_t dotType;
    int8_t dir;

    if(iter==NULL) {
        return FALSE;
    }

    for(dir=1; (c=iter(context, dir))>=0; dir=0) {
        dotType=getDotType(csp, c);
        if(dotType==UCASE_ABOVE) {
            return TRUE; /* at least one cc==230 following */
        } else if(dotType!=UCASE_OTHER_ACCENT) {
            return FALSE; /* next base character, no more cc==230 following */
        }
    }

    return FALSE; /* no more cc==230 following */
}

/* Is followed by a dot above (without cc==230 in between) ? */
static UBool
isFollowedByDotAbove(const UCaseProps *csp, UCaseContextIterator *iter, void *context) {
    UChar32 c;
    int32_t dotType;
    int8_t dir;

    if(iter==NULL) {
        return FALSE;
    }

    for(dir=1; (c=iter(context, dir))>=0; dir=0) {
        if(c==0x307) {
            return TRUE;
        }
        dotType=getDotType(csp, c);
        if(dotType!=UCASE_OTHER_ACCENT) {
            return FALSE; /* next base character or cc==230 in between */
        }
    }

    return FALSE; /* no dot above following */
}

U_CAPI int32_t U_EXPORT2
ucase_toFullLower(const UCaseProps *csp, UChar32 c,
                  UCaseContextIterator *iter, void *context,
                  const UChar **pString,
                  const char *locale, int32_t *locCache) {
    static const UChar
        iDot[2]=        { 0x69, 0x307 },
        jDot[2]=        { 0x6a, 0x307 },
        iOgonekDot[3]= { 0x12f, 0x307 },
        iDotGrave[3]=   { 0x69, 0x307, 0x300 },
        iDotAcute[3]=   { 0x69, 0x307, 0x301 },
        iDotTilde[3]=   { 0x69, 0x307, 0x303 };

    UChar32 result;
    uint16_t props;

    result=c;
    GET_PROPS(csp, c, props);
    if(!PROPS_HAS_EXCEPTION(props)) {
        if(GET_CASE_TYPE(props)>=UCASE_UPPER) {
            result=c+GET_SIGNED_DELTA(props);
        }
    } else {
        const uint16_t *pe=GET_EXCEPTIONS(csp, props), *pe2;
        uint16_t excWord=*pe++;
        int32_t full;

        pe2=pe;

        if(excWord&UCASE_EXC_CONDITIONAL_SPECIAL) {
            /* use hardcoded conditions and mappings */
            int32_t loc=getCaseLocale(locale, locCache);

            /*
             * Test for conditional mappings first
             *   (otherwise the unconditional default mappings are always taken),
             * then test for characters that have unconditional mappings in SpecialCasing.txt,
             * then get the UnicodeData.txt mappings.
             */
            if( loc==LOC_LITHUANIAN &&
                    /* base characters, find accents above */
                    (((c==0x49 || c==0x4a || c==0x12e) &&
                        isFollowedByMoreAbove(csp, iter, context)) ||
                    /* precomposed with accent above, no need to find one */
                    (c==0xcc || c==0xcd || c==0x128))
            ) {
                /*
                    # Lithuanian

                    # Lithuanian retains the dot in a lowercase i when followed by accents.

                    # Introduce an explicit dot above when lowercasing capital I's and J's
                    # whenever there are more accents above.
                    # (of the accents used in Lithuanian: grave, acute, tilde above, and ogonek)

                    0049; 0069 0307; 0049; 0049; lt More_Above; # LATIN CAPITAL LETTER I
                    004A; 006A 0307; 004A; 004A; lt More_Above; # LATIN CAPITAL LETTER J
                    012E; 012F 0307; 012E; 012E; lt More_Above; # LATIN CAPITAL LETTER I WITH OGONEK
                    00CC; 0069 0307 0300; 00CC; 00CC; lt; # LATIN CAPITAL LETTER I WITH GRAVE
                    00CD; 0069 0307 0301; 00CD; 00CD; lt; # LATIN CAPITAL LETTER I WITH ACUTE
                    0128; 0069 0307 0303; 0128; 0128; lt; # LATIN CAPITAL LETTER I WITH TILDE
                 */
                switch(c) {
                case 0x49:  /* LATIN CAPITAL LETTER I */
                    *pString=iDot;
                    return 2;
                case 0x4a:  /* LATIN CAPITAL LETTER J */
                    *pString=jDot;
                    return 2;
                case 0x12e: /* LATIN CAPITAL LETTER I WITH OGONEK */
                    *pString=iOgonekDot;
                    return 2;
                case 0xcc:  /* LATIN CAPITAL LETTER I WITH GRAVE */
                    *pString=iDotGrave;
                    return 3;
                case 0xcd:  /* LATIN CAPITAL LETTER I WITH ACUTE */
                    *pString=iDotAcute;
                    return 3;
                case 0x128: /* LATIN CAPITAL LETTER I WITH TILDE */
                    *pString=iDotTilde;
                    return 3;
                default:
                    return 0; /* will not occur */
                }
            /* # Turkish and Azeri */
            } else if(loc==LOC_TURKISH && c==0x130) {
                /*
                    # I and i-dotless; I-dot and i are case pairs in Turkish and Azeri
                    # The following rules handle those cases.

                    0130; 0069; 0130; 0130; tr # LATIN CAPITAL LETTER I WITH DOT ABOVE
                    0130; 0069; 0130; 0130; az # LATIN CAPITAL LETTER I WITH DOT ABOVE
                 */
                return 0x69;
            } else if(loc==LOC_TURKISH && c==0x307 && isPrecededBy_I(csp, iter, context)) {
                /*
                    # When lowercasing, remove dot_above in the sequence I + dot_above, which will turn into i.
                    # This matches the behavior of the canonically equivalent I-dot_above

                    0307; ; 0307; 0307; tr After_I; # COMBINING DOT ABOVE
                    0307; ; 0307; 0307; az After_I; # COMBINING DOT ABOVE
                 */
                return 0; /* remove the dot (continue without output) */
            } else if(loc==LOC_TURKISH && c==0x49 && !isFollowedByDotAbove(csp, iter, context)) {
                /*
                    # When lowercasing, unless an I is before a dot_above, it turns into a dotless i.

                    0049; 0131; 0049; 0049; tr Not_Before_Dot; # LATIN CAPITAL LETTER I
                    0049; 0131; 0049; 0049; az Not_Before_Dot; # LATIN CAPITAL LETTER I
                 */
                return 0x131;
            } else if(c==0x130) {
                /*
                    # Preserve canonical equivalence for I with dot. Turkic is handled below.

                    0130; 0069 0307; 0130; 0130; # LATIN CAPITAL LETTER I WITH DOT ABOVE
                 */
                *pString=iDot;
                return 2;
            } else if(  c==0x3a3 &&
                        !isFollowedByCasedLetter(csp, iter, context, 1) &&
                        isFollowedByCasedLetter(csp, iter, context, -1) /* -1=preceded */
            ) {
                /* greek capital sigma maps depending on surrounding cased letters (see SpecialCasing.txt) */
                /*
                    # Special case for final form of sigma

                    03A3; 03C2; 03A3; 03A3; Final_Sigma; # GREEK CAPITAL LETTER SIGMA
                 */
                return 0x3c2; /* greek small final sigma */
            } else {
                /* no known conditional special case mapping, use a normal mapping */
            }
        } else if(HAS_SLOT(excWord, UCASE_EXC_FULL_MAPPINGS)) {
            GET_SLOT_VALUE(excWord, UCASE_EXC_FULL_MAPPINGS, pe, full);
            full&=UCASE_FULL_LOWER;
            if(full!=0) {
                /* set the output pointer to the lowercase mapping */
                *pString=pe+1;

                /* return the string length */
                return full;
            }
        }

        if(HAS_SLOT(excWord, UCASE_EXC_LOWER)) {
            GET_SLOT_VALUE(excWord, UCASE_EXC_LOWER, pe2, result);
        }
    }

    return (result==c) ? ~result : result;
}

/* internal */
static int32_t
toUpperOrTitle(const UCaseProps *csp, UChar32 c,
               UCaseContextIterator *iter, void *context,
               const UChar **pString,
               const char *locale, int32_t *locCache,
               UBool upperNotTitle) {
    UChar32 result;
    uint16_t props;

    result=c;
    GET_PROPS(csp, c, props);
    if(!PROPS_HAS_EXCEPTION(props)) {
        if(GET_CASE_TYPE(props)==UCASE_LOWER) {
            result=c+GET_SIGNED_DELTA(props);
        }
    } else {
        const uint16_t *pe=GET_EXCEPTIONS(csp, props), *pe2;
        uint16_t excWord=*pe++;
        int32_t full, index;

        pe2=pe;

        if(excWord&UCASE_EXC_CONDITIONAL_SPECIAL) {
            /* use hardcoded conditions and mappings */
            int32_t loc=getCaseLocale(locale, locCache);

            if(loc==LOC_TURKISH && c==0x69) {
                /*
                    # Turkish and Azeri

                    # I and i-dotless; I-dot and i are case pairs in Turkish and Azeri
                    # The following rules handle those cases.

                    # When uppercasing, i turns into a dotted capital I

                    0069; 0069; 0130; 0130; tr; # LATIN SMALL LETTER I
                    0069; 0069; 0130; 0130; az; # LATIN SMALL LETTER I
                */
                return 0x130;
            } else if(loc==LOC_LITHUANIAN && c==0x307 && isPrecededBySoftDotted(csp, iter, context)) {
                /*
                    # Lithuanian

                    # Lithuanian retains the dot in a lowercase i when followed by accents.

                    # Remove DOT ABOVE after "i" with upper or titlecase

                    0307; 0307; ; ; lt After_Soft_Dotted; # COMBINING DOT ABOVE
                 */
                return 0; /* remove the dot (continue without output) */
            } else {
                /* no known conditional special case mapping, use a normal mapping */
            }
        } else if(HAS_SLOT(excWord, UCASE_EXC_FULL_MAPPINGS)) {
            GET_SLOT_VALUE(excWord, UCASE_EXC_FULL_MAPPINGS, pe, full);

            /* start of full case mapping strings */
            ++pe;

            /* skip the lowercase and case-folding result strings */
            pe+=full&UCASE_FULL_LOWER;
            full>>=4;
            pe+=full&0xf;
            full>>=4;

            if(upperNotTitle) {
                full&=0xf;
            } else {
                /* skip the uppercase result string */
                pe+=full&0xf;
                full=(full>>4)&0xf;
            }

            if(full!=0) {
                /* set the output pointer to the result string */
                *pString=pe;

                /* return the string length */
                return full;
            }
        }

        if(!upperNotTitle && HAS_SLOT(excWord, UCASE_EXC_TITLE)) {
            index=UCASE_EXC_TITLE;
        } else if(HAS_SLOT(excWord, UCASE_EXC_UPPER)) {
            /* here, titlecase is same as uppercase */
            index=UCASE_EXC_UPPER;
        } else {
            return ~c;
        }
        GET_SLOT_VALUE(excWord, index, pe2, result);
    }

    return (result==c) ? ~result : result;
}

U_CAPI int32_t U_EXPORT2
ucase_toFullUpper(const UCaseProps *csp, UChar32 c,
                  UCaseContextIterator *iter, void *context,
                  const UChar **pString,
                  const char *locale, int32_t *locCache) {
    return toUpperOrTitle(csp, c, iter, context, pString, locale, locCache, TRUE);
}

U_CAPI int32_t U_EXPORT2
ucase_toFullTitle(const UCaseProps *csp, UChar32 c,
                  UCaseContextIterator *iter, void *context,
                  const UChar **pString,
                  const char *locale, int32_t *locCache) {
    return toUpperOrTitle(csp, c, iter, context, pString, locale, locCache, FALSE);
}

/* case folding ------------------------------------------------------------- */

/*
 * Case folding is similar to lowercasing.
 * The result may be a simple mapping, i.e., a single code point, or
 * a full mapping, i.e., a string.
 * If the case folding for a code point is the same as its simple (1:1) lowercase mapping,
 * then only the lowercase mapping is stored.
 *
 * Some special cases are hardcoded because their conditions cannot be
 * parsed and processed from CaseFolding.txt.
 *
 * Unicode 3.2 CaseFolding.txt specifies for its status field:

# C: common case folding, common mappings shared by both simple and full mappings.
# F: full case folding, mappings that cause strings to grow in length. Multiple characters are separated by spaces.
# S: simple case folding, mappings to single characters where different from F.
# T: special case for uppercase I and dotted uppercase I
#    - For non-Turkic languages, this mapping is normally not used.
#    - For Turkic languages (tr, az), this mapping can be used instead of the normal mapping for these characters.
#
# Usage:
#  A. To do a simple case folding, use the mappings with status C + S.
#  B. To do a full case folding, use the mappings with status C + F.
#
#    The mappings with status T can be used or omitted depending on the desired case-folding
#    behavior. (The default option is to exclude them.)

 * Unicode 3.2 has 'T' mappings as follows:

0049; T; 0131; # LATIN CAPITAL LETTER I
0130; T; 0069; # LATIN CAPITAL LETTER I WITH DOT ABOVE

 * while the default mappings for these code points are:

0049; C; 0069; # LATIN CAPITAL LETTER I
0130; F; 0069 0307; # LATIN CAPITAL LETTER I WITH DOT ABOVE

 * U+0130 is otherwise lowercased to U+0069 (UnicodeData.txt).
 *
 * In case this code is used with CaseFolding.txt from an older version of Unicode
 * where CaseFolding.txt contains mappings with a status of 'I' that
 * have the opposite polarity ('I' mappings are included by default but excluded for Turkic),
 * we must also hardcode the Unicode 3.2 mappings for the code points 
 * with 'I' mappings.
 * Unicode 3.1.1 has 'I' mappings for U+0130 and U+0131.
 * Unicode 3.2 has a 'T' mapping for U+0130, and lowercases U+0131 to itself (see UnicodeData.txt).
 */

/* return the simple case folding mapping for c */
U_CAPI UChar32 U_EXPORT2
ucase_fold(UCaseProps *csp, UChar32 c, uint32_t options) {
    uint16_t props;
    GET_PROPS(csp, c, props);
    if(!PROPS_HAS_EXCEPTION(props)) {
        if(GET_CASE_TYPE(props)>=UCASE_UPPER) {
            c+=GET_SIGNED_DELTA(props);
        }
    } else {
        const uint16_t *pe=GET_EXCEPTIONS(csp, props);
        uint16_t excWord=*pe++;
        int32_t index;
        if(excWord&UCASE_EXC_CONDITIONAL_FOLD) {
            /* special case folding mappings, hardcoded */
            if((options&_FOLD_CASE_OPTIONS_MASK)==U_FOLD_CASE_DEFAULT) {
                /* default mappings */
                if(c==0x49) {
                    /* 0049; C; 0069; # LATIN CAPITAL LETTER I */
                    return 0x69;
                } else if(c==0x130) {
                    /* no simple default mapping for U+0130, use UnicodeData.txt */
                    return 0x69;
                }
            } else {
                /* Turkic mappings */
                if(c==0x49) {
                    /* 0049; T; 0131; # LATIN CAPITAL LETTER I */
                    return 0x131;
                } else if(c==0x130) {
                    /* 0130; T; 0069; # LATIN CAPITAL LETTER I WITH DOT ABOVE */
                    return 0x69;
                }
            }
        }
        if(HAS_SLOT(excWord, UCASE_EXC_FOLD)) {
            index=UCASE_EXC_FOLD;
        } else if(HAS_SLOT(excWord, UCASE_EXC_LOWER)) {
            index=UCASE_EXC_LOWER;
        } else {
            return c;
        }
        GET_SLOT_VALUE(excWord, index, pe, c);
    }
    return c;
}

/*
 * Issue for canonical caseless match (UAX #21):
 * Turkic casefolding (using "T" mappings in CaseFolding.txt) does not preserve
 * canonical equivalence, unlike default-option casefolding.
 * For example, I-grave and I + grave fold to strings that are not canonically
 * equivalent.
 * For more details, see the comment in unorm_compare() in unorm.cpp
 * and the intermediate prototype changes for Jitterbug 2021.
 * (For example, revision 1.104 of uchar.c and 1.4 of CaseFolding.txt.)
 *
 * This did not get fixed because it appears that it is not possible to fix
 * it for uppercase and lowercase characters (I-grave vs. i-grave)
 * together in a way that they still fold to common result strings.
 */

U_CAPI int32_t U_EXPORT2
ucase_toFullFolding(const UCaseProps *csp, UChar32 c,
                    const UChar **pString,
                    uint32_t options) {
    static const UChar
        iDot[2]=        { 0x69, 0x307 };

    UChar32 result;
    uint16_t props;

    result=c;
    GET_PROPS(csp, c, props);
    if(!PROPS_HAS_EXCEPTION(props)) {
        if(GET_CASE_TYPE(props)>=UCASE_UPPER) {
            result=c+GET_SIGNED_DELTA(props);
        }
    } else {
        const uint16_t *pe=GET_EXCEPTIONS(csp, props), *pe2;
        uint16_t excWord=*pe++;
        int32_t full, index;

        pe2=pe;

        if(excWord&UCASE_EXC_CONDITIONAL_FOLD) {
            /* use hardcoded conditions and mappings */
            if((options&_FOLD_CASE_OPTIONS_MASK)==U_FOLD_CASE_DEFAULT) {
                /* default mappings */
                if(c==0x49) {
                    /* 0049; C; 0069; # LATIN CAPITAL LETTER I */
                    return 0x69;
                } else if(c==0x130) {
                    /* 0130; F; 0069 0307; # LATIN CAPITAL LETTER I WITH DOT ABOVE */
                    *pString=iDot;
                    return 2;
                }
            } else {
                /* Turkic mappings */
                if(c==0x49) {
                    /* 0049; T; 0131; # LATIN CAPITAL LETTER I */
                    return 0x131;
                } else if(c==0x130) {
                    /* 0130; T; 0069; # LATIN CAPITAL LETTER I WITH DOT ABOVE */
                    return 0x69;
                }
            }
        } else if(HAS_SLOT(excWord, UCASE_EXC_FULL_MAPPINGS)) {
            GET_SLOT_VALUE(excWord, UCASE_EXC_FULL_MAPPINGS, pe, full);

            /* start of full case mapping strings */
            ++pe;

            /* skip the lowercase result string */
            pe+=full&UCASE_FULL_LOWER;
            full=(full>>4)&0xf;

            if(full!=0) {
                /* set the output pointer to the result string */
                *pString=pe;

                /* return the string length */
                return full;
            }
        }

        if(HAS_SLOT(excWord, UCASE_EXC_FOLD)) {
            index=UCASE_EXC_FOLD;
        } else if(HAS_SLOT(excWord, UCASE_EXC_LOWER)) {
            index=UCASE_EXC_LOWER;
        } else {
            return ~c;
        }
        GET_SLOT_VALUE(excWord, index, pe2, result);
    }

    return (result==c) ? ~result : result;
}
