/*
******************************************************************************
*                                                                            *
* Copyright (C) 2001-2001, International Business Machines                   *
*                Corporation and others. All Rights Reserved.                *
*                                                                            *
******************************************************************************
*   file name:  ucln_cmn.c
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2001July05
*   created by: George Rhoten
*/

#include "unicode/uclean.h"
#include "ucln_cmn.h"
#include "umutex.h"

void u_cleanup(void)
{
    ucln_uloc();
    ucln_ustring();
    ucln_ucnv();
    ucln_ures();
}