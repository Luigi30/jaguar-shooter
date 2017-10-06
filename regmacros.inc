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
SP		.equr	r9
	
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
	