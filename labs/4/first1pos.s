.data
shouldben1:	.asciiz "Should be -1, and firstposshift and firstposmask returned: "
shouldbe0:	.asciiz "Should be 0 , and firstposshift and firstposmask returned: "
shouldbe16:	.asciiz "Should be 16, and firstposshift and firstposmask returned: "
shouldbe31:	.asciiz "Should be 31, and firstposshift and firstposmask returned: "

.text
main:
	la	$a0, shouldbe31
	jal	print_str
	lui	$a0, 0x8000	# should be 31
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	lui	$a0, 0x8000
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldbe16
	jal	print_str
	lui	$a0, 0x0001	# should be 16
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	lui	$a0, 0x0001
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldbe0
	jal	print_str
	li	$a0, 1		# should be 0
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	li	$a0, 1
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldben1
	jal	print_str
	move	$a0, $0		# should be -1
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	move	$a0, $0
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	li	$v0, 10
	syscall

first1posshift:
	li $s1, 31
loop1:
	blt $s1, $0, notfound1
	li $t1, 1 
	sllv $t0, $t1, $s1
	and	$t0, $a0, $t0
	srlv $t0, $t0, $s1
	beq $t1, $t0, found1 # is equal to 1
	addi $s1, $s1, -1
	j loop1
notfound1:
	addi $v0, $0, -1
	jr $ra
found1:
	add $v0, $0, $s1
	jr $ra

first1posmask:
	li $s1, 31
	li $s2, 0x80000000
loop2:
	blt $s1, $0, notfound2
	srlv $t0, $s2, $s1
	and	$t1, $a0, $t0
	beq $t0, $t1, found2
	addi $s1, $s1, -1
	j loop2
notfound2:
	addi $v0, $0, -1
	jr $ra
found2:
	li $t4, 31
	sub $v0, $t4, $s1
	jr $ra

print_int:
	move	$a0, $v0
	li	$v0, 1
	syscall
	jr	$ra

print_str:
	li	$v0, 4
	syscall
	jr	$ra

print_space:
	li	$a0, ' '
	li	$v0, 11
	syscall
	jr	$ra

print_newline:
	li	$a0, '\n'
	li	$v0, 11
	syscall
	jr	$ra
