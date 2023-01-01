# This is the start of the original array.
Original: .word 200, 270, 250, 100
.word 205, 230, 105, 235
.word 190, 95, 90, 205
.word 80, 205, 110, 215
# The next statement allocates room for the other array.
# The array takes up 4*16=64 bytes.
# 
Second: .space 64
# Summer 2022 ECE 3058
.align 2
.globl main
.text
main: # Your fully commented program starts here.





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

