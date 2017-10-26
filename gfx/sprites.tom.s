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

	;; all uint8_t
	.globl  _BLITTER_LOCK_CPU ; CPU has locked the blitter
	.globl  _BLITTER_LOCK_GPU ; GPU has locked the blitter
	.globl  _BLITTER_LOCK_ALLOW ; 0 = CPU has priority, 1 = GPU has priority
	
	.globl 	_back_buffer
	.globl 	_SPRITES_LIST
	.globl  _shipsheet

	.globl	_mobj_bg_tiles	; start of the mobj_bg_tiles array
	
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
_gpu_process_sprite_list::	
	CURRENT_NODE	.equr	r3
	SUCCESSOR	.equr	r4
	PREDECESSOR	.equr	r5
	SPRITE_COUNT	.equr	r6

	NODE_OFFSET	.equr	r15

	GPU_REG_BANK_1
	movei	#0,r8
	movei	#0,r9
	movei	#0,r16
	movei	#0,r17

	movei	#_gpu_stack_end,SP
	
	movei	#_gpu_sprite_display_list,TEMP1
	load	(TEMP1),CURRENT_NODE	; list is now in r2

	movei	#0,SPRITE_COUNT

.node_loop:
	movei	#.done,JUMPADDR
	load	(CURRENT_NODE),CURRENT_NODE     ; advance to the next node
	load	(CURRENT_NODE),SUCCESSOR	; get the successor if one exists

	cmpq	#0,SUCCESSOR			; is the successor NULL?
	jump	eq,(JUMPADDR)			; if so, we're done
	nop

;;; we have a node! load the rest of it and process it.
	movei	#OFFSET_SPRNODE_ln_Pred,NODE_OFFSET
	load	(NODE_OFFSET+CURRENT_NODE),PREDECESSOR  ; get predecessor

;;; populate the blit values once the blitter is available
	LOCK_BLITTER
	TAKE_BLIT_PRIORITY

	WAIT_FOR_BLITTER_LOCK
	WAIT_FOR_BLITTER_IDLE
	
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
	addq	#18,TEMP1
	movei	#_GPU_blit_sprite,TEMP2
	addq	#2,TEMP1	; interleave
	store	TEMP1,(TEMP2)   ; and this is our sprite value

	GPU_JSR	#_gpu_sprite_blit
	
	movei	#.node_loop,JUMPADDR
	jump	t,(JUMPADDR)	; and loop again
	nop
.done:	
	UNLOCK_BLITTER		; allow blitter usage again

	DUMP_GPU_REGISTERS
	
	StopGPU

	.phrase
_gpu_sprite_blit:
	GPU_REG_BANK_1
	
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

	movei	#0,TEMP1
	store	TEMP1,(R_A1_CLIP)
	
	movei	#PITCH1|PIXEL8|WID320|XADDPIX|YADD0,TEMP1
	store	TEMP1,(R_A1_FLAGS)

	movei	#OFFSET_SpriteGraphic_Size,r14
	load	(r13),r10		;fetch the pointer
	load	(r14+r10),TEMP1		;load the Size field
	move	TEMP1,r17
	move	r14,r6
	move	r10,r16
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

	GPU_RTS

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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;;  gpu_reset_bg_tile_objects
	;; reset the tiles to a height of 16 and pixeldata = shipsheet
	.phrase
_gpu_reset_bg_tile_objects::
	FINE_SCROLLS_PER_COARSE .equr	r15
	PHRASE2_DATA		.equr	r16
	
	PIXELDATA_MASK		.equr	r17
	PIXELDATA_PTR		.equr	r18

	OBJECT_INCREMENT	.equr	r19
	BG_TILE_ARRAY		.equr	r20

	YPOS_MASK		.equr	r21
	YPOS_MASK_INVERTED	.equr	r22

	PHRASE_MODIFIED		.equr	r23

	BG_SCROLL_COUNT		.equr	r24

	LOCK_BLITTER		
	TAKE_BLIT_PRIORITY

	movei	#16,FINE_SCROLLS_PER_COARSE
	movei	#$000000FF,PIXELDATA_MASK
	movei	#$00003FF8,YPOS_MASK
	movei	#$FFFFC007,YPOS_MASK_INVERTED

	movei	#_gpu_bg_fine_scroll_count,BG_SCROLL_COUNT
	movei	#_mobj_bg_tiles,BG_TILE_ARRAY
	addq	#5,BG_TILE_ARRAY
	load	(BG_TILE_ARRAY),BG_TILE_ARRAY ; dereference to get the first tile

	movei	#208,LOOPCOUNTER
	movei	#reset_pixeldata,JUMPADDR
	movei	#$44,OBJECT_INCREMENT
	movei	#_shipsheet,PIXELDATA_PTR ; shipsheet pixel data

	;; Loop through the tile objects, resetting the height to 32 units (16px) and pixeldata ptrs
	;; TODO: set up an array that points to a tile offset in the pixel array
reset_pixeldata:		
	;; first phrase
	move	PIXELDATA_PTR,TEMP1
	shlq	#8,TEMP1	      ; PIXELDATA starts at bit 11
	load	(BG_TILE_ARRAY),TEMP2
	nop
	and	PIXELDATA_MASK,TEMP2  ; mask off the old address
	or	TEMP1,TEMP2	      ; combine with the new address
	store	TEMP2,(BG_TILE_ARRAY) ; and store the pointer in the object

	;; second phrase
	addq	#4,BG_TILE_ARRAY ; point to the second phrase
	load	(BG_TILE_ARRAY),PHRASE2_DATA ; keep a fresh copy of this in register

reset_height:	
	move	PHRASE2_DATA,TEMP2	
	movei	#32,TEMP1     	; background tiles are 32 units tall
	shlq	#13,TEMP1  	; HEIGHT starts at bit 14
	or	TEMP1,TEMP2	; add height to the object data...
	move	TEMP2,PHRASE_MODIFIED ;... and store it
	store	PHRASE_MODIFIED,(BG_TILE_ARRAY)
	
scroll_ypos:
	;; If bg_fine_scroll_count == 15, reset to 0 and add (2*15) to YPOS before cycling.
	;; And move all tiles down one row. This is one coarse scroll.
	move	PHRASE_MODIFIED,TEMP2    ; grab the new phrase again
	move	TEMP2,r25
	and	YPOS_MASK,TEMP2          ; mask off everything but the height
	shrq	#3,TEMP2		 ; shift so we can use subq
	addq	#2,TEMP2		 ; move down 1 line
	shlq	#3,TEMP2		 ; move it back to the correct offset
	and	YPOS_MASK,TEMP2
	
	move	PHRASE_MODIFIED,TEMP1
	and	YPOS_MASK_INVERTED,TEMP1 ; mask off the height
	or	TEMP2,TEMP1		 ; combine with the new ypos
	move	TEMP1,r26
	store	TEMP1,(BG_TILE_ARRAY)	 ; and write the new phrase
	nop

	;; this doesn't work
coarse_scroll:
	load	(BG_SCROLL_COUNT),TEMP1
	addq	#1,TEMP1
	cmp	FINE_SCROLLS_PER_COARSE,BG_SCROLL_COUNT
	jr	eq,.do_coarse_scroll
	nop
	store	TEMP1,(BG_SCROLL_COUNT)
	movei	#loop_check,r30
	jump	t,(r30)
	nop

.do_coarse_scroll:
	;; TODO: advance all tiles by 1 row
	load	(BG_TILE_ARRAY),TEMP2    ; grab the new phrase again
	move	TEMP2,r25
	and	YPOS_MASK,TEMP2          ; mask off everything but the height
	shrq	#3,TEMP2		 ; shift so we can use subq
	subq	#32,TEMP2		 ; move up 16 lines
	shlq	#3,TEMP2		 ; move it back to the correct offset
	and	YPOS_MASK,TEMP2
	store	TEMP2,(BG_TILE_ARRAY)
	movei	#0,TEMP1
	store	TEMP1,(BG_SCROLL_COUNT)
	
loop_check:	
	sub	OBJECT_INCREMENT,BG_TILE_ARRAY ; advance to the next object
	subq	#1,LOOPCOUNTER	  
	cmpq	#0,LOOPCOUNTER	; are we at the end of the array?
	jump	ne,(JUMPADDR)	; if so, we're done
	nop

.done:
	UNLOCK_BLITTER
	StopGPU

	.long
_gpu_bg_scroll_enabled::	dc.l	0
_gpu_bg_fine_scroll_count:	dc.l	0 ; when this reaches 15, reset to 0.
	
_gpu_sprite_display_list::	dc.l	0 ; struct List *

;;; SpriteNode offset table
	OFFSET_SPRNODE_ln_Succ	.equ	0  ; struct Node *
	OFFSET_SPRNODE_ln_Pred	.equ	4  ; struct Node *
	OFFSET_SPRNODE_ln_Type	.equ	8  ; uint8
	OFFSET_SPRNODE_ln_Pri	.equ	9  ; int8
	OFFSET_SPRNODE_ln_Name	.equ	10 ; char *
	OFFSET_SPRNODE_NODESIZE	.equ	14

	;; 2 bytes of padding
	OFFSET_SPRNODE_pad1	.equ	14
	OFFSET_SPRNODE_location	.equ	16 ; struct Coordinate
	OFFSET_SPRNODE_image    .equ    20 ; struct SpriteGraphic
	OFFSET_SPRNODE_delta	.equ    48 ; struct Coordinate
	OFFSET_SPRNODE_isPlayer .equ	52 ; uint8
	OFFSET_SPRNODE_pad2	.equ	53 ; uint8
	OFFSET_SPRNODE_SIZE	.equ    54

;;;
_gpu_stack::			dcb.l	32,0	;32-position stack
_gpu_stack_end:

_gpu_register_dump::		dcb.l	32,0
	
	.68000 			;End section
_gpu_sprite_program_end::
