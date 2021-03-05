
/*
 * Copyright (C) Niklaus F.Schen.
 */
#include "mln_lang_obj.h"
#include <stdio.h>

#ifdef __DEBUG__
#include <assert.h>
#define ASSERT(x) assert(x)
#else
#define ASSERT(x);
#endif

static int
mln_lang_obj_assign(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_obj_pluseq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_obj_equal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_obj_nonequal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_obj_plus(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_obj_property(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);
static int
mln_lang_obj_not(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2);

mln_lang_method_t mln_lang_obj_oprs = {
    mln_lang_obj_assign,
    mln_lang_obj_pluseq,
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
    mln_lang_obj_equal,
    mln_lang_obj_nonequal,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    mln_lang_obj_plus,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    mln_lang_obj_property,
    NULL,
    NULL,
    mln_lang_obj_not,
    NULL,
    NULL
};

static int
mln_lang_obj_assign(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    if (mln_lang_var_setValue(ctx, op1, op2) < 0) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    if ((*ret = mln_lang_var_convert(ctx, op1)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    return 0;
}

static int
mln_lang_obj_pluseq(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_s32_t type = mln_lang_var_getValType(op1);
    if (type != M_LANG_VAL_TYPE_STRING) {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
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

static int
mln_lang_obj_equal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    if (mln_lang_var_getValType(op1) != mln_lang_var_getValType(op2)) {
f:
        if ((*ret = mln_lang_var_createTmpFalse(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
        return 0;
    }
    if (mln_lang_var_getVal(op1)->data.obj != mln_lang_var_getVal(op2)->data.obj)
        goto f;
    if ((*ret = mln_lang_var_createTmpTrue(ctx, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    return 0;
}

static int
mln_lang_obj_nonequal(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    if (mln_lang_var_getValType(op1) != mln_lang_var_getValType(op2)) {
t:
        if ((*ret = mln_lang_var_createTmpTrue(ctx, NULL)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
        return 0;
    }
    if (mln_lang_var_getVal(op1)->data.obj != mln_lang_var_getVal(op2)->data.obj)
        goto t;
    if ((*ret = mln_lang_var_createTmpFalse(ctx, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    return 0;
}

static int
mln_lang_obj_plus(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    if (mln_lang_var_getValType(op2) != M_LANG_VAL_TYPE_STRING) {
        mln_lang_errmsg(ctx, "Operation Not support.");
        return -1;
    }
    mln_lang_method_t *method = mln_lang_methods[M_LANG_VAL_TYPE_STRING];
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

static int
mln_lang_obj_property(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    mln_lang_object_t *obj = mln_lang_var_getVal(op1)->data.obj;
    if (mln_lang_var_getVal(op2)->type != M_LANG_VAL_TYPE_CALL) {
        ASSERT(mln_lang_var_getValType(op2) == M_LANG_VAL_TYPE_STRING);
        mln_lang_var_t *var = mln_lang_set_member_search(obj->members, mln_lang_var_getVal(op2)->data.s);
        if (var == NULL) {
            mln_lang_var_t tmpvar;
            mln_lang_val_t tmpval;
            mln_rbtree_node_t *rn;
            tmpvar.type = M_LANG_VAR_NORMAL;
            tmpvar.name = mln_lang_var_getVal(op2)->data.s;
            tmpvar.val = &tmpval;
            tmpvar.inSet = obj->inSet;
            tmpvar.prev = tmpvar.next = NULL;
            tmpval.data.s = NULL;
            tmpval.type = M_LANG_VAL_TYPE_NIL;
            tmpval.ref = 1;
            if ((var = mln_lang_var_dup(ctx, &tmpvar)) == NULL) {
                mln_lang_errmsg(ctx, "No memory.");
                return -1;
            }
            if ((rn = mln_rbtree_node_new(obj->members, var)) == NULL) {
                mln_lang_var_free(var);
                mln_lang_errmsg(ctx, "No memory.");
                return -1;
            }
            mln_rbtree_insert(obj->members, rn);
        }
        if ((*ret = mln_lang_var_convert(ctx, var)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
    } else {
        if ((*ret = mln_lang_var_createTmpCall(ctx, op2->val->data.call)) == NULL) {
            mln_lang_errmsg(ctx, "No memory.");
            return -1;
        }
        mln_lang_funccall_val_addObject(op2->val->data.call, mln_lang_var_getVal(op1));
        op2->val->data.call = NULL;
    }
    return 0;
}

static int
mln_lang_obj_not(mln_lang_ctx_t *ctx, mln_lang_var_t **ret, mln_lang_var_t *op1, mln_lang_var_t *op2)
{
    ASSERT(op1->val->data.obj != NULL);
    if ((*ret = mln_lang_var_createTmpFalse(ctx, NULL)) == NULL) {
        mln_lang_errmsg(ctx, "No memory.");
        return -1;
    }
    return 0;
}

