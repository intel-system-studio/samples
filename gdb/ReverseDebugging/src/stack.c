/* 
 * ============================================================== 
 * 
 * SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT, 
 * http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/ 
 * 
 * Copyright (C) Intel Corporation 
 * 
 * THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
 * PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS. 
 * 
 * ============================================================= 
 */

/* Trace example - corrupting the stack using function pointers.
 *
 * By passing the wrong argument to main::mask (= fun_A), we increment the
 * function pointer to main::comp (= fun_B) to point beyond the return address
 * push, thus corrupting the stack.
 *
 */


struct A_arg {
    long arg1;
    long arg2;
    long count;
};

static long fun_A(void* arg) {
    struct A_arg* myarg = (struct A_arg*) arg;
    if (!myarg) return -1;

    myarg->count += 1;

    return myarg->arg1 + myarg->arg2;
}

struct B_arg {
    long arg1;
    long arg2;
};

static long fun_B(void* arg) {
    struct B_arg* myarg = (struct B_arg*) arg;
    if (!myarg) return -1;

    return myarg->arg1 + myarg->arg2;
}

typedef long (*fun_t) (void*);
static fun_t functions[] = {
    fun_A,
    fun_B
};

static long compute(long (*mask) (void*), void* marg,
                   long (*comp) (void*), void *carg) {
    mask(marg);
    return comp(carg);
}

static long do_crash(void) {
    struct B_arg arg = { 42, 23 };
    long (*comp) (void*) = functions[1];
    long (*mask) (void*) = functions[0];

    mask(&arg);
    return comp(&arg);
}

static long do_this();
static long do_that() {
  return do_crash();
}

static long do_this() {

do_that();

  return 0;
}

extern int main(void) {
  return do_this();
}
