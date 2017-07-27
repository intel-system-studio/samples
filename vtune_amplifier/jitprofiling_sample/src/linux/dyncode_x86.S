//  ==============================================================
//  
//  SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
//  http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//  
//  Copyright 2005-2016 Intel Corporation
//  
//  THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
//  NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//  PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//  
//  =============================================================

	.file	"dyncode_ia32.S"
	.text

.globl split_proc
	.type	split_proc, @function
split_proc:
split_proc_begin:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$24, %esp
	movl	$0, -4(%ebp)
	movl	$0, -4(%ebp)
	jmp	.L2
.L3:
	movl	-4(%ebp), %eax
	sall	$2, %eax
	addl	8(%ebp), %eax
	movl	(%eax), %eax
	call	*%eax
	incl	-4(%ebp)
.L2:
	movl	-4(%ebp), %eax
	cmpl	12(%ebp), %eax
	jb	.L3
	leave
	ret
	.size	split_proc, .-split_proc
split_proc_end:

.globl dyn_hotspot1
	.type	dyn_hotspot1, @function
dyn_hotspot1:
dyn_hotspot1_begin:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	fldz
	fstpl	-16(%ebp)
	movl	$0, -8(%ebp)
	movl	$0, -4(%ebp)
	movl	$0, -8(%ebp)
	jmp	.L7
.L8:
	movl	$0, -4(%ebp)
	jmp	.L9
.L10:
	movl	-8(%ebp), %eax
	imull	-4(%ebp), %eax
	pushl	%eax
	fildl	(%esp)
	leal	4(%esp), %esp
	fldl	-16(%ebp)
	faddp	%st, %st(1)
	fstpl	-16(%ebp)
	incl	-4(%ebp)
.L9:
	cmpl	$9999, -4(%ebp)
	jle	.L10
	incl	-8(%ebp)
.L7:
	cmpl	$99999, -8(%ebp)
	jle	.L8
	leave
	ret
	.size	dyn_hotspot1, .-dyn_hotspot1
dyn_hotspot1_end:

.globl dyn_hotspot2
	.type	dyn_hotspot2, @function
dyn_hotspot2:
dyn_hotspot2_begin:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$16, %esp
	fldz
	fstpl	-16(%ebp)
	movl	$0, -4(%ebp)
	movl	$0, -4(%ebp)
	jmp	.L15
.L16:
	movl	-4(%ebp), %eax
	addl	-4(%ebp), %eax
	pushl	%eax
	fildl	(%esp)
	leal	4(%esp), %esp
	fldl	-16(%ebp)
	faddp	%st, %st(1)
	fstpl	-16(%ebp)
	incl	-4(%ebp)
.L15:
	cmpl	$99999999, -4(%ebp)
	jle	.L16
	leave
	ret
	.size	dyn_hotspot2, .-dyn_hotspot2
dyn_hotspot2_end:

.globl g_split_proc_size
        .data
        .align 4
        .type   g_split_proc_size, @object
        .size   g_split_proc_size, 4
g_split_proc_size:
        .long   split_proc_end - split_proc_begin

.globl g_dyn_hotspot1_size
        .data
        .align 4
        .type   g_dyn_hotspot1_size, @object
        .size   g_dyn_hotspot1_size, 4
g_dyn_hotspot1_size:
        .long   dyn_hotspot1_end - dyn_hotspot1_begin

.globl g_dyn_hotspot2_size
        .data
        .align 4
        .type   g_dyn_hotspot2_size, @object
        .size   g_dyn_hotspot2_size, 4
g_dyn_hotspot2_size:
        .long   dyn_hotspot2_end - dyn_hotspot2_begin
