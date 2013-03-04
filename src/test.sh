#!/bin/zsh
rm *.svg *.dot

# IR_FILE=../test/simple/t1.ll
IR_FILE=./test.ll
export CHOPPER_JSON=$PWD/test.json
export CHOPPER_DOT=1
# opt-3.2 -load ./PDGPass.so -postdomtree -basicaa -globalsmodref-aa -scev-aa -aa-eval -memdep -pdg < ../test/simple/t1.ll > result.bc
opt-3.2 -S -load ./PDGPass.so -postdomtree -basicaa -globalsmodref-aa -scev-aa -aa-eval -memdep -pdg < $IR_FILE > result.ll
# opt-3.2 -load ./PDGPass.so -postdomtree -scev-aa -memdep -pdg < ../test/cksum/cksum.ll > result.bc
for file in `ls *.dot`
do
dot -Tsvg $file > $file.svg
done

