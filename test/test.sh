#!/bin/bash

WPA=../Release-build/bin/wpa
SOURCE=test.c
BITCODE=${SOURCE/.c/.bc}
PTRANALYSIS=-ander

rm -f $BITCODE
clang $SOURCE -g -emit-llvm -c -o $BITCODE
$WPA $PTRANALYSIS -dump-callgraph $BITCODE
$WPA $PTRANALYSIS -dump-pag $BITCODE
$WPA $PTRANALYSIS -dump-consG $BITCODE
$WPA $PTRANALYSIS -svfg -dump-svfg $BITCODE
$WPA $PTRANALYSIS -svfg -dump-mssa $BITCODE > mssa.txt
$WPA $PTRANALYSIS -print-pts $BITCODE
for i in *.dot; do
	dot -Tpng $i -o ${i/.dot/.png}
	rm -f $i
done
