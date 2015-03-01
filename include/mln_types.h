
/*
 * Copyright (C) Niklaus F.Schen.
 */

#ifndef __MLN_TYPES
#define __MLN_TYPES

#include "mln_defs.h"
#include "mln_path.h"
#if defined(__GNUC__) && (__GNUC__ >= 4 && __GNUC_MINOR__ > 1)
typedef long                  mln_lock_t;
#elif defined(i386) || defined(__x86_64)
typedef unsigned long         mln_lock_t;
#else
#include <pthread.h>
typedef pthread_spinlock_t    mln_lock_t;
#endif

typedef unsigned char         mln_u8_t;
typedef char                  mln_s8_t;
typedef unsigned short        mln_u16_t;
typedef short                 mln_s16_t;
typedef unsigned int          mln_u32_t;
typedef int                   mln_s32_t;
#ifdef i386
typedef unsigned long long    mln_u64_t;
typedef long long             mln_s64_t;
#else
typedef unsigned long         mln_u64_t;
typedef long                  mln_s64_t;
#endif
typedef char *                mln_s8ptr_t;
typedef unsigned char *       mln_u8ptr_t;
typedef short *               mln_s16ptr_t;
typedef unsigned short *      mln_u16ptr_t;
typedef int *                 mln_s32ptr_t;
typedef unsigned int *        mln_u32ptr_t;
#ifdef i386
typedef long long *           mln_s64ptr_t;
typedef unsigned long long *  mln_u64ptr_t;
#else
typedef long *                mln_s64ptr_t;
typedef unsigned long *       mln_u64ptr_t;
#endif
typedef size_t                mln_size_t;
typedef long                  mln_sptr_t;
typedef unsigned long         mln_uptr_t;
typedef long                  mln_sauto_t;
typedef unsigned long         mln_uauto_t;

#endif

