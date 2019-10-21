#!/bin/sh

source_search() {
    grep $1 -i -r -n -s --color=always $2
}

echo "\n-------------- TODO List --"
source_search "TODO" kernel
source_search "TODO" libc
echo "---------------------------"

#echo "\n------------ FIXME List ---"
#source_search "FIXME" kernel
#source_search "FIXME" libc
#echo "---------------------------\n"
