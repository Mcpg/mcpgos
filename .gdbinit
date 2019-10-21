
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

# Break on IdtGlobalIntHandler and its return
#b IdtGlobalIntHandler
#b Idt.c:34

define SchedDump
    echo ::: Current process\n
    print *CurrIteratedProc
    print *CurrIteratedProc->Process

    echo ::: Current thread\n
    print *CurrIteratedThread
    print *CurrIteratedThread->Thread
end

define SchedFrameDump
    SchedDump
    echo ::: Current IDT frame\n
    print *idtFrame
end
