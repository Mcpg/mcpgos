ENTRY (_start)

VirtualAddress = 0xE0000000;
PhysicalAddress = 0x100000;

SECTIONS
{
	. = VirtualAddress;
	KernelStart = .;
    .multiboot ALIGN(8) : AT(ADDR(.multiboot) - VirtualAddress + PhysicalAddress)
    {
        *(.multiboot)
    }
	.text ALIGN(4096) : AT(ADDR(.text) - VirtualAddress + PhysicalAddress)
	{
		*(.text)
	}
	.rodata ALIGN(4096) : AT(ADDR(.rodata) - VirtualAddress + PhysicalAddress)
	{
		*(.rodata)
	}
	.data ALIGN(4096) : AT(ADDR(.data) - VirtualAddress + PhysicalAddress)
	{
		*(.data)
	}
	.bss ALIGN(4096) : AT(ADDR(.bss) - VirtualAddress + PhysicalAddress)
	{
		*(COMMON)
		*(.bss)
	}
    . = NEXT(4096);
	KernelEnd = .;

	KernelSize = KernelEnd - KernelStart;
	KernelSizePages = KernelSize / 4096;
}