SECTIONS {
    . = 0x4000;
    .text: {
      *(.text CODE text)
    }
	
    . = ALIGN(32);
    .data: {
      VBCC_CONSTRUCTORS
      *(.data DATA data)
    }
    . = ALIGN(32);
    .bss: {
      *(.bss BSS bss)
      *(COMMON)
      _BSS_END = ALIGN(32);
    }
	
	. = ALIGN(32);
	.gpucode 0xF03000: AT ( ADDR(.text) + SIZEOF(.text) )
	{
	  *(.gpucode CODE gpucode)
	}
	
	. = ALIGN(32);
	.dspcode 0xF1B000: AT ( ADDR(.dspcode) + SIZEOF(.dspcode) )
	{
	  *(.dspcode CODE dspcode)
	}
  };