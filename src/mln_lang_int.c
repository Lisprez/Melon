
/*
 * Copyright (C) Niklaus F.Schen.
 */
#include "mln_lang_int.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __DEBUG__
#include <assert.h>
#define ASSERT(x) assert(x)
#else
#define ASSERT(x);
#endif

static int
mln_lang_int_assign(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_pluseq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_subeq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_lmoveq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_rmoveq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_muleq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_diveq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_oreq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_andeq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_xoreq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_modeq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_cor(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_cand(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_cxor(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_equal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_nonequal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_less(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_lesseq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_grea(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_greale(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_lmov(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_rmov(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_plus(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_sub(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_mul(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_div(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_mod(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_sdec(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_sinc(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_negative(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_reverse(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_not(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_pinc(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_int_pdec(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);

mln_lang_method_t mln_lang_int_oprs = {
    mln_lang_int_assign,
    mln_lang_int_pluseq,
    mln_lang_int_subeq,
    mln_lang_int_lmoveq,
    mln_lang_int_rmoveq,
    mln_lang_int_muleq,
    mln_lang_int_diveq,
    mln_lang_int_oreq,
    mln_lang_int_andeq,
    mln_lang_int_xoreq,
    mln_lang_int_modeq,
    mln_lang_int_cor,
    mln_lang_int_cand,
    mln_lang_int_cxor,
    mln_lang_int_equal,
    mln_lang_int_nonequal,
    mln_lang_int_less,
    mln_lang_int_lesseq,
    mln_lang_int_grea,
    mln_lang_int_greale,
    mln_lang_int_lmov,
    mln_lang_int_rmov,
    mln_lang_int_plus,
    mln_lang_int_sub,
    mln_lang_int_mul,
    mln_lang_int_div,
    mln_lang_int_mod,
    mln_lang_int_sdec,
    mln_lang_int_sinc,
    NULL,
    NULL,
    mln_lang_int_negative,
    mln_lang_int_reverse,
    mln_lang_int_not,
    mln_lang_int_pinc,
    mln_lang_int_pdec
};

static int
mln_lang_int_assign(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    if (mln_lang_var_value_set(ctx, op1, op2) < 0) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_int_pluseq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op1);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    if (type == M_LANG_VAL_TYPE_STRING) {
        mln_lang_method_t *method = mln_lang_methods[type];
        if (method == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        mln_lang_op handler = method->pluseq_handler;
        if (handler == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        return handler(ctx, ret, op1, op2);
    }
    if (type == M_LANG_VAL_TYPE_REAL) {
        mln_lang_var_set_real(op1, \
                             mln_lang_var_toreal(op1) + \
                                 mln_lang_var_toreal(op2));
    } else {
        mln_lang_var_set_int(op1, \
                            mln_lang_var_toint(op1) + \
                                mln_lang_var_toint(op2));
    }
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_int_subeq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op1);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    if (type == M_LANG_VAL_TYPE_REAL) {
        mln_lang_var_set_real(op1, \
                             mln_lang_var_toreal(op1) - \
                                 mln_lang_var_toreal(op2));
    } else {
        mln_lang_var_set_int(op1, \
                            mln_lang_var_toint(op1) - \
                                mln_lang_var_toint(op2));
    }
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_int_lmoveq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op1);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_var_set_int(op1, \
                        mln_lang_var_toint(op1) << \
                            mln_lang_var_toint(op2));
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_int_rmoveq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op1);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_var_set_int(op1, \
                        mln_lang_var_toint(op1) >> \
                            mln_lang_var_toint(op2));
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_int_muleq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op1);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    if (type == M_LANG_VAL_TYPE_REAL) {
        mln_lang_var_set_real(op1, \
                             mln_lang_var_toreal(op1) * \
                                 mln_lang_var_toreal(op2));
    } else {
        mln_lang_var_set_int(op1, \
                            mln_lang_var_toint(op1) * \
                                mln_lang_var_toint(op2));
    }
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_int_diveq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op1);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    if (type == M_LANG_VAL_TYPE_REAL) {
        double r = mln_lang_var_toreal(op2);
        double tmp = r < 0? -r: r;
        if (tmp <= 1e-15) {
            mln_lang_errmsg(ctx, "Division by zero.");
            return -1;
        }
        mln_lang_var_set_real(op1, mln_lang_var_toreal(op1) / r);
    } else {
        mln_s64_t i = mln_lang_var_toint(op2);
        if (!i) {
            mln_lang_errmsg(ctx, "Division by zero.");
            return -1;
        }
        mln_lang_var_set_int(op1, mln_lang_var_toint(op1) / i);
    }
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_int_oreq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op1);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_var_set_int(op1, \
                        mln_lang_var_toint(op1) | \
                            mln_lang_var_toint(op2));
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_int_andeq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op1);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_var_set_int(op1, \
                        mln_lang_var_toint(op1) & \
                            mln_lang_var_toint(op2));
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_int_xoreq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op1);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_var_set_int(op1, \
                        mln_lang_var_toint(op1) ^ \
                            mln_lang_var_toint(op2));
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_int_modeq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op1);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_s64_t i = mln_lang_var_toint(op2);
    if (!i) {
        mln_lang_errmsg(ctx, "Modulo by zero.");
        return -1;
    }
    mln_lang_var_set_int(op1, mln_lang_var_toint(op1) % i);
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_int_cor(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_val_t *val;
    mln_s64_t i = mln_lang_var_toint(op1) | mln_lang_var_toint(op2);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_cand(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_val_t *val;
    mln_s64_t i = mln_lang_var_toint(op1) & mln_lang_var_toint(op2);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_cxor(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL || \
        type == M_LANG_VAL_TYPE_STRING)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_val_t *val;
    mln_s64_t i = mln_lang_var_toint(op1) ^ mln_lang_var_toint(op2);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_equal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        if ((*ret = mln_lang_var_create_false(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
        return 0;
    }
    mln_lang_val_t *val;
    mln_u8_t b = mln_lang_var_toint(op1) == mln_lang_var_toint(op2);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_BOOL, &b)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_nonequal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        if ((*ret = mln_lang_var_create_true(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
        return 0;
    }
    mln_lang_val_t *val;
    mln_u8_t b = mln_lang_var_toint(op1) != mln_lang_var_toint(op2);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_BOOL, &b)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_less(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_val_t *val;
    mln_u8_t b;
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }

    if (type == M_LANG_VAL_TYPE_STRING) {
        mln_lang_method_t *method = mln_lang_methods[type];
        if (method == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        mln_lang_op handler = method->less_handler;
        if (handler == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        return handler(ctx, ret, op1, op2);
    }
    if (type == M_LANG_VAL_TYPE_REAL) {
        b = mln_lang_var_toreal(op1) < mln_lang_var_toreal(op2);
    } else {
        b = mln_lang_var_toint(op1) < mln_lang_var_toint(op2);
    }
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_BOOL, &b)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_lesseq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_val_t *val;
    mln_u8_t b;
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    if (type == M_LANG_VAL_TYPE_STRING) {
        mln_lang_method_t *method = mln_lang_methods[type];
        if (method == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        mln_lang_op handler = method->lesseq_handler;
        if (handler == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        return handler(ctx, ret, op1, op2);
    }
    if (type == M_LANG_VAL_TYPE_REAL) {
        b = mln_lang_var_toreal(op1) <= mln_lang_var_toreal(op2);
    } else {
        b = mln_lang_var_toint(op1) <= mln_lang_var_toint(op2);
    }
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_BOOL, &b)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_grea(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_val_t *val;
    mln_u8_t b;
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    if (type == M_LANG_VAL_TYPE_STRING) {
        mln_lang_method_t *method = mln_lang_methods[type];
        if (method == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        mln_lang_op handler = method->grea_handler;
        if (handler == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        return handler(ctx, ret, op1, op2);
    }
    if (type == M_LANG_VAL_TYPE_REAL) {
        b = mln_lang_var_toreal(op1) > mln_lang_var_toreal(op2);
    } else {
        b = mln_lang_var_toint(op1) > mln_lang_var_toint(op2);
    }
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_BOOL, &b)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_greale(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_val_t *val;
    mln_u8_t b;
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    if (type == M_LANG_VAL_TYPE_STRING) {
        mln_lang_method_t *method = mln_lang_methods[type];
        if (method == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        mln_lang_op handler = method->greale_handler;
        if (handler == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        return handler(ctx, ret, op1, op2);
    }
    if (type == M_LANG_VAL_TYPE_REAL) {
        b = mln_lang_var_toreal(op1) >= mln_lang_var_toreal(op2);
    } else {
        b = mln_lang_var_toint(op1) >= mln_lang_var_toint(op2);
    }
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_BOOL, &b)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_lmov(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_REAL || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_val_t *val;
    mln_s64_t i = mln_lang_var_toint(op1) << mln_lang_var_toint(op2);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_rmov(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_REAL || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_val_t *val;
    mln_s64_t i = mln_lang_var_toint(op1) >> mln_lang_var_toint(op2);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_plus(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    if (type == M_LANG_VAL_TYPE_STRING) {
        mln_lang_method_t *method = mln_lang_methods[type];
        if (method == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        mln_lang_op handler = method->plus_handler;
        if (handler == NULL) {
            mln_lang_errmsg(ctx, "Operation Not support.");
            return -1;
        }
        return handler(ctx, ret, op1, op2);
    }
    mln_lang_val_t *val;
    if (type == M_LANG_VAL_TYPE_REAL) {
        double r = mln_lang_var_toreal(op1) + mln_lang_var_toreal(op2);
        if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_REAL, &r)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        mln_s64_t i = mln_lang_var_toint(op1) + mln_lang_var_toint(op2);
        if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_sub(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_val_t *val;
    if (type == M_LANG_VAL_TYPE_REAL) {
        double r = mln_lang_var_toreal(op1) - mln_lang_var_toreal(op2);
        if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_REAL, &r)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        mln_s64_t i = mln_lang_var_toint(op1) - mln_lang_var_toint(op2);
        if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_mul(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_val_t *val;
    if (type == M_LANG_VAL_TYPE_REAL) {
        double r = mln_lang_var_toreal(op1) * mln_lang_var_toreal(op2);
        if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_REAL, &r)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        mln_s64_t i = mln_lang_var_toint(op1) * mln_lang_var_toint(op2);
        if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_div(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_val_t *val;
    if (type == M_LANG_VAL_TYPE_REAL) {
        double tmp = mln_lang_var_toreal(op2);
        double tmpr = tmp < 0? -tmp: tmp;
        if (tmpr <= 1e-15) {
            mln_lang_errmsg(ctx, "Division by zero.");
            return -1;
        }
        double r = mln_lang_var_toreal(op1) / tmp;
        if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_REAL, &r)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        mln_s64_t tmp =  mln_lang_var_toint(op2);
        if (!tmp) {
            mln_lang_errmsg(ctx, "Division by zero.");
            return -1;
        }
        mln_s64_t i = mln_lang_var_toint(op1) / tmp;
        if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_mod(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_val_type_get(op2);
    if (type == M_LANG_VAL_TYPE_OBJECT || \
        type == M_LANG_VAL_TYPE_FUNC || \
        type == M_LANG_VAL_TYPE_ARRAY || \
        type == M_LANG_VAL_TYPE_BOOL || \
        type == M_LANG_VAL_TYPE_REAL || \
        type == M_LANG_VAL_TYPE_NIL)
    {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_val_t *val;
    mln_s64_t tmp =  mln_lang_var_toint(op2);
    if (!tmp) {
        mln_lang_errmsg(ctx, "Modulo by zero.");
        return -1;
    }
    mln_s64_t i = mln_lang_var_toint(op1) % tmp;
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_sdec(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_val_t *val;
    mln_s64_t i = mln_lang_var_toint(op1);
    mln_lang_var_set_int(op1, i-1);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_sinc(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_val_t *val;
    mln_s64_t i = mln_lang_var_toint(op1);
    mln_lang_var_set_int(op1, i+1);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_negative(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_val_t *val;
    mln_s64_t i = -mln_lang_var_toint(op1);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_reverse(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_val_t *val;
    mln_s64_t i = ~mln_lang_var_toint(op1);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_not(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_val_t *val;
    mln_u8_t b = !mln_lang_var_toint(op1);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_BOOL, &b)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_pinc(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_val_t *val;
    mln_s64_t i = mln_lang_var_toint(op1) + 1;
    mln_lang_var_set_int(op1, i);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

static int
mln_lang_int_pdec(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_val_t *val;
    mln_s64_t i = mln_lang_var_toint(op1) - 1;
    mln_lang_var_set_int(op1, i);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_INT, &i)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_new(ctx, NULL, M_LANG_VAR_NORMAL, val, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_lang_val_free(val);
        return -1;
    }
    return 0;
}

