	.file	1 "lab3_ex3.c"
	.section .mdebug.abi32
	.previous
	.nan	legacy
	.module	fp=xx
	.module	nooddspreg
	.abicalls
	.text
	.section	.text.startup,"ax",@progbits
	.align	2
	.globl	main
	.set	nomips16
	.set	nomicromips
	.ent	main
	.type	main, @function
main:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	lui	$2,%hi(source)
	lw	$3,%lo(source)($2)
	beq	$3,$0,$L9
	nop

	lui	$28,%hi(__gnu_local_gp)
	addiu	$2,$2,%lo(source)
	addiu	$28,$28,%lo(__gnu_local_gp)
	lw	$4,%got(dest)($28)
$L3:
	addiu	$2,$2,4
	sw	$3,0($4)
	addiu	$4,$4,4
	lw	$3,0($2)
	bne	$3,$0,$L3
	nop

$L9:
	move	$2,$0
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	main
	.size	main, .-main

	.comm	dest,40,4
	.globl	source
	.data
	.align	2
	.type	source, @object
	.size	source, 28
source:
	.word	3
	.word	1
	.word	4
	.word	1
	.word	5
	.word	9
	.word	0
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
