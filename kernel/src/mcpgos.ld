ENTRY (_start)

_VirtualAddress = 0xE0000000;
_PhysicalAddress = 0x100000;

SECTIONS
{
	. = _VirtualAddress;
	_KernelStart = .;
    .multiboot ALIGN(8) : AT(ADDR(.multiboot) - _VirtualAddress + _PhysicalAddress)
    {
        *(.multiboot)
    }
	.text ALIGN(4096) : AT(ADDR(.text) - _VirtualAddress + _PhysicalAddress)
	{
		*(.text)
	}
	.rodata ALIGN(4096) : AT(ADDR(.rodata) - _VirtualAddress + _PhysicalAddress)
	{
		*(.rodata)
	}
	.data ALIGN(4096) : AT(ADDR(.data) - _VirtualAddress + _PhysicalAddress)
	{
		*(.data)
	}
	.bss ALIGN(4096) : AT(ADDR(.bss) - _VirtualAddress + _PhysicalAddress)
	{
		*(COMMON)
		*(.bss)
	}
    . = NEXT(4096);
	_KernelEnd = .;

	_KernelSize = _KernelEnd - _KernelStart;
	_KernelSizePages = _KernelSize / 4096;
}