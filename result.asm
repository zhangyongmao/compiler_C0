.data
const1:	.word	1
const2:	.word	-100
const3:	.word	100
const4:	.word	43
var1:	.space	4
var2:	.space	4
var3:	.space	4
var4:	.space	4
string0:	.asciiz	"input there number"
string1:	.asciiz	"the max number:"
string2:	.asciiz	"Input the base:"
string3:	.asciiz	"Input the step:"
string4:	.asciiz	"Input the number:"
string5:	.asciiz	"the result:"
string6:	.asciiz	""
string7:	.asciiz	""
string8:	.asciiz	"Input the choice: 1 2 3 , 0 for return "
string9:	.asciiz	"good"
string10:	.asciiz	"better"
string11:	.asciiz	"best"
.text
.global main
		j	main
max:
		lw	$a0	4($fp)
		subi	$sp	$sp	4
		sw	$a0	-8($fp)
		lw	$a0	8($fp)
		subi	$sp	$sp	4
		sw	$a0	-12($fp)
		lw	$a0	12($fp)
		subi	$sp	$sp	4
		sw	$a0	-16($fp)
		subi	$sp	$sp	4
		lw	$t1	-8($fp)
		lw	$t2	-12($fp)
		bge	$t1	$t2	LABEL_3
		j	LABEL_2
LABEL_3:
		j	LABEL_0
LABEL_2:
		j	LABEL_1
LABEL_0:
		lw	$t3	-16($fp)
		bge	$t1	$t3	LABEL_7
		j	LABEL_6
LABEL_7:
		j	LABEL_4
LABEL_6:
		j	LABEL_5
LABEL_4:
		sw	$t1	-20($fp)
		j	LABEL_8
LABEL_5:
		sw	$t3	-20($fp)
LABEL_8:
		j	LABEL_9
LABEL_1:
		bge	$t2	$t3	LABEL_13
		j	LABEL_12
LABEL_13:
		j	LABEL_10
LABEL_12:
		j	LABEL_11
LABEL_10:
		sw	$t2	-20($fp)
		j	LABEL_14
LABEL_11:
		sw	$t3	-20($fp)
LABEL_14:
LABEL_9:
		lw	$t4	-20($fp)
		move	$v1	$t4
		move	$sp	$fp
		lw	$fp	($sp)
		move	$t0	$ra
		lw	$ra	-4($sp)
		jr	$t0
add1:
		lw	$a0	4($fp)
		subi	$sp	$sp	4
		sw	$a0	-8($fp)
		lw	$a0	8($fp)
		subi	$sp	$sp	4
		sw	$a0	-12($fp)
		lw	$a0	12($fp)
		subi	$sp	$sp	4
		sw	$a0	-16($fp)
LABEL_15:
		lw	$t1	-16($fp)
		li	$t2	0
		ble	$t1	$t2	LABEL_16
		lw	$t3	-8($fp)
		lw	$t4	-12($fp)
		subi	$sp	$sp	4
		add	$t5	$t3	$t4
		sw	$t5	-8($fp)
		li	$t6	1
		subi	$sp	$sp	4
		sub	$t7	$t1	$t6
		sw	$t7	-16($fp)
		j	LABEL_15
LABEL_16:
		lw	$t8	-8($fp)
		move	$v1	$t8
		move	$sp	$fp
		lw	$fp	($sp)
		move	$t0	$ra
		lw	$ra	-4($sp)
		jr	$t0
main:
		subi	$sp	$sp	4
		move	$fp	$sp
		subi	$sp	$sp	8
		subi	$sp	$sp	4
		subi	$sp	$sp	4
		subi	$sp	$sp	4
		subi	$sp	$sp	4
		subi	$sp	$sp	4
		subi	$sp	$sp	4
		subi	$sp	$sp	4
		subi	$sp	$sp	4
		li	$t1	100
		la	$t0	var3
		sw	$t1	($t0)
		li	$t2	43
		la	$t0	var4
		sw	$t2	($t0)
		la	$t0	string0
		move	$a0	$t0
		li	$v0	4
		syscall
		li	$v0	5
		syscall
		sw	$v0	-8($fp)
		li	$v0	5
		syscall
		sw	$v0	-12($fp)
		li	$v0	5
		syscall
		sw	$v0	-16($fp)
		subi	$sp	$sp	4
		lw	$t3	-16($fp)
		sw	$t3	-40($fp)
		subi	$sp	$sp	4
		lw	$t4	-12($fp)
		sw	$t4	-44($fp)
		subi	$sp	$sp	4
		lw	$t5	-8($fp)
		sw	$t5	-48($fp)
		sw	$t3	-16($fp)
		sw	$t4	-12($fp)
		sw	$t5	-8($fp)
		sw	$fp	($sp)
		move	$fp	$sp
		subi	$sp	$sp	4
		sw	$ra	($sp)
		subi	$sp	$sp	4
		jal	max
		addi	$sp	$sp	12
		subi	$sp	$sp	4
		sw	$v1	-40($fp)
		lw	$t1	-40($fp)
		la	$t0	var1
		sw	$t1	($t0)
		la	$t0	string1
		move	$a0	$t0
		li	$v0	4
		syscall
		lw	$a0	-40($fp)
		li	$v0	1
		syscall
		la	$t0	string2
		move	$a0	$t0
		li	$v0	4
		syscall
		li	$v0	5
		syscall
		sw	$v0	-20($fp)
		la	$t0	string3
		move	$a0	$t0
		li	$v0	4
		syscall
		li	$v0	5
		syscall
		sw	$v0	-24($fp)
		la	$t0	string4
		move	$a0	$t0
		li	$v0	4
		syscall
		li	$v0	5
		syscall
		sw	$v0	-28($fp)
		subi	$sp	$sp	4
		lw	$t2	-28($fp)
		sw	$t2	-44($fp)
		subi	$sp	$sp	4
		lw	$t3	-24($fp)
		sw	$t3	-48($fp)
		subi	$sp	$sp	4
		lw	$t4	-20($fp)
		sw	$t4	-52($fp)
		sw	$t1	-40($fp)
		sw	$t2	-28($fp)
		sw	$t3	-24($fp)
		sw	$t4	-20($fp)
		sw	$fp	($sp)
		move	$fp	$sp
		subi	$sp	$sp	4
		sw	$ra	($sp)
		subi	$sp	$sp	4
		jal	add1
		addi	$sp	$sp	12
		subi	$sp	$sp	4
		sw	$v1	-44($fp)
		la	$t0	string5
		move	$a0	$t0
		li	$v0	4
		syscall
		la	$t0	string6
		move	$a0	$t0
		li	$v0	4
		syscall
		lw	$a0	-44($fp)
		li	$v0	1
		syscall
		la	$t0	string7
		move	$a0	$t0
		li	$v0	4
		syscall
		la	$t0	var4
		lw	$a0	($t0)
		li	$v0	1
		syscall
		la	$t0	string8
		move	$a0	$t0
		li	$v0	4
		syscall
		li	$v0	5
		syscall
		sw	$v0	-36($fp)
LABEL_17:
		lw	$t1	-36($fp)
		li	$t2	0
		beq	$t1	$t2	LABEL_18
		li	$t3	1
		beq	$t1	$t3	LABEL_19
		li	$t4	2
		beq	$t1	$t4	LABEL_20
		li	$t5	3
		beq	$t1	$t5	LABEL_21
LABEL_19:
		la	$t0	string9
		move	$a0	$t0
		li	$v0	4
		syscall
LABEL_20:
		la	$t0	string10
		move	$a0	$t0
		li	$v0	4
		syscall
LABEL_21:
		la	$t0	string11
		move	$a0	$t0
		li	$v0	4
		syscall
		li	$v0	5
		syscall
		sw	$v0	-36($fp)
		j	LABEL_17
LABEL_18:
		li	$v0	10
		syscall
