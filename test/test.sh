#!/bin/zsh
# rm *.svg *.dot

# IR_FILE=../test/simple/t1.ll
PROGNAME=wc
BASEDIR=$HOME/Programs/cxx/coreutils-5.0/src/
export CHOPPER_JSON=$PWD/test.json
# export CHOPPER_DOT=1
# opt-3.2 -load ./PDGPass.so -postdomtree -basicaa -globalsmodref-aa -scev-aa -aa-eval -memdep -pdg < ../test/simple/t1.ll > result.bc
for IR_FILE in `ls $BASEDIR | grep -e "\.ll$"`
do
PROGNAME=`echo $IR_FILE | sed "s/\.ll$//"`
opt-3.2 -S -load ../src/PDGPass.so -postdomtree -basicaa \
  -globalsmodref-aa -scev-aa -aa-eval -memdep -pdg < $BASEDIR/$IR_FILE > \
  result.ll 2> $PROGNAME.result

cat $PROGNAME.result | sed '/^====/d' | sed '/^\s/d' \
  | node analysisJSON.js > $PROGNAME.tsv

cat general.plt | sed "s/PROGNAME/$PROGNAME/g" | gnuplot
done

# opt-3.2 -load ./PDGPass.so -postdomtree -scev-aa -memdep -pdg < ../test/cksum/cksum.ll > result.bc
#for file in `ls *.dot`
#do
#dot -Tsvg $file > $file.svg
#done

