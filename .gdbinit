
dir kernel
dir libc
file kernel/mcpgos.kernel
target remote localhost:1234

# Auto break on PF
b IDT.c:13

# and any other exception
b IDT.c:22

# Auto break on return from MmVmAllocate()
#b VirtualMemory.c:55

# Auto break on return from MmPmAllocate()
#b PhysicalMemory.c:53
