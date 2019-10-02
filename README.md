# McpgOS
Experimental Unix-like x86-32 operating system with a dirty codebase, written in
C++.

## Building
You need:
 * a Unix-y environment (WSL is good if you're on Win10, Cygwin might be a good
   pick on other versions, didn't test it)
 * nasm
 * GNU make or something compatible
 * i686-elf toolchain
 * GRUB packages for generating the disk image

Now run `make [whatever module you want]` where the module can be:
 * all - everything except for the disk image
 * diskimg - the ISO
 * kernel - the amazing kernel itself
 * libc - libc to link with userspace software
 * klibc - libc to link with the kernel

Now hope I didn't push something uncompilable :-)
