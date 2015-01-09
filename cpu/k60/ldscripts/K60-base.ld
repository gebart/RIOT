/* RAM limits */
__sram_u_start  = ORIGIN(sram_u);
__sram_u_length = LENGTH(sram_u);
__sram_u_end    = __sram_u_start + __sram_u_length;
__sram_l_start  = ORIGIN(sram_l);
__sram_l_length = LENGTH(sram_l);
__sram_l_end    = __sram_l_start + __sram_l_length;

/* Define the default stack size for interrupt mode. As no context is
   saved on this stack and ISRs are supposed to be short, it can be fairly
   small. 512 byte should be a safe assumption here */
STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : 0x200; /* 512 byte */

SECTIONS
{
    /* Start at address 0x00. */
    . = 0x00;

    /* Interrupt vectors 0x00-0x3ff. */
    .vector_table :
    {
        _vector_rom = .;
        KEEP(*(.isr_vector))
        KEEP(*(.vector_table))
    } > vectors
    /* Actually only 111 entries are used by the K60DNxxxVLL10. (111*4 == 444 == 0x1bc) */
    ASSERT (SIZEOF(.vector_table) == 444, "Interrupt vector table of invalid size.")
    ASSERT (. == 0x1bc, "Interrupt vector table at invalid location.")

    /*
     * Allocate space for interrupt vector in RAM
     * This can safely be removed to free up 0x400 bytes of RAM if the code does
     * not make use of this CPU feature.
     */
    .ramvect :
    {
        . = ALIGN(1024);
        _vector_ram_start = .;
        . = _vector_ram_start + 0x400;
        _vector_ram_end = .;
    } > sram_u


    /* Flash configuration field, very important in order to not accidentally lock the device */
    /* Flash configuration field 0x400-0x40f. */
    .fcfield :
    {
        /* Backdoor key. */
        LONG (0x01234567) LONG (0x89abcdef)

        /* FPROT0-3: no region of the flash is protected. */
        LONG (0xffffffff)

        /* FSEC: place MCU in unsecure mode. */
        BYTE (0xfe)

        /* FOPT: disable EzPort. */
        BYTE (0xfd)

        /* FEPROT: default. */
        BYTE (0xff)

        /* FDPROT: default. */
        BYTE (0xff)
    } > flashsec
    ASSERT (SIZEOF(.fcfield) == 0x10, "Flash configuration field of invalid size (linker-script error?).")
    ASSERT (. == 0x410, "Flash configuration field of invalid size or position (linker-script error?).")

    /* Program code 0x410-. */
    .text :
    {
        . = ALIGN(8);
        _text_load = LOADADDR(.text);
        _text_start = .;
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(SORT(.preinit_array.*)))
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(8);

        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(8);

        /* fini data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(8);

        KEEP (*(SORT_NONE(.init)))
        KEEP (*(SORT_NONE(.fini)))
        /* Default ISR handlers */
        KEEP(*(.default_handlers))
        *(.text.unlikely .text.*_unlikely .text.unlikely.*)
        *(.text.exit .text.exit.*)
        *(.text.startup .text.startup.*)
        *(.text.hot .text.hot.*)
        *(.text .stub .text.* .gnu.linkonce.t.*)

        /* gcc uses crtbegin.o to find the start of
           the constructors, so we make sure it is
           first.  Because this is a wildcard, it
           doesn't matter if the user does not
           actually link against crtbegin.o; the
           linker won't look for a file to match a
           wildcard.  The wildcard also means that it
           doesn't matter which directory crtbegin.o
           is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        KEEP (*crtbeginTS.o(.ctors))
        /* We don't want to include the .ctor section from
           the crtend.o file until after the sorted ctors.
           The .ctor section from the crtend file contains the
           end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))

        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*crtbeginTS.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        . = ALIGN(8);
        _rodata_start = .;
        *(.rodata .rodata* .gnu.linkonce.r.*)
        . = ALIGN(8);
        _rodata_end = .;
        _text_end = .;
    } > flash

    .ramcode :
    {
        . = ALIGN(8);
        _ramcode_load = LOADADDR(.ramcode);
        _ramcode_start = .;
        *(.ramcode*)
        . = ALIGN(8);
        _ramcode_end = .;
    } > sram_l AT>flash

    /* The .extab, .exidx sections are used for C++ exception handling */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > flash

    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > flash
    PROVIDE_HIDDEN (__exidx_end = .);

    .eh_frame_hdr : { *(.eh_frame_hdr) } > flash
    .eh_frame       : ONLY_IF_RO { KEEP (*(.eh_frame)) } > flash
    .gcc_except_table   : ONLY_IF_RO { *(.gcc_except_table .gcc_except_table.*) } > flash

/*
    .eh_frame       : ONLY_IF_RW { KEEP (*(.eh_frame)) } > sram_u AT > flash
    .gcc_except_table   : ONLY_IF_RW { *(.gcc_except_table .gcc_except_table.*) } > sram_u AT > flash
*/
    /* Program data, values stored in flash and loaded upon init. */
    .data :
    {
        . = ALIGN(8);
        _data_load  = LOADADDR(.data);
        _data_start = .;

        *(.data*)
        . = ALIGN(8);

        _data_end = .;
        /* edata is used instead of _data_end by some RTOSes startup code (e.g. ChibiOS) */
        PROVIDE(edata = .);
    } > sram_u AT >flash

    /* Program bss, zeroed out during init. */
    .bss (NOLOAD):
    {
        . = ALIGN(8);
        _bss_start = .;
        __bss_start = .;
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        _bss_end = .;
        __bss_end = .;
        __bss_end__ = .;
        . = ALIGN(8);
    } > sram_u

    /* Make sure we set _end, in case we want dynamic memory management... */
    _end = .;
    __end = .;
    __end__ = .;
    PROVIDE(end = .);

    HEAP_SIZE = ORIGIN(sram_u) + LENGTH(sram_u) - STACK_SIZE - .;

    .heap (NOLOAD):
    {
        . = ALIGN(8);
        _heap_start = .;
        __heap_start = .;
        . = . + HEAP_SIZE;
        . = ALIGN(8);
        _heap_end = .;
    } > sram_u

    /* stack section */
    .stack (NOLOAD):
    {
        . = ALIGN(8);
        _sstack = .;
        . = . + STACK_SIZE;
        . = ALIGN(8);
        _estack = .;
    } > sram_u

    /* Any debugging sections */
    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end ) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    /* DWARF 3 */
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    .debug_ranges   0 : { *(.debug_ranges) }
    /* DWARF Extension.  */
    .debug_macro    0 : { *(.debug_macro) }

    /* XXX: what is the purpose of these sections? */
    .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) KEEP (*(.gnu.attributes)) }
    .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
    /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) }
}
