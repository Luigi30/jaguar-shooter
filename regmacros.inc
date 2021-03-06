; Equates
BMP_WIDTH	.equ	320
BMP_HEIGHT	.equ	200

; Registers we want to be consistent with
	.gpu
TEMP1		.equr	r0
TEMP2		.equr	r1

LOOPCOUNTER	.equr	r2
LOOPEND		.equr	r3

VIDMEMBASE	.equr	r4
LOADTEMP	.equr	r5
VIDDEST		.equr	r6

JUMPADDR	.equr	r7
RETURNADDR	.equr	r8
SP		.equr	r31
	
	.macro	LoadValue val, destination
        movei	#\val,LOADTEMP
        load	(LOADTEMP),\destination
	.endm
	
	.macro	StopGPU
	moveq   #0,TEMP1         	; Stop GPU
	movei   #G_CTRL,TEMP2
	store   TEMP1,(TEMP2)
	nop
	nop
	.endm

	.macro	StopDSP
	moveq	#0,TEMP1		; Stop DSP
	movei	#D_CTRL,TEMP2
	store	TEMP1,(TEMP2)
	nop
	nop
	.endm

	.MACRO	GPU_JSR		; Jump to subroutine macro
				;	trashes R2
	subq	#4,SP		; adjust the stack pointer
	nop
	move	PC,r2		; determine current program address
	addq	#16,r2		; new address for after this macro
	store	r2,(SP)		; push return address onto stack
	movei	\1,r2		; load up subroutine address
	jump	t,(r2)		; jump to subroutine
	nop			; jump does not occur until this instr

	.ENDM

	.MACRO	GPU_RTS		; Return from Subroutine macro
				;	trashes r2
	load	(SP),r2		; load up return address
	addq	#4,SP		; adjust the stack pointer
	jump	t,(r2)		; jump back to return address
	nop			; jump does not occur until this instr
	.ENDM

	.MACRO	GPU_REG_BANK_0
	movei   #G_FLAGS,r1       ; Status flags
	load    (r1),r0
	bclr    #14,r0
	store   r0,(r1)           ; Switch the GPU/DSP to bank 0
	.ENDM

	.MACRO	GPU_REG_BANK_1
	movei   #G_FLAGS,r1       ; Status flags
	load    (r1),r0
	bset    #14,r0
	store   r0,(r1)           ; Switch the GPU/DSP to bank 1
	.ENDM

	.macro	WAIT_FOR_BLITTER_LOCK
	movei	#_BLITTER_LOCK_CPU,TEMP1
.blitter_lock_loop_\~:
	loadb	(TEMP1),TEMP2
	btst	#0,TEMP2
	jr	ne,.blitter_lock_loop_\~
	nop
	.endm

	.macro	WAIT_FOR_BLITTER_IDLE
	movei	#B_CMD,TEMP1
.blitter_in_use_loop_\~:
	load	(TEMP1),TEMP2
	btst	#0,TEMP2
	jr	eq,.blitter_in_use_loop_\~
	nop
	.endm

	.macro	LOCK_BLITTER
	nop
	movei	#1,TEMP1
	movei	#_BLITTER_LOCK_GPU,TEMP2
	storeb	TEMP1,(TEMP2)
	.endm

	.macro	UNLOCK_BLITTER
	nop
	movei	#0,TEMP1
	movei	#_BLITTER_LOCK_GPU,TEMP2
	storeb	TEMP1,(TEMP2)
	.endm

	.macro  TAKE_BLIT_PRIORITY
	nop
	movei	#1,TEMP1
	movei	#_BLITTER_LOCK_ALLOW,TEMP2
	storeb	TEMP1,(TEMP2)
	.endm

	.macro  DUMP_GPU_REGISTERS
	movei	#_gpu_register_dump,TEMP2
	store	r0,(TEMP2)
	
	addq	#4,TEMP2
	store	r1,(TEMP2)
	
	addq	#4,TEMP2
	store	r2,(TEMP2)

	addq	#4,TEMP2
	store	r3,(TEMP2)

	addq	#4,TEMP2
	store	r4,(TEMP2)

	addq	#4,TEMP2
	store	r5,(TEMP2)

	addq	#4,TEMP2
	store	r6,(TEMP2)

	addq	#4,TEMP2
	store	r7,(TEMP2)

	addq	#4,TEMP2
	store	r8,(TEMP2)

	addq	#4,TEMP2
	store	r9,(TEMP2)

	addq	#4,TEMP2
	store	r10,(TEMP2)

	addq	#4,TEMP2
	store	r11,(TEMP2)

	addq	#4,TEMP2
	store	r12,(TEMP2)

	addq	#4,TEMP2
	store	r13,(TEMP2)

	addq	#4,TEMP2
	store	r14,(TEMP2)

	addq	#4,TEMP2
	store	r15,(TEMP2)

	addq	#4,TEMP2
	store	r16,(TEMP2)

	addq	#4,TEMP2
	store	r17,(TEMP2)

	addq	#4,TEMP2
	store	r18,(TEMP2)

	addq	#4,TEMP2
	store	r19,(TEMP2)

	addq	#4,TEMP2
	store	r20,(TEMP2)

	addq	#4,TEMP2
	store	r21,(TEMP2)

	addq	#4,TEMP2
	store	r22,(TEMP2)

	addq	#4,TEMP2
	store	r23,(TEMP2)

	addq	#4,TEMP2
	store	r24,(TEMP2)

	addq	#4,TEMP2
	store	r25,(TEMP2)

	addq	#4,TEMP2
	store	r26,(TEMP2)

	addq	#4,TEMP2
	store	r27,(TEMP2)

	addq	#4,TEMP2
	store	r28,(TEMP2)

	addq	#4,TEMP2
	store	r29,(TEMP2)

	addq	#4,TEMP2
	store	r30,(TEMP2)

	addq	#4,TEMP2
	store	r31,(TEMP2)

	.endm