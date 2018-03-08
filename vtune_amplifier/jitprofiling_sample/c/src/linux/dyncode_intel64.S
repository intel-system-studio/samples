//  ==============================================================
//  
//  SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
//  http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//  
//  Copyright (C) Intel Corporation
//  
//  THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
//  NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
//  PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//  
//  =============================================================

	.file	"dyncode_em64t.s"
	.text

.globl split_proc
	.type	split_proc, @function
split_proc:
split_proc_begin:
.LFB5:
	pushq	%rbp
.LCFI0:
	movq	%rsp, %rbp
.LCFI1:
	subq	$32, %rsp
.LCFI2:
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movl	$0, -4(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L2
.L3:
	mov	-4(%rbp), %eax
	salq	$3, %rax
	addq	-24(%rbp), %rax
	movq	(%rax), %rax
	call	*%rax
	incl	-4(%rbp)
.L2:
	movl	-4(%rbp), %eax
	cmpl	-28(%rbp), %eax
	jb	.L3
	leave
	ret
.LFE5:
	.size	split_proc, .-split_proc
split_proc_end:

.globl dyn_hotspot1
	.type	dyn_hotspot1, @function
dyn_hotspot1:
dyn_hotspot1_begin:
.LFB6:
	pushq	%rbp
.LCFI3:
	movq	%rsp, %rbp
.LCFI4:
	movl	$0, %eax
	movq	%rax, -16(%rbp)
	movl	$0, -8(%rbp)
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L7
.L8:
	movl	$0, -4(%rbp)
	jmp	.L9
.L10:
	movl	-8(%rbp), %eax
	imull	-4(%rbp), %eax
	cvtsi2sd	%eax, %xmm1
	movlpd	-16(%rbp), %xmm0
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -16(%rbp)
	incl	-4(%rbp)
.L9:
	cmpl	$9999, -4(%rbp)
	jle	.L10
	incl	-8(%rbp)
.L7:
	cmpl	$99999, -8(%rbp)
	jle	.L8
	leave
	ret
.LFE6:
	.size	dyn_hotspot1, .-dyn_hotspot1
dyn_hotspot1_end:

.globl dyn_hotspot2
	.type	dyn_hotspot2, @function
dyn_hotspot2:
dyn_hotspot2_begin:
.LFB7:
	pushq	%rbp
.LCFI5:
	movq	%rsp, %rbp
.LCFI6:
	movl	$0, %eax
	movq	%rax, -16(%rbp)
	movl	$0, -4(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L15
.L16:
	movl	-4(%rbp), %eax
	addl	-4(%rbp), %eax
	cvtsi2sd	%eax, %xmm1
	movlpd	-16(%rbp), %xmm0
	addsd	%xmm1, %xmm0
	movsd	%xmm0, -16(%rbp)
	incl	-4(%rbp)
.L15:
	cmpl	$99999999, -4(%rbp)
	jle	.L16
	leave
	ret
.LFE7:
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

