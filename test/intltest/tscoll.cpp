/********************************************************************
 * COPYRIGHT: 
 * Copyright (c) 1997-2003, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/

/**
 * IntlTestCollator is the medium level test class for everything in the directory "collate".
 */

/***********************************************************************
* Modification history
* Date        Name        Description
* 02/14/2001  synwee      Compare with cintltst and commented away tests 
*                         that are not run.
***********************************************************************/

#include "unicode/utypes.h"

#if !UCONFIG_NO_COLLATION

#include "unicode/uchar.h"


#include "dadrcoll.h"

#include "encoll.h"
#include "frcoll.h"
#include "decoll.h"
#include "dacoll.h"
#include "escoll.h"
#include "ficoll.h"
#include "jacoll.h"
#include "trcoll.h"
#include "allcoll.h"
#include "g7coll.h"
#include "mnkytst.h"
#include "apicoll.h"
#include "regcoll.h"
#include "currcoll.h"
#include "itercoll.h"
//#include "capicoll.h"   // CollationCAPITest
#include "tstnorm.h"
#include "normconf.h"
#include "thcoll.h"
#include "srchtest.h"
#include "cntabcol.h"
#include "lcukocol.h"
#include "ucaconf.h"
#include "svccoll.h"

void IntlTestCollator::runIndexedTest( int32_t index, UBool exec, const char* &name, char* par )
{
    if (exec)
    {
        logln("TestSuite Collator: ");
    }

    switch (index)
    {
    case 0:
        name = "CollationEnglishTest";

        if (exec)
        {
            logln("CollationEnglishtest---");
            logln("");
            CollationEnglishTest test;
            callTest( test, par );
        }
        break;

    case 1:
        name = "CollationFrenchTest";
        
        if (exec)
        {
            logln("CollationFrenchtest---");
            logln("");
            CollationFrenchTest test;
            callTest( test, par );
        }
        break;

    case 2:
        name = "CollationGermanTest";
        
        if (exec)
        {
            logln("CollationGermantest---");
            logln("");

            CollationGermanTest test;
            callTest( test, par );
        }
        break;

    case 3:
        name = "CollationDanishTest";

        if (exec)
        {
            logln("CollationDanishtest---");
            logln("");
            
            CollationDanishTest test;
            callTest( test, par );
        }
        break;

    case 4:
        name = "CollationSpanishTest";
        
        if (exec)
        {
            logln("CollationSpanishtest---");
            logln("");

            CollationSpanishTest test;
            callTest( test, par );
        }
        break;

    case 5:
        name = "CollationFinnishTest"; 

        if (exec)
        {
            logln("CollationFinnishtest---");
            
            CollationFinnishTest test;
            callTest( test, par ); 
        }
        break;

    case 6:
        name = "CollationKanaTest"; 

        if (exec)
        {
            logln("CollationKanatest---");
            
            CollationKanaTest test;
            callTest( test, par );
        }
        break;

    case 7:
        name = "CollationTurkishTest";
        
        if (exec)
        {
            logln("CollationTurkishtest---");
            logln("");

            CollationTurkishTest test;
            callTest( test, par );
        }
        break;

    case 8:
        name = "CollationDummyTest"; 

        if (exec)
        {
            logln("CollationDummyTest---");
            
            CollationDummyTest test;
            callTest( test, par );
        }
        break;

    case 9:
        name = "G7CollationTest";
        
        if (exec)
        {
            logln("G7CollationTest---");
            
            G7CollationTest test;
            callTest( test, par );
        }
        break;

    case 10:
        name = "CollationMonkeyTest";
        
        if (exec)
        {
            logln("CollationMonkeyTest---");
            
            CollationMonkeyTest test;
            callTest( test, par );
        }
        break;

    case 11:
        name = "CollationAPITest";
        
        if (exec)
        {
            logln("CollationAPITest---");
            logln("");
            
            CollationAPITest test;
            callTest( test, par );
        }
        break;

    case 12:
        name = "CollationRegressionTest"; 

        if (exec)
        {
            logln("CollationRegressionTest---");
            
            CollationRegressionTest test;
            callTest( test, par );
        }
        break;

    case 13:
        name = "CollationCurrencyTest"; 

        if (exec)
        {
            logln("CollationCurrencyTest---");
            logln("");
            
            CollationCurrencyTest test;
            callTest( test, par );
        }
        break;

    case 14:
        name = "CollationIteratorTest"; 

        if (exec)
        {
            logln("CollationIteratorTest---");
            
            CollationIteratorTest test;
            callTest( test, par );
        }
        break;

    case 15: 
      /*        name = "CollationCAPITest"; 
        if (exec) {
            logln("Collation C API test---"); logln("");
            CollationCAPITest test;
            callTest( test, par );
        }
        break;

    case 16: */
        name = "CollationThaiTest"; 
        if (exec) {
            logln("CollationThaiTest---"); 
            
            CollationThaiTest test;
            callTest( test, par );
        }
        break;

    case 16: 
        name = "LotusCollationTest";

        name = "LotusCollationKoreanTest"; 
        if (exec) {
            logln("LotusCollationKoreanTest---"); logln("");
            LotusCollationKoreanTest test;
            callTest( test, par );
        }
        break;

    case 17:
        name = "StringSearchTest"; 
        if (exec) {
            logln("StringSearchTest---"); 
            
            StringSearchTest test;
            callTest( test, par );
        }
        break;

    case 18: 
        name = "ContractionTableTest";

        name = "ContractionTableTest"; 
        if (exec) {
            logln("ContractionTableTest---"); logln("");
            ContractionTableTest test;
            callTest( test, par );
        }
        break;

    case 19:
      name = "DataDrivenTest";
      if (exec) {
        logln("DataDrivenTest---"); logln("");
        DataDrivenCollatorTest test;
        callTest( test, par );
      }
      break;

    case 20:
      name = "UCAConformanceTest";
      if (exec) {
        logln("UCAConformanceTest---"); logln("");
        UCAConformanceTest test;
        callTest( test, par );
      }
      break;

    case 21:
      name = "CollationServiceTest";
      if (exec) {
        logln("CollationServiceTest---"); logln("");
        CollationServiceTest test;
        callTest( test, par );
      }
      break;

    default: name = ""; break;
    }
}

UCollationResult 
IntlTestCollator::compareUsingPartials(UCollator *coll, const UChar source[], int32_t sLen, const UChar target[], int32_t tLen, int32_t pieceSize, UErrorCode &status) {
  int32_t partialSKResult = 0;
  uint8_t sBuf[512], tBuf[512];
  UCharIterator sIter, tIter;
  uint32_t sState[2], tState[2];
  int32_t sSize = pieceSize, tSize = pieceSize;
  int32_t i = 0;
  status = U_ZERO_ERROR;
  sState[0] = 0; sState[1] = 0;
  tState[0] = 0; tState[1] = 0;
  while(sSize == pieceSize && tSize == pieceSize && partialSKResult == 0) {
    uiter_setString(&sIter, source, sLen);
    uiter_setString(&tIter, target, tLen);
    sSize = ucol_nextSortKeyPart(coll, &sIter, sState, sBuf, pieceSize, &status);
    tSize = ucol_nextSortKeyPart(coll, &tIter, tState, tBuf, pieceSize, &status);
    
    if(sState[0] != 0 || tState[0] != 0) {
      log("State != 0 : %08X %08X\n", sState[0], tState[0]);
    }
    log("%i ", i++);
    
    partialSKResult = memcmp(sBuf, tBuf, pieceSize);
  }

  if(partialSKResult < 0) {
      return UCOL_LESS;
  } else if(partialSKResult > 0) {
    return UCOL_GREATER;
  } else {
    return UCOL_EQUAL;
  }
}

void 
IntlTestCollator::doTestVariant(Collator* col, const UnicodeString &source, const UnicodeString &target, Collator::EComparisonResult result)
{   
  UErrorCode status = U_ZERO_ERROR;

  UCollator *myCollation = (UCollator *)((RuleBasedCollator *)col)->getUCollator();

  Collator::EComparisonResult compareResult = col->compare(source, target);

  CollationKey srckey, tgtkey;
  col->getCollationKey(source, srckey, status);
  col->getCollationKey(target, tgtkey, status);
  if (U_FAILURE(status)){
    errln("Creation of collation keys failed\n");
  }
  Collator::EComparisonResult keyResult = srckey.compareTo(tgtkey);

  reportCResult(source, target, srckey, tgtkey, compareResult, keyResult, result, result);

    UColAttributeValue norm = ucol_getAttribute(myCollation, UCOL_NORMALIZATION_MODE, &status);

    int32_t sLen = source.length(), tLen = target.length();
    const UChar* src = source.getBuffer();
    const UChar* trg = target.getBuffer();
    UCollationResult compareResultIter = (UCollationResult)result;

    if(!quick) {
      UCharIterator sIter, tIter;
      uiter_setString(&sIter, src, sLen);
      uiter_setString(&tIter, trg, tLen);
      compareResultIter = ucol_strcollIter(myCollation, &sIter, &tIter, &status);
      if(compareResultIter != (UCollationResult)result) {
        errln("Different result for iterative comparison "+source+" "+target);
      }
    }
    /* convert the strings to UTF-8 and do try comparing with char iterator */
    if(!quick) { /*!QUICK*/
      char utf8Source[256], utf8Target[256];
      int32_t utf8SourceLen = 0, utf8TargetLen = 0;
      u_strToUTF8(utf8Source, 256, &utf8SourceLen, src, sLen, &status);
      if(U_FAILURE(status)) { /* probably buffer is not big enough */
        log("Src UTF-8 buffer too small! Will not compare!\n");
      } else {
        u_strToUTF8(utf8Target, 256, &utf8TargetLen, trg, tLen, &status);
        if(U_SUCCESS(status)) { /* probably buffer is not big enough */
          UCollationResult compareResultUTF8 = (UCollationResult)result, compareResultUTF8Norm = (UCollationResult)result;
          UCharIterator sIter, tIter;
          /*log_verbose("Strings converted to UTF-8:%s, %s\n", aescstrdup(source,-1), aescstrdup(target,-1));*/
          uiter_setUTF8(&sIter, utf8Source, utf8SourceLen);
          uiter_setUTF8(&tIter, utf8Target, utf8TargetLen);
       /*uiter_setString(&sIter, source, sLen);
      uiter_setString(&tIter, target, tLen);*/
          compareResultUTF8 = ucol_strcollIter(myCollation, &sIter, &tIter, &status);
          ucol_setAttribute(myCollation, UCOL_NORMALIZATION_MODE, UCOL_ON, &status);
          sIter.move(&sIter, 0, UITER_START);
          tIter.move(&tIter, 0, UITER_START);
          compareResultUTF8Norm = ucol_strcollIter(myCollation, &sIter, &tIter, &status);
          ucol_setAttribute(myCollation, UCOL_NORMALIZATION_MODE, norm, &status);
          if(compareResultUTF8 != compareResultIter) {
            errln("different results in iterative comparison for UTF-16 and UTF-8 encoded strings. "+source+", "+target);
          }
          if(compareResultUTF8 != compareResultUTF8Norm) {
            errln("different results in iterative when normalization is turned on with UTF-8 strings. "+source+", "+target);
          }
        } else {
          log("Target UTF-8 buffer too small! Did not compare!\n");
        }
        if(U_FAILURE(status)) {
          log("UTF-8 strcoll failed! Ignoring result\n");
        }
      }
    }

    /* testing the partial sortkeys */
    if(1) { /*!QUICK*/
      int32_t partialSizes[] = { 3, 1, 2, 4, 8, 20, 80 }; /* just size 3 in the quick mode */
      int32_t partialSizesSize = 1;
      if(!quick) {
        partialSizesSize = 7;
      }
      int32_t i = 0;
      log("partial sortkey test piecesize=");
      for(i = 0; i < partialSizesSize; i++) {
        UCollationResult partialSKResult = (UCollationResult)result, partialNormalizedSKResult = (UCollationResult)result;
        log("%i ", partialSizes[i]);

        partialSKResult = compareUsingPartials(myCollation, src, sLen, trg, tLen, partialSizes[i], status);
        if(partialSKResult != (UCollationResult)result) {
          errln("Partial sortkey comparison returned wrong result: "+source+", "+target+" (size "+partialSizes[i]+")");           
        }

        if(norm != UCOL_ON && !quick) {
          log("N ");
          ucol_setAttribute(myCollation, UCOL_NORMALIZATION_MODE, UCOL_ON, &status);
          partialNormalizedSKResult = compareUsingPartials(myCollation, src, sLen, trg, tLen, partialSizes[i], status);
          ucol_setAttribute(myCollation, UCOL_NORMALIZATION_MODE, norm, &status);
          if(partialSKResult != partialNormalizedSKResult) {
            errln("Partial sortkey comparison gets different result when normalization is on: "+source+", "+target+" (size "+partialSizes[i]+")");           
          }
        }
      }
      log("\n");
    }
/*
  if (compareResult != result) {
    errln("String comparison failed in variant test\n");
  }
  if (keyResult != result) {
    errln("Collation key comparison failed in variant test\n");
  }
*/
}

void
IntlTestCollator::doTest(Collator* col, const UChar *source, const UChar *target, Collator::EComparisonResult result) {
  doTest(col, UnicodeString(source), UnicodeString(target), result);
}

void 
IntlTestCollator::doTest(Collator* col, const UnicodeString &source, const UnicodeString &target, Collator::EComparisonResult result)
{
  doTestVariant(col, source, target, result);
  if(result == Collator::LESS) {
    doTestVariant(col, target, source, Collator::GREATER);
  } else if (result == Collator::GREATER) {
    doTestVariant(col, target, source, Collator::LESS);
  }

  UErrorCode status = U_ZERO_ERROR;
  CollationElementIterator* c = ((RuleBasedCollator *)col)->createCollationElementIterator( source );
  logln("Testing iterating source: "+source);
  backAndForth(*c);
  c->setText(target, status);
  logln("Testing iterating target: "+target);
  backAndForth(*c);
  delete c;
}


// used for collation result reporting, defined here for convenience
// (maybe moved later)
void
IntlTestCollator::reportCResult( const UnicodeString &source, const UnicodeString &target,
             CollationKey &sourceKey, CollationKey &targetKey,
             Collator::EComparisonResult compareResult,
             Collator::EComparisonResult keyResult,
                                Collator::EComparisonResult incResult,
                         Collator::EComparisonResult expectedResult )
{
    if (expectedResult < -1 || expectedResult > 1)
    {
        errln("***** invalid call to reportCResult ****");
        return;
    }

    UBool ok1 = (compareResult == expectedResult);
    UBool ok2 = (keyResult == expectedResult);
    UBool ok3 = (incResult == expectedResult);


    if (ok1 && ok2 && ok3 && !verbose) {
        // Keep non-verbose, passing tests fast
        return;
    } else {
        UnicodeString msg1(ok1 ? "Ok: compare(" : "FAIL: compare(");
        UnicodeString msg2(", "), msg3(") returned "), msg4("; expected ");
        UnicodeString prettySource, prettyTarget, sExpect, sResult;

        IntlTest::prettify(source, prettySource);
        IntlTest::prettify(target, prettyTarget);
        appendCompareResult(compareResult, sResult);
        appendCompareResult(expectedResult, sExpect);

        if (ok1) {
            logln(msg1 + prettySource + msg2 + prettyTarget + msg3 + sResult);
        } else {
            errln(msg1 + prettySource + msg2 + prettyTarget + msg3 + sResult + msg4 + sExpect);
        }

        msg1 = UnicodeString(ok2 ? "Ok: key(" : "FAIL: key(");
        msg2 = ").compareTo(key(";
        msg3 = ")) returned ";

        appendCompareResult(keyResult, sResult);

        if (ok2) {
            logln(msg1 + prettySource + msg2 + prettyTarget + msg3 + sResult);
        } else {
            errln(msg1 + prettySource + msg2 + prettyTarget + msg3 + sResult + msg4 + sExpect);

            msg1 = "  ";
            msg2 = " vs. ";

            prettify(sourceKey, prettySource);
            prettify(targetKey, prettyTarget);

            errln(msg1 + prettySource + msg2 + prettyTarget);
        }
        msg1 = UnicodeString (ok3 ? "Ok: incCompare(" : "FAIL: incCompare(");
        msg2 = ", ";
        msg3 = ") returned ";

        appendCompareResult(incResult, sResult);

        if (ok3) {
            logln(msg1 + prettySource + msg2 + prettyTarget + msg3 + sResult);
        } else {
            errln(msg1 + prettySource + msg2 + prettyTarget + msg3 + sResult + msg4 + sExpect);
        }
    }
}

UnicodeString&
IntlTestCollator::appendCompareResult(Collator::EComparisonResult result,
                  UnicodeString& target)
{
    if (result == Collator::LESS)
    {
        target += "LESS";
    }
    else if (result == Collator::EQUAL)
    {
        target += "EQUAL";
    }
    else if (result == Collator::GREATER)
    {
        target += "GREATER";
    }
    else
    {
        UnicodeString huh = "?";

        target += (huh + (int32_t)result);
    }

    return target;
}

// Produce a printable representation of a CollationKey
UnicodeString &IntlTestCollator::prettify(const CollationKey &source, UnicodeString &target)
{
    int32_t i, byteCount;
    const uint8_t *bytes = source.getByteArray(byteCount);

    target.remove();
    target += "[";

    for (i = 0; i < byteCount; i += 1)
    {
        appendHex(bytes[i], 2, target);
        target += " ";
    }

    target += "]";

    return target;
}

void IntlTestCollator::backAndForth(CollationElementIterator &iter)
{
    // Run through the iterator forwards and stick it into an array
    int32_t orderLength = 0;
    int32_t *orders = getOrders(iter, orderLength);
    UErrorCode status = U_ZERO_ERROR;

    // Now go through it backwards and make sure we get the same values
    int32_t index = orderLength;
    int32_t o;

    // reset the iterator
    iter.reset();

    while ((o = iter.previous(status)) != CollationElementIterator::NULLORDER)
    {
        if (o != orders[--index])
        {
            if (o == 0)
                index ++;
            else
            {
                while (index > 0 && orders[--index] == 0)
                {
                }
                if (o != orders[index])
                {
                    errln("Mismatch at index %d: 0x%X vs 0x%X", index,
                        orders[index], o);
                    break;
                }
            }
        }
    }

    while (index != 0 && orders[index - 1] == 0)
    {
      index --;
    }

    if (index != 0)
    {
        UnicodeString msg("Didn't get back to beginning - index is ");
        errln(msg + index);

        iter.reset();
        err("next: ");
        while ((o = iter.next(status)) != CollationElementIterator::NULLORDER)
        {
            UnicodeString hexString("0x");

            appendHex(o, 8, hexString);
            hexString += " ";
            err(hexString);
        }
        errln("");

        err("prev: ");
        while ((o = iter.previous(status)) != CollationElementIterator::NULLORDER)
        {
            UnicodeString hexString("0x");

            appendHex(o, 8, hexString);
            hexString += " ";
             err(hexString);
        }
        errln("");
    }

    delete[] orders;
}


/**
 * Return an integer array containing all of the collation orders
 * returned by calls to next on the specified iterator
 */
int32_t *IntlTestCollator::getOrders(CollationElementIterator &iter, int32_t &orderLength)
{
    int32_t maxSize = 100;
    int32_t size = 0;
    int32_t *orders = new int32_t[maxSize];
    UErrorCode status = U_ZERO_ERROR;

    int32_t order;
    while ((order = iter.next(status)) != CollationElementIterator::NULLORDER)
    {
        if (size == maxSize)
        {
            maxSize *= 2;
            int32_t *temp = new int32_t[maxSize];

            uprv_memcpy(temp, orders, size * sizeof(int32_t));
            delete[] orders;
            orders = temp;
        }

        orders[size++] = order;
    }

    if (maxSize > size)
    {
        int32_t *temp = new int32_t[size];

        uprv_memcpy(temp, orders, size * sizeof(int32_t));
        delete[] orders;
        orders = temp;
    }

    orderLength = size;
    return orders;
}

#endif /* #if !UCONFIG_NO_COLLATION */
