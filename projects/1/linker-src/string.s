# CS 61C Spring 2015 Project 1-2 
# string.s

#==============================================================================
#                              Project 1-2 Part 1
#                               String README
#==============================================================================
# In this file you will be implementing some utilities for manipulating strings.
# The functions you need to implement are:
#  - strlen()
#  - strncpy()
#  - copy_of_str()
# Test cases are in linker-tests/test_string.s
#==============================================================================

.data
newline:	.asciiz "\n"
tab:	.asciiz "\t"

.text
#------------------------------------------------------------------------------
# function strlen()
#------------------------------------------------------------------------------
# Arguments:
#  $a0 = string input
#
# Returns: the length of the string
#------------------------------------------------------------------------------
strlen:
	# backup
	addiu $sp, $sp, -8
	sw $s0, 0($sp)
	sw $s1, 4($sp)

	move $s0, $a0 # copy a0 to s0 so we don’t have to back it up
	lbu $s1, 0($s0) # load character for first iteration
Loop1:
	beq $s1, $0, Ret1 # break out of loop if loaded character is null terminator
	addiu $s0, $s0, 1 # increment pointer
	lbu $s1, 0($s0) # load next character
	j Loop1

Ret1:
	sub $v0, $s0, $a0
	
	# restore
	lw $s1, 4($sp)
	lw $s0, 0($sp)
	addiu $sp, $sp, 8

	jr $ra

#------------------------------------------------------------------------------
# function strncpy()
#------------------------------------------------------------------------------
# Arguments:
#  $a0 = pointer to destination array
#  $a1 = source string
#  $a2 = number of characters to copy
#
# Returns: the destination array
#------------------------------------------------------------------------------
strncpy:
	# backup
	addiu $sp, $sp, -8
	sw $s0, 0($sp)
	sw $s1, 4($sp)

	move $s0, $0
	lbu $s1, 0($a1) # load character for first iteration
Loop2:
	beq $s1, $0, Ret2 # break out of loop if loaded character is null terminator
	beq $s0, $a2, Ret2 # break out of loop if number of characters is reached
	add $t0, $s0, $a0 # address of dest
	sb $s1, 0($t0) # store char to dest
	addiu $s0, $s0, 1 # increment index
	add $t0, $s0, $a1 # address of src
	lbu $s1, 0($t0) # load next character
	j Loop2

Ret2:
	add $t0, $s0, $a0 # address of dest
	sb $0, 0($t0) # store null terminator to dest
	move $v0, $a0

	# restore
	lw $s1, 4($sp)
	lw $s0, 0($sp)
	addiu $sp, $sp, 8

	jr $ra

#------------------------------------------------------------------------------
# function copy_of_str()
#------------------------------------------------------------------------------
# Creates a copy of a string. You will need to use sbrk (syscall 9) to allocate
# space for the string. strlen() and strncpy() will be helpful for this function.
# In MARS, to malloc memory use the sbrk syscall (syscall 9). See help for details.
#
# Arguments:
#   $a0 = string to copy
#
# Returns: pointer to the copy of the string
#------------------------------------------------------------------------------
copy_of_str:
	# backup
	addiu $sp, $sp, -12
	sw $s0, 0($sp)
	sw $s1, 4($sp)
	sw $ra, 8($sp)

	move $s0, $a0
	jal strlen
	move $s1, $v0 # backup strlen

	move $a0, $v0 # strlen to a0
	addi $a0, $a0, 1 # strlen + 1 (for null terminator)
	li $v0, 9 # allocate space for new string
	syscall

	move $a0, $v0 # a0 = pointer to new space (dest)
	move $a1, $s0 # a1 = pointer to source
	move $a2, $s1 # a2 = strlen
	jal strncpy

	# restore	
	lw $ra, 8($sp)
	lw $s1, 4($sp)
	lw $s0, 0($sp)
	addiu $sp, $sp, 12

	jr $ra

###############################################################################
#                 DO NOT MODIFY ANYTHING BELOW THIS POINT                       
###############################################################################

#------------------------------------------------------------------------------
# function streq() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Arguments:
#  $a0 = string 1
#  $a1 = string 2
#
# Returns: 0 if string 1 and string 2 are equal, -1 if they are not equal
#------------------------------------------------------------------------------
streq:
	beq $a0, $0, streq_false	# Begin streq()
	beq $a1, $0, streq_false
streq_loop:
	lb $t0, 0($a0)
	lb $t1, 0($a1)
	addiu $a0, $a0, 1
	addiu $a1, $a1, 1
	bne $t0, $t1, streq_false
	beq $t0, $0, streq_true
	j streq_loop
streq_true:
	li $v0, 0
	jr $ra
streq_false:
	li $v0, -1
	jr $ra			# End streq()

#------------------------------------------------------------------------------
# function dec_to_str() - DO NOT MODIFY THIS FUNCTION
#------------------------------------------------------------------------------
# Convert a number to its unsigned decimal integer string representation, eg.
# 35 => "35", 1024 => "1024". 
#
# Arguments:
#  $a0 = int to write
#  $a1 = character buffer to write into
#
# Returns: the number of digits written
#------------------------------------------------------------------------------
dec_to_str:
	li $t0, 10			# Begin dec_to_str()
	li $v0, 0
dec_to_str_largest_divisor:
	div $a0, $t0
	mflo $t1		# Quotient
	beq $t1, $0, dec_to_str_next
	mul $t0, $t0, 10
	j dec_to_str_largest_divisor
dec_to_str_next:
	mfhi $t2		# Remainder
dec_to_str_write:
	div $t0, $t0, 10	# Largest divisible amount
	div $t2, $t0
	mflo $t3		# extract digit to write
	addiu $t3, $t3, 48	# convert num -> ASCII
	sb $t3, 0($a1)
	addiu $a1, $a1, 1
	addiu $v0, $v0, 1
	mfhi $t2		# setup for next round
	bne $t2, $0, dec_to_str_write
	jr $ra			# End dec_to_str()
