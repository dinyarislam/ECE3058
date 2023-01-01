# Name: Dinyar Islam
# GTID: 903586823
# This is the start of the original array.
Original: .word 200, 270, 250, 100
.word 205, 230, 105, 235
.word 190, 95, 90, 205
.word 80, 205, 110, 215
# The next statement allocates room for the other array.
# The array takes up 4*16=64 bytes.
# 
Second: .space 64
# Fall 2022 ECE 3058
.align 2
.globl main
.text
main: # Your fully commented program starts here.
addi $s1, $0, 0 # set row index $s1 = 0
addi $s5, $0, 4

Loop1:
addi $s0, $0, 0 # set column index $s0 = 0
Loop2:
sll $s2, $s0, 2 # $s0 * 4
sll $s3, $s1, 4 # $s1 * 16
add $s3, $s2, $s3 # $s3 = $s2 + $s3 (original matrix index)
lw $s4, Original($s3) # load $s4 = Original[$s3]

sll $s2, $s0, 4 # $s0 * 16
sll $s3, $s1, 2 # $s1 * 4
add $s3, $s2, $s3 # $s3 = $s2 + $s3 (Second matrix index)
sw $s4, Second($s3) # store Second[$s3] = $s4

addi $s0, $s0, 1 # increment $s0
bne $s0, $s5, Loop2 # Loop back if $s0 = 4

addi $s1, $s1, 1 # increment $s1
bne $s1, $s5, Loop1 # Loop back if $s1 = 4


End:
# Your fully commented program ends here. Remeber to store 
# the transposed matrix in the pre-allocated memory space. 

# Please make sure your code finishes here so that that grading
# code below can execute. If your assembly exits by jumping out 
# of an inner loop, you can add a lable above and jump to that
# label instead

# DO NOT MODIFY. THIS IS FOR GRADING PURPOSES
add $a3, $0, $0
addiu $t1, $0, 64

checker:

lw $t2, Second($a3)
addiu $a3, $a3, 4

li $v0, 1
move $a0, $t2

syscall
bne $a3, $t1, checker


Exit: li $v0, 10 # terminate program
syscall
