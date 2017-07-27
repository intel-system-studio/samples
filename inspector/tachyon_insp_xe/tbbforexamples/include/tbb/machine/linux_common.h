/*

!==============================================================
!
! SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
! http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
!
! Copyright 2016 Intel Corporation
!
! THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
! NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
! PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
!
! =============================================================
*/

#ifndef __TBB_machine_H
#error Do not #include this internal file directly; use public TBB headers instead.
#endif

#include <sched.h>
#define __TBB_Yield()  sched_yield()

#include <unistd.h>
/* Futex definitions */
#include <sys/syscall.h>

#if defined(SYS_futex)

#define __TBB_USE_FUTEX 1
#include <limits.h>
#include <errno.h>
// Unfortunately, some versions of Linux do not have a header that defines FUTEX_WAIT and FUTEX_WAKE.

#ifdef FUTEX_WAIT
#define __TBB_FUTEX_WAIT FUTEX_WAIT
#else
#define __TBB_FUTEX_WAIT 0
#endif

#ifdef FUTEX_WAKE
#define __TBB_FUTEX_WAKE FUTEX_WAKE
#else
#define __TBB_FUTEX_WAKE 1
#endif

#ifndef __TBB_ASSERT
#error machine specific headers must be included after tbb_stddef.h
#endif

namespace tbb {

namespace internal {

inline int futex_wait( void *futex, int comparand ) {
    int r = syscall( SYS_futex,futex,__TBB_FUTEX_WAIT,comparand,NULL,NULL,0 );
#if TBB_USE_ASSERT
    int e = errno;
    __TBB_ASSERT( r==0||r==EWOULDBLOCK||(r==-1&&(e==EAGAIN||e==EINTR)), "futex_wait failed." );
#endif /* TBB_USE_ASSERT */
    return r;
}

inline int futex_wakeup_one( void *futex ) {
    int r = ::syscall( SYS_futex,futex,__TBB_FUTEX_WAKE,1,NULL,NULL,0 );
    __TBB_ASSERT( r==0||r==1, "futex_wakeup_one: more than one thread woken up?" );
    return r;
}

inline int futex_wakeup_all( void *futex ) {
    int r = ::syscall( SYS_futex,futex,__TBB_FUTEX_WAKE,INT_MAX,NULL,NULL,0 );
    __TBB_ASSERT( r>=0, "futex_wakeup_all: error in waking up threads" );
    return r;
}

} /* namespace internal */

} /* namespace tbb */

#endif /* SYS_futex */
