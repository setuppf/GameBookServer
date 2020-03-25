#!/bin/bash
#
#

n=$1
[ -z $n ] && n=50

for name in callgrind.out.*; do
    var=${name#callgrind.out.}
    echo "gprof2dot -f callgrind -n${n} -s ${name} | dot -Grankdir=LR -Tpng -o valgrind-${var}-n${n}.png"
    gprof2dot -f callgrind -n${n} -s ${name} | dot -Grankdir=LR -Tpng -o valgrind-${var}-n${n}.png
done
