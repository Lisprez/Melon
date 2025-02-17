
/*
 * Copyright (C) Niklaus F.Schen.
 */
#include "mln_lang_str.h"
#include "mln_string.h"
#include <stdio.h>

#ifdef __DEBUG__
#include <assert.h>
#define ASSERT(x) assert(x)
#else
#define ASSERT(x);
#endif

static int
mln_lang_str_assign(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_str_pluseq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_str_equal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_str_nonequal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_str_less(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_str_lesseq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_str_grea(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_str_greale(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_str_plus(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_str_not(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_str_index(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);

mln_lang_method_t mln_lang_str_oprs = {
    mln_lang_str_assign,
    mln_lang_str_pluseq,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    mln_lang_str_equal,
    mln_lang_str_nonequal,
    mln_lang_str_less,
    mln_lang_str_lesseq,
    mln_lang_str_grea,
    mln_lang_str_greale,
    NULL,
    NULL,
    mln_lang_str_plus,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    mln_lang_str_index,
    NULL,
    NULL,
    NULL,
    mln_lang_str_not,
    NULL,
    NULL
};

static int
mln_lang_str_assign(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    if (mln_lang_var_value_set_string_ref(ctx, op1, op2) < 0) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_str_pluseq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_string_t *s, *tmp1, *tmp2;
    if ((tmp1 = mln_lang_var_tostring(ctx->pool, op1)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((tmp2 = mln_lang_var_tostring(ctx->pool, op2)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_string_free(tmp1);
        return -1;
    }
    if ((s = mln_string_pool_strcat(ctx->pool, tmp1, tmp2)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_string_free(tmp1);
        mln_string_free(tmp2);
        return -1;
    }
    mln_string_free(tmp1);
    mln_string_free(tmp2);
    mln_lang_var_set_string(op1, s);
    *ret = mln_lang_var_ref(op1);
    return 0;
}

static int
mln_lang_str_equal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    int rv;
    mln_string_t *tmp;
    mln_s32_t type = mln_lang_var_val_type_get(op2);

    if (type != M_LANG_VAL_TYPE_INT && type != M_LANG_VAL_TYPE_REAL && type != M_LANG_VAL_TYPE_STRING) {
        if ((*ret = mln_lang_var_create_false(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
        return 0;
    }
    if ((tmp = mln_lang_var_tostring(ctx->pool, op2)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    rv = mln_string_strcmp(mln_lang_var_val_get(op1)->data.s, tmp);
    mln_string_free(tmp);
    if (rv) {
        if ((*ret = mln_lang_var_create_false(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        if ((*ret = mln_lang_var_create_true(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    }
    return 0;
}

static int
mln_lang_str_nonequal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    int rv;
    mln_string_t *tmp;
    mln_s32_t type = mln_lang_var_val_type_get(op2);

    if (type != M_LANG_VAL_TYPE_INT && type != M_LANG_VAL_TYPE_REAL && type != M_LANG_VAL_TYPE_STRING) {
        if ((*ret = mln_lang_var_create_true(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
        return 0;
    }
    if ((tmp = mln_lang_var_tostring(ctx->pool, op2)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    rv = mln_string_strcmp(mln_lang_var_val_get(op1)->data.s, tmp);
    mln_string_free(tmp);
    if (rv) {
        if ((*ret = mln_lang_var_create_true(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        if ((*ret = mln_lang_var_create_false(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    }
    return 0;
}

static int
mln_lang_str_less(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    int rv;
    mln_string_t *tmp1, *tmp2;
    mln_s32_t type = mln_lang_var_val_type_get(op2);

    if (type != M_LANG_VAL_TYPE_INT && type != M_LANG_VAL_TYPE_REAL && type != M_LANG_VAL_TYPE_STRING) {
        mln_lang_errmsg(ctx, "Operation NOT support.");
        return -1;
    }
    if ((tmp1 = mln_lang_var_tostring(ctx->pool, op1)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((tmp2 = mln_lang_var_tostring(ctx->pool, op2)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_string_free(tmp1);
        return -1;
    }
    rv = mln_string_strseqcmp(tmp1, tmp2);
    mln_string_free(tmp1);
    mln_string_free(tmp2);
    if (rv < 0) {
        if ((*ret = mln_lang_var_create_true(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        if ((*ret = mln_lang_var_create_false(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    }
    return 0;
}

static int
mln_lang_str_lesseq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    int rv;
    mln_string_t *tmp1, *tmp2;
    mln_s32_t type = mln_lang_var_val_type_get(op2);

    if (type != M_LANG_VAL_TYPE_INT && type != M_LANG_VAL_TYPE_REAL && type != M_LANG_VAL_TYPE_STRING) {
        mln_lang_errmsg(ctx, "Operation NOT support.");
        return -1;
    }
    if ((tmp1 = mln_lang_var_tostring(ctx->pool, op1)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((tmp2 = mln_lang_var_tostring(ctx->pool, op2)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_string_free(tmp1);
        return -1;
    }
    rv = mln_string_strseqcmp(tmp1, tmp2);
    mln_string_free(tmp1);
    mln_string_free(tmp2);
    if (rv <= 0) {
        if ((*ret = mln_lang_var_create_true(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        if ((*ret = mln_lang_var_create_false(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    }
    return 0;
}

static int
mln_lang_str_grea(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    int rv;
    mln_string_t *tmp1, *tmp2;
    mln_s32_t type = mln_lang_var_val_type_get(op2);

    if (type != M_LANG_VAL_TYPE_INT && type != M_LANG_VAL_TYPE_REAL && type != M_LANG_VAL_TYPE_STRING) {
        mln_lang_errmsg(ctx, "Operation NOT support.");
        return -1;
    }
    if ((tmp1 = mln_lang_var_tostring(ctx->pool, op1)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((tmp2 = mln_lang_var_tostring(ctx->pool, op2)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_string_free(tmp1);
        return -1;
    }
    rv = mln_string_strseqcmp(tmp1, tmp2);
    mln_string_free(tmp1);
    mln_string_free(tmp2);
    if (rv > 0) {
        if ((*ret = mln_lang_var_create_true(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        if ((*ret = mln_lang_var_create_false(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    }
    return 0;
}

static int
mln_lang_str_greale(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    int rv;
    mln_string_t *tmp1, *tmp2;
    mln_s32_t type = mln_lang_var_val_type_get(op2);

    if (type != M_LANG_VAL_TYPE_INT && type != M_LANG_VAL_TYPE_REAL && type != M_LANG_VAL_TYPE_STRING) {
        mln_lang_errmsg(ctx, "Operation NOT support.");
        return -1;
    }
    if ((tmp1 = mln_lang_var_tostring(ctx->pool, op1)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((tmp2 = mln_lang_var_tostring(ctx->pool, op2)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_string_free(tmp1);
        return -1;
    }
    rv = mln_string_strseqcmp(tmp1, tmp2);
    mln_string_free(tmp1);
    mln_string_free(tmp2);
    if (rv >= 0) {
        if ((*ret = mln_lang_var_create_true(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        if ((*ret = mln_lang_var_create_false(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    }
    return 0;
}

static int
mln_lang_str_plus(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_string_t *s, *tmp1, *tmp2;
    mln_lang_val_t *val;
    if ((tmp1 = mln_lang_var_tostring(ctx->pool, op1)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((tmp2 = mln_lang_var_tostring(ctx->pool, op2)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_string_free(tmp1);
        return -1;
    }
    if ((s = mln_string_pool_strcat(ctx->pool, tmp1, tmp2)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_string_free(tmp1);
        mln_string_free(tmp2);
        return -1;
    }
    mln_string_free(tmp1);
    mln_string_free(tmp2);
    if ((val = mln_lang_val_new(ctx, M_LANG_VAL_TYPE_STRING, s)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        mln_string_free(s);
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
mln_lang_str_not(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    if (mln_lang_condition_is_true(op1)) {
        if ((*ret = mln_lang_var_create_false(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        if ((*ret = mln_lang_var_create_true(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    }
    return 0;
}

static int
mln_lang_str_index(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s64_t offset;
    mln_string_t c;
    mln_string_t *s = mln_lang_var_val_get(op1)->data.s;
    if (mln_lang_var_val_type_get(op2) != M_LANG_VAL_TYPE_INT) {
        mln_lang_errmsg(ctx, "Offset must be an integer.");
        return -1;
    }
    offset = mln_lang_var_val_get(op2)->data.i;
    if (offset < 0) {
        offset = (mln_s64_t)(s->len) + offset;
    }
    if (offset < 0 || offset >= s->len) {
        mln_lang_errmsg(ctx, "Invalid offset.");
        return -1;
    }
    mln_string_nset(&c, &(s->data[offset]), 1);
    if ((*ret = mln_lang_var_create_string(ctx, &c, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    return 0;
}

