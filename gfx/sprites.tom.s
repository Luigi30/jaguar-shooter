	;;  jump condition codes
	;;    %00000:   T    always
	;;    %00100:   CC   carry clear (less than)
	;;    %01000:   CS   carry set   (greater or equal)
	;;    %00010:   EQ   zero set (equal)
	;;    %00001:   NE   zero clear (not equal)
	;;    %11000:   MI   negative set
	;;    %10100:   PL   negative clear
	;;    %00101:   HI   greater than

	.section text

	.include "jaguar.inc"
	.include "regmacros.inc"

	.globl 	_back_buffer
	.globl 	_SPRITES_LIST
	.globl  _shipsheet

	.macro BLIT_XY x,y
	move	\x,TEMP2
	move	\y,TEMP1
	shlq	#16,TEMP1
	or	TEMP2,TEMP1
	.endm

	.macro TEMP1_SWAP_WORDS	; Takes the long in TEMP1,
	move	TEMP1,TEMP2	; swaps the hi and lo words
	shlq	#16,TEMP1	; and stores the result in TEMP2.
	shrq	#16,TEMP2	; Clobbers TEMP1 and TEMP2.
	or	TEMP1,TEMP2
	.endm

	.gpu
_gpu_sprite_program::
	.org	$F03000
	
	R_A1_BASE	.equr	r19
	R_A1_PIXEL 	.equr	r20
	R_A1_FPIXEL 	.equr	r21
	R_A1_FLAGS 	.equr	r22
	R_A1_STEP 	.equr	r23
	R_A1_CLIP 	.equr	r24
	R_A2_BASE 	.equr	r25
	R_A2_PIXEL 	.equr	r26
	R_A2_STEP 	.equr	r27
	R_A2_FLAGS 	.equr	r28
	R_B_COUNT 	.equr	r29
	R_B_CMD   	.equr	r30

_gpu_sprite_program_start::
	
	.phrase
_gpu_sprite_test::
	movei   #G_FLAGS,r1       ; Status flags
	load    (r1),r0
	bset    #14,r0
	store   r0,(r1)           ; Switch the GPU/DSP to bank 1
	
.load_blitter_registers:
	movei	#B_CMD,r30
	movei	#B_COUNT,r29

	movei	#A2_FLAGS,r28
	movei	#A2_STEP,r27
	movei	#A2_PIXEL,r26
	movei	#A2_BASE,r25

	movei	#A1_CLIP,r24
	movei	#A1_STEP,r23
	movei	#A1_FLAGS,r22
	movei	#A1_FPIXEL,r21
	movei	#A1_PIXEL,r20
	movei	#A1_BASE,r19

	movei	#_GPU_blit_destination,r10
	movei	#_GPU_blit_destination_coordinate,r11
	movei	#_GPU_blit_source,r12
	movei	#_GPU_blit_sprite,r13

.write_a1:
	load	(r10),TEMP1		; A1_BASE
	store	TEMP1,(R_A1_BASE)

	load	(r11),TEMP1
	TEMP1_SWAP_WORDS
	store	TEMP2,(R_A1_PIXEL) 	;swapped X and Y parts
	
	movei	#0,TEMP1
	store	TEMP1,(R_A1_FPIXEL) 	;always 0
	movei	#B_PATD,TEMP2
	store	TEMP1,(TEMP2)		;clear the pattern register

	movei	#$00C80140,TEMP1
	store	TEMP1,(R_A1_CLIP)
	
	movei	#PITCH1|PIXEL8|WID320|XADDPIX|YADD0,TEMP1
	store	TEMP1,(R_A1_FLAGS)

	movei	#OFFSET_SpriteGraphic_Size,r14
	load	(r13),r10		;fetch the pointer
	load	(r14+r10),TEMP1		;load the Size field
	TEMP1_SWAP_WORDS		;swap hi and lo, result in TEMP2
	movei	#$0000FFFF,TEMP1
	and	TEMP1,TEMP2
	movei	#320,TEMP1
	sub	TEMP2,TEMP1
	move	TEMP1,r18
	store	TEMP1,(R_A1_STEP) 	;A1_STEP = BLIT_XY(320-sprite->size.x, 0)

.write_a2:
	load	(r12),TEMP1
	store	TEMP1,(R_A2_BASE) 	;A2_BASE = source

	movei	#OFFSET_SpriteGraphic_Location,r14
	load	(r13),r10
	load	(r14+r10),TEMP1		;load the Location field
	TEMP1_SWAP_WORDS
	store	TEMP2,(R_A2_PIXEL) 	;swap and store it

	movei	#OFFSET_SpriteGraphic_Size,r14
*	r10 still contains the correct pointer (r13)
	load	(r14+r10),TEMP1		;load the Size field
	TEMP1_SWAP_WORDS		;swap hi and lo, result in TEMP2
	movei	#$0000FFFF,TEMP1
	and	TEMP1,TEMP2
	movei	#256,TEMP1
	sub	TEMP2,TEMP1
	store	TEMP1,(R_A2_STEP) 	;A1_STEP = BLIT_XY(256-sprite->size.x, 0)

	movei	#PITCH1|PIXEL8|WID256|XADDPIX|YADD0,TEMP1
	store	TEMP1,(R_A2_FLAGS)

	movei	#OFFSET_SpriteGraphic_Size,r14
*	r10 still contains the correct pointer (r13) 
	load	(r14+r10),TEMP1		;load the Size field
	TEMP1_SWAP_WORDS
	store	TEMP2,(R_B_COUNT)

.do_blit:
	movei	#SRCEN|DSTEN|UPDA1|UPDA2|DCOMPEN|LFU_REPLACE,TEMP1
	store	TEMP1,(R_B_CMD)
	
	StopGPU

	.long
_GPU_blit_destination::			dc.l	0 ; pointer
_GPU_blit_destination_coordinate::	dc.l	0 ; uint16_t x, uint16_t y
_GPU_blit_source::			dc.l	0 ; pointer
_GPU_blit_sprite::			dc.l	0 ; pointer to SpriteGraphic
	
;;; SpriteGraphic struct
	OFFSET_SpriteGraphic_START	.equ	0
	OFFSET_SpriteGraphic_Name	.equ	0
	OFFSET_SpriteGraphic_Location	.equ	16
	OFFSET_SpriteGraphic_Size	.equ	20
	OFFSET_SpriteGraphic_END	.equ	24
;;; 
_gpu_process_sprite_list::
	STACK_PTR	.equr	r31
	
	CURRENT_NODE	.equr	r10
	SUCCESSOR	.equr	r11
	PREDECESSOR	.equr	r12
	SPRITE_COUNT	.equr	r13

	NODE_OFFSET	.equr	r14

	movei	#_gpu_stack_end,STACK_PTR
	
	movei	#_gpu_sprite_display_list,TEMP1
	load	(TEMP1),CURRENT_NODE	; list is now in r10

	movei	#0,SPRITE_COUNT

.node_loop:
	movei	#.done,JUMPADDR
	load	(CURRENT_NODE),CURRENT_NODE 	; advance to the next node
	load	(CURRENT_NODE),SUCCESSOR	; get the successor node if one exists
	cmpq	#0,SUCCESSOR			; is the successor NULL?
	jump	eq,(JUMPADDR)			; if so, we're done
	nop

;;; we have a node! load the rest of it and process it.
	movei	#OFFSET_SPRNODE_ln_Pred,NODE_OFFSET
	load	(r14+CURRENT_NODE),PREDECESSOR  ; get predecessor

;;; populate the blit values
	movei	#_back_buffer,TEMP1
	load	(TEMP1),TEMP1	; dereference
	movei	#_GPU_blit_destination,TEMP2
	store	TEMP1,(TEMP2)	; store the buffer address

	movei	#OFFSET_SPRNODE_location,NODE_OFFSET
	load	(NODE_OFFSET+CURRENT_NODE),TEMP1 ; this is a value, not a pointer.
	movei	#_GPU_blit_destination_coordinate,TEMP2
	store	TEMP1,(TEMP2)			 ; store the location value

	movei	#_shipsheet,TEMP1 ; shipsheet is always our source
	movei	#_GPU_blit_source,TEMP2
	store	TEMP1,(TEMP2)	  ; store a ptr to it
	
	move	CURRENT_NODE,TEMP1
	addq	#16,TEMP1
	movei	#_GPU_blit_sprite,TEMP2
	addq	#2,TEMP1	; interleave
	store	TEMP1,(TEMP2)   ; and this is our sprite value
	
	addq	#1,SPRITE_COUNT	; debugging: index

	;; TODO: JSR gpu_sprite_test

	movei	#.node_loop,JUMPADDR
	jump	t,(JUMPADDR)	; and loop again
	nop
.done:	
	StopGPU

	.long
_gpu_sprite_display_list::	dc.l	0 ; struct List *

;;; SpriteNode offset table
	OFFSET_SPRNODE_ln_Succ	.equ	0  ; struct Node *
	OFFSET_SPRNODE_ln_Pred	.equ	4  ; struct Node *
	OFFSET_SPRNODE_ln_Type	.equ	8  ; uint8
	OFFSET_SPRNODE_ln_Pri	.equ	9  ; int8
	OFFSET_SPRNODE_ln_Name	.equ	10 ; char *
	OFFSET_SPRNODE_NODESIZE	.equ	14

	OFFSET_SPRNODE_location	.equ	14 ; struct Coordinate
	OFFSET_SPRNODE_image    .equ    18 ; struct SpriteGraphic
	OFFSET_SPRNODE_delta	.equ    42 ; struct Coordinate
	OFFSET_SPRNODE_isPlayer .equ	46 ; uint8
	OFFSET_SPRNODE_padding	.equ	47 ; uint8
	OFFSET_SPRNODE_SIZE	.equ    48

;;;
_gpu_stack::			dcb.l	32,0	;32-position stack
_gpu_stack_end:	
	
	.68000 			;End section
_gpu_sprite_program_end::
