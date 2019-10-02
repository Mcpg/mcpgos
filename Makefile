
.PHONY: all clean kernel libc klibc diskimg

all: kernel

clean:
	$(MAKE) -C kernel clean
	BUILD_KLIBC=false $(MAKE) -C libc clean
	BUILD_KLIBC=true $(MAKE) -C libc clean

libc:
	BUILD_KLIBC=false $(MAKE) -C libc all

klibc:
	BUILD_KLIBC=true $(MAKE) -C libc all

kernel: klibc
	$(MAKE) -C kernel all

diskimg:
	$(MAKE) -C diskimg all
