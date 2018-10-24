#!/bin/bash

WPA=../Release-build/bin/wpa
SOURCE=test.cpp
BITCODE=${SOURCE/.cpp/.bc}
PTRANALYSIS=-fspta

rm -f $BITCODE
clang++ $SOURCE -g -emit-llvm -c -o $BITCODE
$WPA $PTRANALYSIS -dump-callgraph $BITCODE
$WPA $PTRANALYSIS -dump-pag $BITCODE
$WPA $PTRANALYSIS -dump-consG $BITCODE
$WPA $PTRANALYSIS -svfg -dump-svfg $BITCODE
$WPA $PTRANALYSIS -svfg -dump-mssa $BITCODE > mssa.txt
$WPA $PTRANALYSIS -print-pts $BITCODE
