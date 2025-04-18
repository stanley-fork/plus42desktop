/*****************************************************************************
 * Plus42 -- an enhanced HP-42S calculator simulator
 * Copyright (C) 2004-2025  Thomas Okken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#ifndef CORE_STO_RCL_H
#define CORE_STO_RCL_H 1


#include "free42.h"
#include "core_phloat.h"
#include "core_globals.h"


/************************************************/
/* Signatures for functions mapped by map_unary */
/************************************************/

typedef int (*mappable_r)(phloat x, phloat *z);
typedef int (*mappable_c)(phloat xre, phloat xim, phloat *zre, phloat *zim);


/*************************************************/
/* Signatures for functions mapped by map_binary */
/*************************************************/

typedef int (*mappable_rr)(phloat x, phloat y, phloat *z);
typedef int (*mappable_rc)(phloat x, phloat yre, phloat yim,
                                                phloat *zre, phloat *zim);
typedef int (*mappable_cr)(phloat xre, phloat xim, phloat y,
                                                phloat *zre, phloat *zim);
typedef int (*mappable_cc)(phloat xre, phloat xim, phloat yre, phloat yim,
                                                phloat *zre, phloat *zim);


/****************************************************************/
/* Generic arithmetic operators, for use in the implementations */
/* of +, -, *, /, STO+, STO-, etc...                            */
/****************************************************************/

int assert_numeric(const vartype *v);
int generic_div(const vartype *x, const vartype *y,
                            int (*completion)(int, vartype *));
int generic_mul(const vartype *x, const vartype *y,
                            int (*completion)(int, vartype *));
int generic_sub(const vartype *x, const vartype *y, vartype **res);
int generic_add(const vartype *x, const vartype *y, vartype **res);
int generic_rcl(arg_struct *arg, vartype **dst, bool must_be_writable = false);
int generic_sto(arg_struct *arg, char operation);


/**********************************************/
/* Mappers to apply unary or binary operators */
/* to arbitrary parameter types               */
/**********************************************/

int map_unary(const vartype *src, vartype **dst, mappable_r mr, mappable_c mc,
            bool do_units = false);
int map_binary(const vartype *src1, const vartype *src2, vartype **dst,
            mappable_rr mrr, mappable_rc mrc, mappable_cr mcr, mappable_cc mcc);

#endif
