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

#ifndef CORE_COMMANDS5_H
#define CORE_COMMANDS5_H 1

#include "free42.h"
#include "core_globals.h"

int docmd_linf(arg_struct *arg);
int docmd_logf(arg_struct *arg);
int docmd_expf(arg_struct *arg);
int docmd_pwrf(arg_struct *arg);
int docmd_allsigma(arg_struct *arg);
int docmd_best(arg_struct *arg);
int docmd_corr(arg_struct *arg);
int docmd_fcstx(arg_struct *arg);
int docmd_fcsty(arg_struct *arg);
int docmd_mean(arg_struct *arg);
int docmd_sdev(arg_struct *arg);
int docmd_slope(arg_struct *arg);
int docmd_sum(arg_struct *arg);
int docmd_wmean(arg_struct *arg);
int docmd_yint(arg_struct *arg);

int docmd_integ(arg_struct *arg);
int docmd_linsigma(arg_struct *arg);
int docmd_pgmslv(arg_struct *arg);
int docmd_eqnslv(arg_struct *arg);
int docmd_pgmint(arg_struct *arg);
int docmd_eqnint(arg_struct *arg);
int docmd_pgmslvi(arg_struct *arg);
int docmd_eqnslvi(arg_struct *arg);
int docmd_pgminti(arg_struct *arg);
int docmd_eqninti(arg_struct *arg);

int docmd_a_thru_f(arg_struct *arg);
int docmd_binm(arg_struct *arg);
int docmd_octm(arg_struct *arg);
int docmd_decm(arg_struct *arg);
int docmd_hexm(arg_struct *arg);
int docmd_and(arg_struct *arg);
int docmd_or(arg_struct *arg);
int docmd_xor(arg_struct *arg);
int docmd_not(arg_struct *arg);
int docmd_bit_t(arg_struct *arg);
int docmd_rotxy(arg_struct *arg);
int docmd_baseadd(arg_struct *arg);
int docmd_basesub(arg_struct *arg);
int docmd_basemul(arg_struct *arg);
int docmd_basediv(arg_struct *arg);
int docmd_basechs(arg_struct *arg);

int docmd_sl(arg_struct *arg);
int docmd_sr(arg_struct *arg);
int docmd_asr(arg_struct *arg);
int docmd_sln(arg_struct *arg);
int docmd_srn(arg_struct *arg);
int docmd_asrn(arg_struct *arg);
int docmd_lj(arg_struct *arg);
int docmd_rj(arg_struct *arg);
int docmd_rl(arg_struct *arg);
int docmd_rr(arg_struct *arg);
int docmd_rlc(arg_struct *arg);
int docmd_rrc(arg_struct *arg);
int docmd_rln(arg_struct *arg);
int docmd_rrn(arg_struct *arg);
int docmd_rlcn(arg_struct *arg);
int docmd_rrcn(arg_struct *arg);
int docmd_sb(arg_struct *arg);
int docmd_cb(arg_struct *arg);
int docmd_b_t(arg_struct *arg);
int docmd_num_b(arg_struct *arg);
int docmd_maskl(arg_struct *arg);
int docmd_maskr(arg_struct *arg);
int docmd_sc(arg_struct *arg);
int docmd_cc(arg_struct *arg);
int docmd_c_t(arg_struct *arg);
int docmd_n_to_bs(arg_struct *arg);
int docmd_n_to_bd(arg_struct *arg);
int docmd_n_to_bq(arg_struct *arg);
int docmd_n_to_ds(arg_struct *arg);
int docmd_n_to_dd(arg_struct *arg);
int docmd_n_to_dq(arg_struct *arg);
int docmd_bs_to_n(arg_struct *arg);
int docmd_bd_to_n(arg_struct *arg);
int docmd_bq_to_n(arg_struct *arg);
int docmd_ds_to_n(arg_struct *arg);
int docmd_dd_to_n(arg_struct *arg);
int docmd_dq_to_n(arg_struct *arg);
int docmd_wsize(arg_struct *arg);
int docmd_wsize_t(arg_struct *arg);
int docmd_bsigned(arg_struct *arg);
int docmd_bwrap(arg_struct *arg);
int docmd_breset(arg_struct *arg);
int docmd_decint(arg_struct *arg);
int docmd_binsep(arg_struct *arg);
int docmd_octsep(arg_struct *arg);
int docmd_decsep(arg_struct *arg);
int docmd_hexsep(arg_struct *arg);

int docmd_solve(arg_struct *arg);
int docmd_vmsolve(arg_struct *arg);
int docmd_to_dec(arg_struct *arg);
int docmd_to_oct(arg_struct *arg);
int docmd_sigmaadd(arg_struct *arg);
int docmd_sigmasub(arg_struct *arg);

#endif
