/*
*******************************************************************************
*
*   Copyright (C) 2001, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  ucol_tok.cpp
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created 02/22/2001
*   created by: Vladimir Weinstein
*
* This module maintains a contraction table structure in expanded form
* and provides means to flatten this structure
* 
*/

#ifndef UCOL_CNTTABLE_H
#define UCOL_CNTTABLE_H

#include "ucmp32.h"
#include "uhash.h"
#include "ucol_elm.h"

#define UPRV_CNTTAB_NEWELEMENT 0xFFFFFF

typedef struct ContractionTable ContractionTable;
struct ContractionTable {
    UChar *codePoints;
    uint32_t *CEs;
    int32_t position;
    int32_t size;
    int32_t backSize;
    UBool forward;
    ContractionTable *reversed;
};

struct CntTable {
	ContractionTable **elements;
    CompactIntArray *mapping;
    UChar *codePoints;
    uint32_t *CEs;
    int32_t *offsets;
    int32_t position;
    int32_t size;
    int32_t capacity;
};

CntTable *uprv_cnttab_open(CompactIntArray *mapping, UErrorCode *status);
void uprv_cnttab_close(CntTable *table);

/* construct the table for output */
int32_t uprv_cnttab_constructTable(CntTable *table, uint32_t mainOffset, UErrorCode *status); 
/* moves table around... since we have absolute offsets from the start of the table */
int32_t uprv_cnttab_moveTable(CntTable *table, uint32_t oldOffset, uint32_t newOffset, UErrorCode *status);
/* adds more contractions in table. If element is non existant, it creates on. Returns element handle */
uint32_t uprv_cnttab_addContraction(CntTable *table, uint32_t element, UChar codePoint, uint32_t value, UBool forward, UErrorCode *status);
/* sets a part of contraction sequence in table. If element is non existant, it creates on. Returns element handle */
uint32_t uprv_cnttab_setContraction(CntTable *table, uint32_t element, int32_t offset, UChar codePoint, uint32_t value, UBool forward, UErrorCode *status);
/* inserts a part of contraction sequence in table. Sequences behind the offset are moved back. If element is non existant, it creates on. Returns element handle */
uint32_t uprv_cnttab_insertContraction(CntTable *table, uint32_t element, UChar codePoint, uint32_t value, UBool forward, UErrorCode *status);
/* this is for adding non contractions */
uint32_t uprv_cnttab_changeLastCE(CntTable *table, uint32_t element, uint32_t value, UBool forward, UErrorCode *status);

uint32_t uprv_cnttab_findCP(CntTable *table, uint32_t element, UChar codePoint, UBool forward, UErrorCode *status);

uint32_t uprv_cnttab_getCE(CntTable *table, uint32_t element, int32_t position, UBool forward, UErrorCode *status);
uint32_t uprv_cnttab_changeContraction(CntTable *table, uint32_t element, UChar codePoint, uint32_t newCE, UBool forward, UErrorCode *status);

#endif
