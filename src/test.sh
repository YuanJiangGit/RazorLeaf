#!/bin/zsh
rm *.svg *.dot

# opt-3.2 -load ./PDGPass.so -postdomtree -scev-aa -memdep -pdg < ../test/simple/t1.ll > result.bc
opt-3.2 -load ./PDGPass.so -postdomtree -scev-aa -memdep -pdg < ../test/cksum/cksum.ll > result.bc
for file in `ls *.dot`
do
dot -Tsvg $file > $file.svg
done

