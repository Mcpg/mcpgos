#!/bin/sh
./todos.sh
qemu-system-i386 -s -drive media=disk,file=diskimg/mcpgos.iso,format=raw \
   -d int
