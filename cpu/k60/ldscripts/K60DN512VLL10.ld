OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

MEMORY
{
    vectors (rx)   : ORIGIN = 0x0,        LENGTH = 0x400
    flashsec (rx)  : ORIGIN = 0x400,      LENGTH = 0x10
    flash (rx)     : ORIGIN = 0x410,      LENGTH = 512K - 0x410
    sram_l (rwx)   : ORIGIN = 0x20000000 - 64K, LENGTH = 64K /* Only accessible via code bus. */
    sram_u (rwx)   : ORIGIN = 0x20000000, LENGTH = 64K /* Only accessible via system bus. */
}

/* RAM limits */
__sram_u_start  = ORIGIN(sram_u);
__sram_u_length = LENGTH(sram_u);
__sram_u_end    = __sram_u_start + __sram_u_length;
__sram_l_start  = ORIGIN(sram_l);
__sram_l_length = LENGTH(sram_l);
__sram_l_end    = __sram_l_start + __sram_l_length;

SECTIONS
{
    /* Start at address 0x00. */
    . = 0x00;

    /* Interrupt vectors */
    INCLUDE vector-table.ld

    /* Flash security section, very important in order to not accidentally lock the device */
    INCLUDE flash-security.ld

    /* .text and .ramcode sections */
    INCLUDE text.ld

    /* .data and .bss sections */
    INCLUDE data.ld

    /* Any debugging sections */
    INCLUDE debugging-sections.ld

    /* XXX: what is the purpose of these sections? */
    .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) KEEP (*(.gnu.attributes)) }
    .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
    /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) }
}
