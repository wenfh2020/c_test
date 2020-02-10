#!/bin/sh

echo $1

align=1
unalign=0
array=(1, 2, 4, 8, 16, 32, 64)

for x in ${array[*]}
do
    gcc -g -O0 align.cpp -o align  && time ./align $x $align
    echo '-------'
    gcc -g -O0 align.cpp -o align  && time ./align $x $unalign
    echo '>>>>>>>'
done