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

#include <stdlib.h>

#include "core_globals.h"
#include "core_linalg1.h"
#include "core_linalg2.h"
#include "core_main.h"
#include "core_math2.h"
#include "core_sto_rcl.h"
#include "core_variables.h"


/**********************************/
/***** Matrix-matrix division *****/
/**********************************/

static int (*linalg_div_completion)(int, vartype *);
static const vartype *linalg_div_left;
static vartype *linalg_div_result;

static int div_rr_completion1(int error, vartype_realmatrix *a, int4 *perm,
                                    phloat det);
static int div_rr_completion2(int error, vartype_realmatrix *a, int4 *perm,
                                    vartype_realmatrix *b);
static int div_rc_completion1(int error, vartype_complexmatrix *a, int4 *perm,
                                    phloat det_re, phloat det_im);
static int div_rc_completion2(int error, vartype_complexmatrix *a, int4 *perm,
                                    vartype_complexmatrix *b);
static int div_cr_completion1(int error, vartype_realmatrix *a, int4 *perm,
                                    phloat det);
static int div_cr_completion2(int error, vartype_realmatrix *a, int4 *perm,
                                    vartype_complexmatrix *b);
static int div_cc_completion1(int error, vartype_complexmatrix *a, int4 *perm,
                                    phloat det_re, phloat det_im);
static int div_cc_completion2(int error, vartype_complexmatrix *a, int4 *perm,
                                    vartype_complexmatrix *b);
static int small_div(const vartype *left, const vartype *right,
                                    int (*completion)(int, vartype *));

int linalg_div(const vartype *left, const vartype *right,
                                    int (*completion)(int, vartype *)) {
    if (left->type == TYPE_REALMATRIX) {
        if (right->type == TYPE_REALMATRIX) {
            vartype_realmatrix *num = (vartype_realmatrix *) left;
            vartype_realmatrix *denom = (vartype_realmatrix *) right;
            vartype *lu, *res;
            int4 rows = num->rows;
            int4 columns = num->columns;
            int4 *perm;
            if (denom->rows != rows || denom->columns != rows)
                return completion(ERR_DIMENSION_ERROR, NULL);
            if (denom->rows <= 2)
                return small_div(left, right, completion);
            perm = (int4 *) malloc(rows * sizeof(int4));
            if (perm == NULL)
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            lu = new_realmatrix(rows, rows);
            if (lu == NULL) {
                free(perm);
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            }
            res = new_realmatrix(rows, columns);
            if (res == NULL) {
                free(perm);
                free_vartype(lu);
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            }
            matrix_copy(lu, right);
            linalg_div_completion = completion;
            linalg_div_left = left;
            linalg_div_result = res;
            return lu_decomp_r((vartype_realmatrix *) lu, perm,
                                                div_rr_completion1);
        } else {
            vartype_realmatrix *num = (vartype_realmatrix *) left;
            vartype_complexmatrix *denom = (vartype_complexmatrix *) right;
            vartype *lu, *res;
            int4 rows = num->rows;
            int4 columns = num->columns;
            int4 *perm;
            if (denom->rows != rows || denom->columns != rows)
                return completion(ERR_DIMENSION_ERROR, NULL);
            if (denom->rows <= 2)
                return small_div(left, right, completion);
            perm = (int4 *) malloc(rows * sizeof(int4));
            if (perm == NULL)
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            lu = new_complexmatrix(rows, rows);
            if (lu == NULL) {
                free(perm);
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            }
            res = new_complexmatrix(rows, columns);
            if (res == NULL) {
                free(perm);
                free_vartype(lu);
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            }
            matrix_copy(lu, right);
            linalg_div_completion = completion;
            linalg_div_left = left;
            linalg_div_result = res;
            return lu_decomp_c((vartype_complexmatrix *) lu, perm,
                                                div_rc_completion1);
        }
    } else {
        if (right->type == TYPE_REALMATRIX) {
            vartype_complexmatrix *num = (vartype_complexmatrix *) left;
            vartype_realmatrix *denom = (vartype_realmatrix *) right;
            vartype *lu, *res;
            int4 rows = num->rows;
            int4 columns = num->columns;
            int4 *perm;
            if (denom->rows != rows || denom->columns != rows)
                return completion(ERR_DIMENSION_ERROR, 0);
            if (denom->rows <= 2)
                return small_div(left, right, completion);
            perm = (int4 *) malloc(rows * sizeof(int4));
            if (perm == NULL)
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            lu = new_realmatrix(rows, rows);
            if (lu == NULL) {
                free(perm);
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            }
            res = new_complexmatrix(rows, columns);
            if (res == NULL) {
                free(perm);
                free_vartype(lu);
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            }
            matrix_copy(lu, right);
            linalg_div_completion = completion;
            linalg_div_left = left;
            linalg_div_result = res;
            return lu_decomp_r((vartype_realmatrix *) lu, perm,
                                                    div_cr_completion1);
        } else {
            vartype_complexmatrix *num = (vartype_complexmatrix *) left;
            vartype_complexmatrix *denom = (vartype_complexmatrix *) right;
            vartype *lu, *res;
            int4 rows = num->rows;
            int4 columns = num->columns;
            int4 *perm;
            if (denom->rows != rows || denom->columns != rows)
                return completion(ERR_DIMENSION_ERROR, NULL);
            if (denom->rows <= 2)
                return small_div(left, right, completion);
            perm = (int4 *) malloc(rows * sizeof(int4));
            if (perm == NULL)
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            lu = new_complexmatrix(rows, rows);
            if (lu == NULL) {
                free(perm);
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            }
            res = new_complexmatrix(rows, columns);
            if (res == NULL) {
                free(perm);
                free_vartype(lu);
                return completion(ERR_INSUFFICIENT_MEMORY, NULL);
            }
            matrix_copy(lu, right);
            linalg_div_completion = completion;
            linalg_div_left = left;
            linalg_div_result = res;
            return lu_decomp_c((vartype_complexmatrix *) lu, perm,
                                                    div_cc_completion1);
        }
    }
}

static int div_rr_completion1(int error, vartype_realmatrix *a, int4 *perm,
                                         phloat det) {
    if (error != ERR_NONE) {
        free_vartype((vartype *) a);
        free(perm);
        free_vartype(linalg_div_result);
        return error;
    } else {
        matrix_copy(linalg_div_result, linalg_div_left);
        return lu_backsubst_rr(a, perm,
                                (vartype_realmatrix *) linalg_div_result,
                                div_rr_completion2);
    }
}

static int div_rr_completion2(int error, vartype_realmatrix *a, int4 *perm,
                                          vartype_realmatrix *b) {
    if (error != ERR_NONE)
        free_vartype(linalg_div_result); /* Note: linalg_div_result == b */
    free_vartype((vartype *) a);
    free(perm);
    return linalg_div_completion(error, linalg_div_result);
}

static int div_rc_completion1(int error, vartype_complexmatrix *a, int4 *perm,
                                         phloat det_re, phloat det_im) {
    if (error != ERR_NONE) {
        free_vartype((vartype *) a);
        free(perm);
        free_vartype(linalg_div_result);
        return error;
    } else {
        matrix_copy(linalg_div_result, linalg_div_left);
        return lu_backsubst_cc(a, perm,
                                (vartype_complexmatrix *) linalg_div_result,
                                div_rc_completion2);
    }
}

static int div_rc_completion2(int error, vartype_complexmatrix *a, int4 *perm,
                                          vartype_complexmatrix *b) {
    if (error != ERR_NONE)
        free_vartype(linalg_div_result); /* Note: linalg_div_result == b */
    free_vartype((vartype *) a);
    free(perm);
    return linalg_div_completion(error, linalg_div_result);
}

static int div_cr_completion1(int error, vartype_realmatrix *a, int4 *perm,
                                    phloat det) {
    if (error != ERR_NONE) {
        free_vartype((vartype *) a);
        free(perm);
        free_vartype(linalg_div_result);
        return error;
    } else {
        matrix_copy(linalg_div_result, linalg_div_left);
        return lu_backsubst_rc(a, perm,
                                (vartype_complexmatrix *) linalg_div_result,
                                div_cr_completion2);
    }
}

static int div_cr_completion2(int error, vartype_realmatrix *a, int4 *perm,
                                    vartype_complexmatrix *b) {
    if (error != ERR_NONE)
        free_vartype(linalg_div_result); /* Note: linalg_div_result == b */
    free_vartype((vartype *) a);
    free(perm);
    return linalg_div_completion(error, linalg_div_result);
}

static int div_cc_completion1(int error, vartype_complexmatrix *a, int4 *perm,
                                    phloat det_re, phloat det_im) {
    if (error != ERR_NONE) {
        free_vartype((vartype *) a);
        free(perm);
        free_vartype(linalg_div_result);
        return error;
    } else {
        matrix_copy(linalg_div_result, linalg_div_left);
        return lu_backsubst_cc(a, perm,
                                (vartype_complexmatrix *) linalg_div_result,
                                div_cc_completion2);
    }
}

static int div_cc_completion2(int error, vartype_complexmatrix *a, int4 *perm,
                                    vartype_complexmatrix *b) {
    if (error != ERR_NONE)
        free_vartype(linalg_div_result); /* Note: linalg_div_result == b */
    free_vartype((vartype *) a);
    free(perm);
    return linalg_div_completion(error, linalg_div_result);
}

static int small_inv_r(vartype_realmatrix *ma, void (*completion)(vartype *));
static int small_inv_c(vartype_complexmatrix *ma, void (*completion)(vartype *));
static int matrix_mul_rr(vartype_realmatrix *left, vartype_realmatrix *right, int (*completion)(int, vartype *));
static int matrix_mul_cr(vartype_complexmatrix *left, vartype_realmatrix *right, int (*completion)(int, vartype *));
static int matrix_mul_rc(vartype_realmatrix *left, vartype_complexmatrix *right, int (*completion)(int, vartype *));
static int matrix_mul_cc(vartype_complexmatrix *left, vartype_complexmatrix *right, int (*completion)(int, vartype *));

static vartype *small_div_res;
static int (*small_div_completion)(int, vartype *);

static void small_div_completion_1(vartype *v) {
    small_div_res = v;
}

static int small_div_completion_2(int err, vartype *v) {
    free_vartype(small_div_res);
    return small_div_completion(err, v);
}

static int small_div(const vartype *left, const vartype *right, int (*completion)(int, vartype *)) {
    int err;
    if (right->type == TYPE_REALMATRIX) {
        err = small_inv_r((vartype_realmatrix *) right, small_div_completion_1);
        if (err != ERR_NONE)
            return completion(err, NULL);
        small_div_completion = completion;
        if (left->type == TYPE_REALMATRIX)
            return matrix_mul_rr((vartype_realmatrix *) small_div_res, (vartype_realmatrix *) left, small_div_completion_2);
        else
            return matrix_mul_rc((vartype_realmatrix *) small_div_res, (vartype_complexmatrix *) left, small_div_completion_2);
    } else {
        err = small_inv_c((vartype_complexmatrix *) right, small_div_completion_1);
        if (err != ERR_NONE)
            return completion(err, NULL);
        small_div_completion = completion;
        if (left->type == TYPE_REALMATRIX)
            return matrix_mul_cr((vartype_complexmatrix *) small_div_res, (vartype_realmatrix *) left, small_div_completion_2);
        else
            return matrix_mul_cc((vartype_complexmatrix *) small_div_res, (vartype_complexmatrix *) left, small_div_completion_2);
    }
}


/****************************************/
/***** Matrix-matrix multiplication *****/
/****************************************/

struct mul_rr_data_struct {
    vartype_realmatrix *left;
    vartype_realmatrix *right;
    vartype *result;
    int4 i, j, k;
    phloat sum;
    int (*completion)(int error, vartype *result);
};

static mul_rr_data_struct *mul_rr_data;

static int matrix_mul_rr_worker(bool interrupted);

static int matrix_mul_rr(vartype_realmatrix *left, vartype_realmatrix *right,
                         int (*completion)(int, vartype *)) {

    mul_rr_data_struct *dat;
    int error;

    if (left->columns != right->rows) {
        error = ERR_DIMENSION_ERROR;
        goto finished;
    }

    if (contains_strings(left) || contains_strings(right)) {
        error = ERR_ALPHA_DATA_IS_INVALID;
        goto finished;
    }

    dat = (mul_rr_data_struct *) malloc(sizeof(mul_rr_data_struct));
    if (dat == NULL) {
        error = ERR_INSUFFICIENT_MEMORY;
        goto finished;
    }

    dat->result = new_realmatrix(left->rows, right->columns);
    if (dat->result == NULL) {
        free(dat);
        error = ERR_INSUFFICIENT_MEMORY;
        goto finished;
    }

    dat->left = left;
    dat->right = right;
    dat->i = 0;
    dat->j = 0;
    dat->k = 0;
    dat->sum = 0;
    dat->completion = completion;

    mul_rr_data = dat;
    mode_interruptible = matrix_mul_rr_worker;
    mode_stoppable = false;
    return ERR_INTERRUPTIBLE;

    finished:
    return completion(error, NULL);
}

static int matrix_mul_rr_worker(bool interrupted) {
    mul_rr_data_struct *dat = mul_rr_data;
    int count = 0;
    int inf;
    phloat *l = dat->left->array->data;
    phloat *r = dat->right->array->data;
    phloat *p = ((vartype_realmatrix *) dat->result)->array->data;
    int4 i = dat->i;
    int4 j = dat->j;
    int4 k = dat->k;
    int4 m = dat->left->rows;
    int4 n = dat->right->columns;
    int4 q = dat->left->columns;
    phloat sum = dat->sum;

    if (interrupted) {
        int err = dat->completion(ERR_INTERRUPTED, NULL);
        free_vartype(dat->result);
        free(dat);
        return err;
    }

    while (count++ < 1000) {
        sum += l[i * q + k] * r[k * n + j];
        if (++k < q)
            continue;
        k = 0;
        if ((inf = p_isinf(sum)) != 0) {
            if (core_settings.matrix_outofrange && !flags.f.range_error_ignore){
                int err = dat->completion(ERR_OUT_OF_RANGE, NULL);
                free_vartype(dat->result);
                free(dat);
                return err;
            } else
                sum = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
        }
        p[i * n + j] = sum;
        sum = 0;
        if (++j < n)
            continue;
        j = 0;
        if (++i < m)
            continue;
        else {
            int err = dat->completion(ERR_NONE, dat->result);
            free(dat);
            return err;
        }
    }

    dat->i = i;
    dat->j = j;
    dat->k = k;
    dat->sum = sum;
    return ERR_INTERRUPTIBLE;
}

#if 0
/* TODO: Blocked matrix multiplication
 * This commented-out function implements a working blocked matrix
 * multiplication algorithm. It works by breaking up the multiplication into
 * multiplications of submatrices of the multiplicands. If the submatrices are
 * sufficiently small, they can be made to fit entirely in the CPU's L1 cache.
 * With block sizes of around 45, I have observed a speed-up factor of circa
 * 1.7 on a Pentium MMX, and with block sizes of about 90, I have observed a
 * speed-up factor of circa 3 on a Duron. (Those were the optimum block sizes
 * observed for each processor.) These numbers suggest the Pentium MMX has a 32
 * kilobyte L1 cache, and the Duron has a 128 kilobyte L1 cache.
 * For production use, the block size should be a user-configurable parameter,
 * with a built-in tool to automatically determine the optimum value. Since the
 * UI for this kind of advanced configuration does not exist yet, and sice the
 * determination of optimum block size is a very slow process, and since I
 * expect most HP-42S users will not be very interested in using a calculator
 * to work with order-100+ matrices, and since even in the optimum case the
 * speed-up is not staggering, I'm putting full implementation of this feature
 * off until some later date.
 */
static int matrix_mul_rr(vartype_realmatrix *left, vartype_realmatrix *right,
                         vartype **result) {
    phloat *cache = NULL;
    phloat *leftcache, *rightcache;

    phloat *l = left->array->data;
    phloat *r = right->array->data;
    phloat *p;
    int inf;
    int4 m, n, q;
    int4 i, j, k;

    q = left->columns;
    if (q != right->rows)
        return ERR_DIMENSION_ERROR;
    m = left->rows;
    n = right->columns;

    if (contains_strings(left) || contains_strings(right))
        return ERR_ALPHA_DATA_IS_INVALID;

    *result = new_realmatrix(m, n);
    if (*result == NULL)
        return ERR_INSUFFICIENT_MEMORY;
    p = ((vartype_realmatrix *) *result)->array->data;

    if (BLOCK_SIZE > 0) {
        int4 cachesize = 2 * BLOCK_SIZE * BLOCK_SIZE;
        int4 alignment = CACHE_ALIGNMENT;
        int4 misalgn;
        if (alignment == 0)
            alignment = 1;
        misalgn = alignment % sizeof(phloat);
        if (misalgn != 0)
            alignment += sizeof(phloat) - misalgn;

        cache = (phloat *) malloc(cachesize * sizeof(phloat) + alignment);
        if (cache != NULL) {
            misalgn = ((unsigned long) cache) % alignment;
            if (misalgn == 0)
                leftcache = cache;
            else
                leftcache = cache + (alignment - misalgn) / sizeof(phloat);
            rightcache = leftcache + BLOCK_SIZE * BLOCK_SIZE;
        }
    }

    if (cache == NULL) {

        /* Falling back on basic i,j,k algorithm */
        for (i = 0; i < m; i++)
            for (j = 0; j < n; j++) {
                phloat sum = 0;
                for (k = 0; k < q; k++)
                    sum += l[i * q + k] * r[k * n + j];
                if ((inf = p_isinf(sum)) != 0) {
                    if (core_settings.matrix_outofrange
                                            && !flags.f.range_error_ignore)
                        return ERR_OUT_OF_RANGE;
                    else
                        sum = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
                }
                p[i * n + j] = sum;
            }
        return ERR_NONE;

    } else {

        /* Blocked i,j,k algorithm for optimal cache utilization */
        int4 i, j, k, ii, jj, kk;
        for (i = 0; i < m; i += BLOCK_SIZE) {
            int4 iimax = m - i;
            if (iimax > BLOCK_SIZE)
                iimax = BLOCK_SIZE;
            for (j = 0; j < n; j += BLOCK_SIZE) {
                int4 jjmax = n - j;
                if (jjmax > BLOCK_SIZE)
                    jjmax = BLOCK_SIZE;
                for (k = 0; k < q; k += BLOCK_SIZE) {
                    int4 kkmax = q - k;
                    if (kkmax > BLOCK_SIZE)
                        kkmax = BLOCK_SIZE;
                    for (ii = 0; ii < iimax; ii++)
                        for (kk = 0; kk < kkmax; kk++)
                            leftcache[ii * BLOCK_SIZE + kk]
                                = l[(ii + i) * q + (kk + k)];
                    for (kk = 0; kk < kkmax; kk++)
                        for (jj = 0; jj < jjmax; jj++)
                            rightcache[kk * BLOCK_SIZE + jj]
                                = r[(kk + k) * n + (jj + j)];
                    for (ii = 0; ii < iimax; ii++)
                        for (jj = 0; jj < jjmax; jj++) {
                            phloat sum = p[(ii + i) * n + (jj + j)];
                            for (kk = 0; kk < kkmax; kk++)
                                sum += leftcache[ii * BLOCK_SIZE + kk]
                                        * rightcache[kk * BLOCK_SIZE + jj];
                            if ((inf = p_isinf(sum)) != 0) {
                                if (core_settings.matrix_outofrange
                                            && !flags.f.range_error_ignore) {
                                    free(cache);
                                    return ERR_OUT_OF_RANGE;
                                } else
                                    sum = inf < 0 ? NEG_HUGE_PHLOAT
                                                  : POS_HUGE_PHLOAT;
                            }
                            p[(ii + i) * n + (jj + j)] = sum;
                        }
                }
            }
        }
        free(cache);
        return ERR_NONE;

    }
}
#endif

struct mul_rc_data_struct {
    vartype_realmatrix *left;
    vartype_complexmatrix *right;
    vartype *result;
    int4 i, j, k;
    phloat sum_re, sum_im;
    int (*completion)(int error, vartype *result);
};

static mul_rc_data_struct *mul_rc_data;

static int matrix_mul_rc_worker(bool interrupted);

static int matrix_mul_rc(vartype_realmatrix *left, vartype_complexmatrix *right,
                         int (*completion)(int, vartype *)) {

    mul_rc_data_struct *dat;
    int error;

    if (left->columns != right->rows) {
        error = ERR_DIMENSION_ERROR;
        goto finished;
    }

    if (contains_strings(left)) {
        error = ERR_ALPHA_DATA_IS_INVALID;
        goto finished;
    }

    dat = (mul_rc_data_struct *) malloc(sizeof(mul_rc_data_struct));
    if (dat == NULL) {
        error = ERR_INSUFFICIENT_MEMORY;
        goto finished;
    }

    dat->result = new_complexmatrix(left->rows, right->columns);
    if (dat->result == NULL) {
        free(dat);
        error = ERR_INSUFFICIENT_MEMORY;
        goto finished;
    }

    dat->left = left;
    dat->right = right;
    dat->i = 0;
    dat->j = 0;
    dat->k = 0;
    dat->sum_re = 0;
    dat->sum_im = 0;
    dat->completion = completion;

    mul_rc_data = dat;
    mode_interruptible = matrix_mul_rc_worker;
    mode_stoppable = false;
    return ERR_INTERRUPTIBLE;

    finished:
    return completion(error, NULL);
}

static int matrix_mul_rc_worker(bool interrupted) {
    mul_rc_data_struct *dat = mul_rc_data;
    int count = 0;
    int inf;
    phloat *l = dat->left->array->data;
    phloat *r = dat->right->array->data;
    phloat *p = ((vartype_complexmatrix *) dat->result)->array->data;
    int4 i = dat->i;
    int4 j = dat->j;
    int4 k = dat->k;
    int4 m = dat->left->rows;
    int4 n = dat->right->columns;
    int4 q = dat->left->columns;
    phloat sum_re = dat->sum_re;
    phloat sum_im = dat->sum_im;

    if (interrupted) {
        int err = dat->completion(ERR_INTERRUPTED, NULL);
        free_vartype(dat->result);
        free(dat);
        return err;
    }

    while (count++ < 1000) {
        phloat tmp = l[i * q + k];
        sum_re += tmp * r[2 * (k * n + j)];
        sum_im += tmp * r[2 * (k * n + j) + 1];
        if (++k < q)
            continue;
        k = 0;
        if ((inf = p_isinf(sum_re)) != 0) {
            if (core_settings.matrix_outofrange && !flags.f.range_error_ignore){
                int err = dat->completion(ERR_OUT_OF_RANGE, NULL);
                free_vartype(dat->result);
                free(dat);
                return err;
            } else
                sum_re = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
        }
        if ((inf = p_isinf(sum_im)) != 0) {
            if (core_settings.matrix_outofrange && !flags.f.range_error_ignore){
                int err = dat->completion(ERR_OUT_OF_RANGE, NULL);
                free_vartype(dat->result);
                free(dat);
                return err;
            } else
                sum_im = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
        }
        p[2 * (i * n + j)] = sum_re;
        p[2 * (i * n + j) + 1] = sum_im;
        sum_re = 0;
        sum_im = 0;
        if (++j < n)
            continue;
        j = 0;
        if (++i < m)
            continue;
        else {
            int err = dat->completion(ERR_NONE, dat->result);
            free(dat);
            return err;
        }
    }

    dat->i = i;
    dat->j = j;
    dat->k = k;
    dat->sum_re = sum_re;
    dat->sum_im = sum_im;
    return ERR_INTERRUPTIBLE;
}

struct mul_cr_data_struct {
    vartype_complexmatrix *left;
    vartype_realmatrix *right;
    vartype *result;
    int4 i, j, k;
    phloat sum_re, sum_im;
    int (*completion)(int error, vartype *result);
};

static mul_cr_data_struct *mul_cr_data;

static int matrix_mul_cr_worker(bool interrupted);

static int matrix_mul_cr(vartype_complexmatrix *left, vartype_realmatrix *right,
                         int (*completion)(int, vartype *)) {

    mul_cr_data_struct *dat;
    int error;

    if (left->columns != right->rows) {
        error = ERR_DIMENSION_ERROR;
        goto finished;
    }

    if (contains_strings(right)) {
        error = ERR_ALPHA_DATA_IS_INVALID;
        goto finished;
    }

    dat = (mul_cr_data_struct *) malloc(sizeof(mul_cr_data_struct));
    if (dat == NULL) {
        error = ERR_INSUFFICIENT_MEMORY;
        goto finished;
    }

    dat->result = new_complexmatrix(left->rows, right->columns);
    if (dat->result == NULL) {
        free(dat);
        error = ERR_INSUFFICIENT_MEMORY;
        goto finished;
    }

    dat->left = left;
    dat->right = right;
    dat->i = 0;
    dat->j = 0;
    dat->k = 0;
    dat->sum_re = 0;
    dat->sum_im = 0;
    dat->completion = completion;

    mul_cr_data = dat;
    mode_interruptible = matrix_mul_cr_worker;
    mode_stoppable = false;
    return ERR_INTERRUPTIBLE;

    finished:
    return completion(error, NULL);
}

static int matrix_mul_cr_worker(bool interrupted) {
    mul_cr_data_struct *dat = mul_cr_data;
    int count = 0;
    int inf;
    phloat *l = dat->left->array->data;
    phloat *r = dat->right->array->data;
    phloat *p = ((vartype_complexmatrix *) dat->result)->array->data;
    int4 i = dat->i;
    int4 j = dat->j;
    int4 k = dat->k;
    int4 m = dat->left->rows;
    int4 n = dat->right->columns;
    int4 q = dat->left->columns;
    phloat sum_re = dat->sum_re;
    phloat sum_im = dat->sum_im;

    if (interrupted) {
        int err = dat->completion(ERR_INTERRUPTED, NULL);
        free_vartype(dat->result);
        free(dat);
        return err;
    }

    while (count++ < 1000) {
        phloat tmp = r[k * n + j];
        sum_re += tmp * l[2 * (i * q + k)];
        sum_im += tmp * l[2 * (i * q + k) + 1];
        if (++k < q)
            continue;
        k = 0;
        if ((inf = p_isinf(sum_re)) != 0) {
            if (core_settings.matrix_outofrange && !flags.f.range_error_ignore){
                int err = dat->completion(ERR_OUT_OF_RANGE, NULL);
                free_vartype(dat->result);
                free(dat);
                return err;
            } else
                sum_re = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
        }
        if ((inf = p_isinf(sum_im)) != 0) {
            if (core_settings.matrix_outofrange && !flags.f.range_error_ignore){
                int err = dat->completion(ERR_OUT_OF_RANGE, NULL);
                free_vartype(dat->result);
                free(dat);
                return err;
            } else
                sum_im = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
        }
        p[2 * (i * n + j)] = sum_re;
        p[2 * (i * n + j) + 1] = sum_im;
        sum_re = 0;
        sum_im = 0;
        if (++j < n)
            continue;
        j = 0;
        if (++i < m)
            continue;
        else {
            int err = dat->completion(ERR_NONE, dat->result);
            free(dat);
            return err;
        }
    }

    dat->i = i;
    dat->j = j;
    dat->k = k;
    dat->sum_re = sum_re;
    dat->sum_im = sum_im;
    return ERR_INTERRUPTIBLE;
}

struct mul_cc_data_struct {
    vartype_complexmatrix *left;
    vartype_complexmatrix *right;
    vartype *result;
    int4 i, j, k;
    phloat sum_re, sum_im;
    int (*completion)(int error, vartype *result);
};

static mul_cc_data_struct *mul_cc_data;

static int matrix_mul_cc_worker(bool interrupted);

static int matrix_mul_cc(vartype_complexmatrix *left, vartype_complexmatrix *right,
                         int (*completion)(int, vartype *)) {

    mul_cc_data_struct *dat;
    int error;

    if (left->columns != right->rows) {
        error = ERR_DIMENSION_ERROR;
        goto finished;
    }

    dat = (mul_cc_data_struct *) malloc(sizeof(mul_cc_data_struct));
    if (dat == NULL) {
        error = ERR_INSUFFICIENT_MEMORY;
        goto finished;
    }

    dat->result = new_complexmatrix(left->rows, right->columns);
    if (dat->result == NULL) {
        free(dat);
        error = ERR_INSUFFICIENT_MEMORY;
        goto finished;
    }

    dat->left = left;
    dat->right = right;
    dat->i = 0;
    dat->j = 0;
    dat->k = 0;
    dat->sum_re = 0;
    dat->sum_im = 0;
    dat->completion = completion;

    mul_cc_data = dat;
    mode_interruptible = matrix_mul_cc_worker;
    mode_stoppable = false;
    return ERR_INTERRUPTIBLE;

    finished:
    return completion(error, NULL);
}

static int matrix_mul_cc_worker(bool interrupted) {
    mul_cc_data_struct *dat = mul_cc_data;
    int count = 0;
    int inf;
    phloat *l = dat->left->array->data;
    phloat *r = dat->right->array->data;
    phloat *p = ((vartype_complexmatrix *) dat->result)->array->data;
    int4 i = dat->i;
    int4 j = dat->j;
    int4 k = dat->k;
    int4 m = dat->left->rows;
    int4 n = dat->right->columns;
    int4 q = dat->left->columns;
    phloat sum_re = dat->sum_re;
    phloat sum_im = dat->sum_im;

    if (interrupted) {
        int err = dat->completion(ERR_INTERRUPTED, NULL);
        free_vartype(dat->result);
        free(dat);
        return err;
    }

    while (count++ < 1000) {
        phloat l_re = l[2 * (i * q + k)];
        phloat l_im = l[2 * (i * q + k) + 1];
        phloat r_re = r[2 * (k * n + j)];
        phloat r_im = r[2 * (k * n + j) + 1];
        sum_re += l_re * r_re - l_im * r_im;
        sum_im += l_im * r_re + l_re * r_im;
        if (++k < q)
            continue;
        k = 0;
        if ((inf = p_isinf(sum_re)) != 0) {
            if (core_settings.matrix_outofrange && !flags.f.range_error_ignore){
                int err = dat->completion(ERR_OUT_OF_RANGE, NULL);
                free_vartype(dat->result);
                free(dat);
                return err;
            } else
                sum_re = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
        }
        if ((inf = p_isinf(sum_im)) != 0) {
            if (core_settings.matrix_outofrange && !flags.f.range_error_ignore){
                int err = dat->completion(ERR_OUT_OF_RANGE, NULL);
                free_vartype(dat->result);
                free(dat);
                return err;
            } else
                sum_im = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
        }
        p[2 * (i * n + j)] = sum_re;
        p[2 * (i * n + j) + 1] = sum_im;
        sum_re = 0;
        sum_im = 0;
        if (++j < n)
            continue;
        j = 0;
        if (++i < m)
            continue;
        else {
            int err = dat->completion(ERR_NONE, dat->result);
            free(dat);
            return err;
        }
    }

    dat->i = i;
    dat->j = j;
    dat->k = k;
    dat->sum_re = sum_re;
    dat->sum_im = sum_im;
    return ERR_INTERRUPTIBLE;
}

int linalg_mul(const vartype *left, const vartype *right,
                                    int (*completion)(int, vartype *)) {
    if (left->type == TYPE_REALMATRIX) {
        if (right->type == TYPE_REALMATRIX)
            return matrix_mul_rr((vartype_realmatrix *) left,
                                 (vartype_realmatrix *) right,
                                 completion);
        else
            return matrix_mul_rc((vartype_realmatrix *) left,
                                 (vartype_complexmatrix *) right,
                                 completion);
    } else {
        if (right->type == TYPE_REALMATRIX)
            return matrix_mul_cr((vartype_complexmatrix *) left,
                                 (vartype_realmatrix *) right,
                                 completion);
        else
            return matrix_mul_cc((vartype_complexmatrix *) left,
                                 (vartype_complexmatrix *) right,
                                 completion);
    }
}


/**************************/
/***** Matrix inverse *****/
/**************************/

static void (*linalg_inv_completion)(vartype *det);
static vartype *linalg_inv_result;

static int inv_r_completion1(int error, vartype_realmatrix *a, int4 *perm,
                                phloat det);
static int inv_r_completion2(int error, vartype_realmatrix *a, int4 *perm,
                                vartype_realmatrix *b);
static int inv_c_completion1(int error, vartype_complexmatrix *a, int4 *perm,
                                phloat det_re, phloat det_im);
static int inv_c_completion2(int error, vartype_complexmatrix *a, int4 *perm,
                                vartype_complexmatrix *b);

int linalg_inv(const vartype *src, void (*completion)(vartype *)) {
    int4 n;
    int4 *perm;
    if (src->type == TYPE_REALMATRIX) {
        vartype_realmatrix *ma = (vartype_realmatrix *) src;
        vartype *lu, *inv;
        n = ma->rows;
        if (n != ma->columns)
            return ERR_DIMENSION_ERROR;
        if (contains_strings(ma))
            return ERR_ALPHA_DATA_IS_INVALID;
        if (n <= 2)
            return small_inv_r(ma, completion);
        lu = new_realmatrix(n, n);
        if (lu == NULL)
            return ERR_INSUFFICIENT_MEMORY;
        inv = new_realmatrix(n, n);
        if (inv == NULL) {
            free_vartype(lu);
            return ERR_INSUFFICIENT_MEMORY;
        }
        perm = (int4 *) malloc(n * sizeof(int4));
        if (perm == NULL) {
            free_vartype(lu);
            free_vartype(inv);
            return ERR_INSUFFICIENT_MEMORY;
        }
        matrix_copy(lu, src);
        linalg_inv_completion = completion;
        linalg_inv_result = inv;
        return lu_decomp_r((vartype_realmatrix *) lu, perm, inv_r_completion1);
    } else {
        vartype_complexmatrix *ma = (vartype_complexmatrix *) src;
        vartype *lu, *inv;
        n = ma->rows;
        if (n != ma->columns)
            return ERR_DIMENSION_ERROR;
        if (n <= 2)
            return small_inv_c(ma, completion);
        lu = new_complexmatrix(n, n);
        if (lu == NULL)
            return ERR_INSUFFICIENT_MEMORY;
        inv = new_complexmatrix(n, n);
        if (inv == NULL) {
            free_vartype(lu);
            return ERR_INSUFFICIENT_MEMORY;
        }
        perm = (int4 *) malloc(n * sizeof(int4));
        if (perm == NULL) {
            free_vartype(lu);
            free_vartype(inv);
            return ERR_INSUFFICIENT_MEMORY;
        }
        matrix_copy(lu, src);
        linalg_inv_completion = completion;
        linalg_inv_result = inv;
        return lu_decomp_c((vartype_complexmatrix *) lu, perm,
                                                    inv_c_completion1);
    }
}

static int inv_r_completion1(int error, vartype_realmatrix *a, int4 *perm,
                                phloat det) {
    if (error != ERR_NONE) {
        free_vartype(linalg_inv_result);
        free_vartype((vartype *) a);
        free(perm);
        return error;
    } else {
        int4 i, n = a->rows;
        vartype_realmatrix *inv = (vartype_realmatrix *) linalg_inv_result;
        for (i = 0; i < n; i++)
            inv->array->data[i * (n + 1)] = 1;
        return lu_backsubst_rr(a, perm, inv, inv_r_completion2);
    }
}

static int inv_r_completion2(int error, vartype_realmatrix *a, int4 *perm,
                                vartype_realmatrix *b) {
    if (error != ERR_NONE)
        free_vartype(linalg_inv_result); /* Note: linalg_inv_result == b */
    else
        linalg_inv_completion(linalg_inv_result);
    free_vartype((vartype *) a);
    free(perm);
    return error;
}

static int inv_c_completion1(int error, vartype_complexmatrix *a, int4 *perm,
                                phloat det_re, phloat det_im) {
    if (error != ERR_NONE) {
        free_vartype(linalg_inv_result);
        free_vartype((vartype *) a);
        free(perm);
        return error;
    } else {
        int4 i, n = a->rows;
        vartype_complexmatrix *inv =
                            (vartype_complexmatrix *) linalg_inv_result;
        for (i = 0; i < n; i++)
            inv->array->data[2 * (i * (n + 1))] = 1;
        return lu_backsubst_cc(a, perm, inv, inv_c_completion2);
    }
}

static int inv_c_completion2(int error, vartype_complexmatrix *a, int4 *perm,
                                vartype_complexmatrix *b) {
    if (error != ERR_NONE)
        free_vartype(linalg_inv_result); /* Note: linalg_inv_result == b */
    else
        linalg_inv_completion(linalg_inv_result);
    free_vartype((vartype *) a);
    free(perm);
    return error;
}

static int small_det_r(vartype_realmatrix *m, phloat *r);
static int small_det_c(vartype_complexmatrix *m, phloat *dre, phloat *dim);

static vartype *small_inv_res;
static int small_inv_completion(int err, vartype *res) {
    small_inv_res = res;
    return err;
}

static int small_inv_r(vartype_realmatrix *ma, void (*completion)(vartype *)) {
    if (ma->rows == 1) {
        phloat x = ma->array->data[0];
        if (x == 0) {
            if (core_settings.matrix_singularmatrix)
                return ERR_SINGULAR_MATRIX;
            x = POS_HUGE_PHLOAT;
        } else {
            x = phloat(1) / x;
            int inf = p_isinf(x);
            if (inf != 0) {
                if (core_settings.matrix_outofrange && !flags.f.range_error_ignore)
                    return ERR_OUT_OF_RANGE;
                x = inf > 0 ? POS_HUGE_PHLOAT : NEG_HUGE_PHLOAT;
            }
        }
        vartype *r = new_realmatrix(1, 1);
        if (r == NULL)
            return ERR_INSUFFICIENT_MEMORY;
        ((vartype_realmatrix *) r)->array->data[0] = x;
        completion(r);
        return ERR_NONE;
    }
    phloat det;
    int err = small_det_r(ma, &det);
    if (det == 0) {
        if (core_settings.matrix_singularmatrix)
            return ERR_SINGULAR_MATRIX;
        vartype_realmatrix *sm = (vartype_realmatrix *) new_realmatrix(2, 2);
        if (sm == NULL)
            return ERR_INSUFFICIENT_MEMORY;
        sm->array->data[0] = POS_HUGE_PHLOAT;
        sm->array->data[3] = POS_HUGE_PHLOAT;
        completion((vartype *) sm);
        return ERR_NONE;
    }
    vartype *d = new_real(det);
    if (d == NULL)
        return ERR_INSUFFICIENT_MEMORY;
    bool saved_range_error_ignore = flags.f.range_error_ignore;
    flags.f.range_error_ignore = !core_settings.matrix_outofrange || flags.f.range_error_ignore;
    err = generic_div(d, (vartype *) ma, small_inv_completion);
    flags.f.range_error_ignore = saved_range_error_ignore;
    free_vartype(d);
    if (err != ERR_NONE)
        return err;
    phloat *data = ((vartype_realmatrix *) small_inv_res)->array->data;
    phloat t = data[0];
    data[0] = data[3];
    data[3] = t;
    data[1] = -data[1];
    data[2] = -data[2];
    completion(small_inv_res);
    return ERR_NONE;
}

static int small_inv_c(vartype_complexmatrix *ma, void (*completion)(vartype *)) {
    if (ma->rows == 1) {
        phloat xre = ma->array->data[0];
        phloat xim = ma->array->data[1];
        if (xre == 0 && xim == 0) {
            if (core_settings.matrix_singularmatrix)
                return ERR_SINGULAR_MATRIX;
            xre = POS_HUGE_PHLOAT;
            xim = 0;
        } else {
            bool saved_range_error_ignore = flags.f.range_error_ignore;
            flags.f.range_error_ignore = !core_settings.matrix_outofrange || flags.f.range_error_ignore;
            int err = math_inv(xre, xim, &xre, &xim);
            flags.f.range_error_ignore = saved_range_error_ignore;
            if (err != ERR_NONE)
                return err;
        }
        vartype *r = new_complexmatrix(1, 1);
        if (r == NULL)
            return ERR_INSUFFICIENT_MEMORY;
        ((vartype_complexmatrix *) r)->array->data[0] = xre;
        ((vartype_complexmatrix *) r)->array->data[1] = xim;
        completion(r);
        return ERR_NONE;
    }
    phloat dre, dim;
    int err = small_det_c(ma, &dre, &dim);
    if (dre == 0 && dim == 0) {
        if (core_settings.matrix_singularmatrix)
            return ERR_SINGULAR_MATRIX;
        vartype_complexmatrix *sm = (vartype_complexmatrix *) new_complexmatrix(2, 2);
        if (sm == NULL)
            return ERR_INSUFFICIENT_MEMORY;
        sm->array->data[0] = POS_HUGE_PHLOAT;
        sm->array->data[6] = POS_HUGE_PHLOAT;
        completion((vartype *) sm);
        return ERR_NONE;
    }
    vartype *d = new_complex(dre, dim);
    if (d == NULL)
        return ERR_INSUFFICIENT_MEMORY;
    bool saved_range_error_ignore = flags.f.range_error_ignore;
    flags.f.range_error_ignore = !core_settings.matrix_outofrange || flags.f.range_error_ignore;
    err = generic_div(d, (vartype *) ma, small_inv_completion);
    flags.f.range_error_ignore = saved_range_error_ignore;
    free_vartype(d);
    if (err != ERR_NONE)
        return err;
    phloat *data = ((vartype_complexmatrix *) small_inv_res)->array->data;
    phloat t = data[0];
    data[0] = data[6];
    data[6] = t;
    t = data[1];
    data[1] = data[7];
    data[7] = t;
    data[2] = -data[2];
    data[3] = -data[3];
    data[4] = -data[4];
    data[5] = -data[5];
    completion(small_inv_res);
    return ERR_NONE;
}


/******************************/
/***** Matrix determinant *****/
/******************************/

static void (*linalg_det_completion)(vartype *det);
static bool linalg_det_prev_sm_err;

static int det_r_completion(int error, vartype_realmatrix *a, int4 *perm,
                                    phloat det);
static int det_c_completion(int error, vartype_complexmatrix *a, int4 *perm,
                                    phloat det_re, phloat det_im);

int linalg_det(const vartype *src, void (*completion)(vartype *)) {
    int4 n;
    int4 *perm;
    if (src->type == TYPE_REALMATRIX) {
        vartype_realmatrix *ma = (vartype_realmatrix *) src;
        n = ma->rows;
        if (n != ma->columns)
            return ERR_DIMENSION_ERROR;
        if (contains_strings(ma))
            return ERR_ALPHA_DATA_IS_INVALID;
        if (n <= 2) {
            phloat d;
            int err = small_det_r(ma, &d);
            if (err != ERR_NONE)
                return err;
            vartype *v = new_real(d);
            if (v == NULL)
                return ERR_INSUFFICIENT_MEMORY;
            completion(v);
            return ERR_NONE;
        }
        ma = (vartype_realmatrix *) dup_vartype(src);
        if (ma == NULL)
            return ERR_INSUFFICIENT_MEMORY;
        if (!disentangle((vartype *) ma)) {
            free_vartype((vartype *) ma);
            return ERR_INSUFFICIENT_MEMORY;
        }
        perm = (int4 *) malloc(n * sizeof(int4));
        if (perm == NULL) {
            free_vartype((vartype *) ma);
            return ERR_INSUFFICIENT_MEMORY;
        }

        /* Before calling lu_decomp_r, make sure the 'singular matrix'
         * error reporting mode is on; we don't want the HP-42S compatible
         * zero-pivot-fudging to take place when all we're doing is computing
         * the determinant.
         * The completion routine will restore the 'singular matrix' error
         * mode to its original value.
         */
        linalg_det_prev_sm_err = core_settings.matrix_singularmatrix;
        core_settings.matrix_singularmatrix = true;

        linalg_det_completion = completion;
        return lu_decomp_r(ma, perm, det_r_completion);
    } else /* src->type == TYPE_COMPLEXMATRIX */ {
        vartype_complexmatrix *ma = (vartype_complexmatrix *) src;
        n = ma->rows;
        if (n != ma->columns)
            return ERR_DIMENSION_ERROR;
        if (n <= 2) {
            phloat dre, dim;
            int err = small_det_c(ma, &dre, &dim);
            if (err != ERR_NONE)
                return err;
            vartype *v = new_complex(dre, dim);
            if (v == NULL)
                return ERR_INSUFFICIENT_MEMORY;
            completion(v);
            return ERR_NONE;
        }
        ma = (vartype_complexmatrix *) dup_vartype(src);
        if (ma == NULL)
            return ERR_INSUFFICIENT_MEMORY;
        if (!disentangle((vartype *) ma)) {
            free_vartype((vartype *) ma);
            return ERR_INSUFFICIENT_MEMORY;
        }
        n = ma->rows;
        perm = (int4 *) malloc(n * sizeof(int4));
        if (perm == NULL) {
            free_vartype((vartype *) ma);
            return ERR_INSUFFICIENT_MEMORY;
        }

        /* Before calling lu_decomp_c, make sure the 'singular matrix'
         * error reporting mode is on; we don't want the HP-42S compatible
         * zero-pivot-fudging to take place when all we're doing is computing
         * the determinant.
         * The completion routine will restore the 'singular matrix' error
         * mode to its original value.
         */
        linalg_det_prev_sm_err = core_settings.matrix_singularmatrix;
        core_settings.matrix_singularmatrix = true;

        linalg_det_completion = completion;
        return lu_decomp_c(ma, perm, det_c_completion);
    }
}

static int det_r_completion(int error, vartype_realmatrix *a, int4 *perm,
                                         phloat det) {
    vartype *det_v = NULL;

    core_settings.matrix_singularmatrix = linalg_det_prev_sm_err;

    free_vartype((vartype *) a);
    free(perm);
    if (error == ERR_SINGULAR_MATRIX) {
        det = 0;
        error = ERR_NONE;
    }
    if (error == ERR_NONE) {
        int inf = p_isinf(det);
        if (inf != 0) {
            if (flags.f.range_error_ignore)
                det = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
            else
                error = ERR_OUT_OF_RANGE;
        }
    }
    if (error == ERR_NONE) {
        det_v = new_real(det);
        if (det_v == NULL)
            error = ERR_INSUFFICIENT_MEMORY;
    }

    if (error == ERR_NONE)
        linalg_det_completion(det_v);
    return error;
}

static int det_c_completion(int error, vartype_complexmatrix *a, int4 *perm,
                                    phloat det_re, phloat det_im) {
    vartype *det_v = NULL;

    core_settings.matrix_singularmatrix = linalg_det_prev_sm_err;

    free_vartype((vartype *) a);
    free(perm);
    if (error == ERR_SINGULAR_MATRIX) {
        det_re = 0;
        det_im = 0;
        error = ERR_NONE;
    }
    if (error == ERR_NONE) {
        int inf;
        if ((inf = p_isinf(det_re)) != 0) {
            if (flags.f.range_error_ignore)
                det_re = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
            else
                error = ERR_OUT_OF_RANGE;
        }
        if ((inf = p_isinf(det_im)) != 0) {
            if (flags.f.range_error_ignore)
                det_im = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
            else
                error = ERR_OUT_OF_RANGE;
        }
    }
    if (error == ERR_NONE) {
        det_v = new_complex(det_re, det_im);
        if (det_v == NULL)
            error = ERR_INSUFFICIENT_MEMORY;
    }

    if (error == ERR_NONE)
        linalg_det_completion(det_v);
    return error;
}

static phloat dot_2d(phloat a1, phloat a2, phloat a3, phloat a4, bool add, int *scale = NULL) {
    int s1 = a1 == 0 || p_isinf(a1) != 0 ? 0 : ilogb(a1);
    int s2 = a2 == 0 || p_isinf(a2) != 0 ? 0 : ilogb(a2);
    int s3 = a3 == 0 || p_isinf(a3) != 0 ? 0 : ilogb(a3);
    int s4 = a4 == 0 || p_isinf(a4) != 0 ? 0 : ilogb(a4);
    phloat p1 = scalbn(a1, -s1) * scalbn(a3, -s3);
    int z1 = s1 + s3;
    phloat p2 = scalbn(a2, -s2) * scalbn(a4, -s4);
    int z2 = s2 + s4;
    if (z1 > z2) {
        p2 = scalbn(p2, z2 - z1);
        z2 = z1;
    } else if (z2 > z1) {
        p1 = scalbn(p1, z1 - z2);
        z1 = z2;
    }
    if (scale == NULL)
        return scalbn(add ? p1 + p2 : p1 - p2, z1);
    *scale = z1;
    return add ? p1 + p2 : p1 - p2;
}

static phloat ssub(phloat a1, int s1, phloat a2, int s2) {
    if (s1 > s2) {
        a2 = scalbn(a2, s2 - s1);
        s2 = s1;
    } else if (s2 > s1) {
        a1 = scalbn(a1, s1 - s2);
        s1 = s2;
    }
    return scalbn(a1 - a2, s1);
}

static int small_det_r(vartype_realmatrix *m, phloat *r) {
    if (m->rows == 1) {
        *r = m->array->data[0];
        return ERR_NONE;
    }
    phloat *a = m->array->data;
    *r = dot_2d(a[0], a[1], a[3], a[2], false);
    int inf = p_isinf(*r);
    if (inf != 0)
        if (flags.f.range_error_ignore)
            *r = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
        else
            return ERR_OUT_OF_RANGE;
    return ERR_NONE;
}

static int small_det_c(vartype_complexmatrix *m, phloat *dre, phloat *dim) {
    if (m->rows == 1) {
        *dre = m->array->data[0];
        *dim = m->array->data[1];
        return ERR_NONE;
    }
    phloat *a = m->array->data;
    int s1, s2, s3, s4;
    phloat r1re = dot_2d(a[0], a[1], a[6], a[7], false, &s1);
    phloat r1im = dot_2d(a[0], a[1], a[7], a[6], true, &s2);
    phloat r2re = dot_2d(a[2], a[3], a[4], a[5], false, &s3);
    phloat r2im = dot_2d(a[2], a[3], a[5], a[4], true, &s4);
    *dre = ssub(r1re, s1, r2re, s3);
    *dim = ssub(r1im, s2, r2im, s4);
    int inf = p_isinf(*dre);
    if (inf != 0)
        if (flags.f.range_error_ignore)
            *dre = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
        else
            return ERR_OUT_OF_RANGE;
    inf = p_isinf(*dim);
    if (inf != 0)
        if (flags.f.range_error_ignore)
            *dim = inf < 0 ? NEG_HUGE_PHLOAT : POS_HUGE_PHLOAT;
        else
            return ERR_OUT_OF_RANGE;
    return ERR_NONE;
}
