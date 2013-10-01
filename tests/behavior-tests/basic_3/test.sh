#!/bin/bash -eu
make clean
make
MPL_RPT_EXPLICIT=1
export MPL_RPT_EXPLICIT
./test

ref=`grep new *.txt`
echo $ref
if [[ $ref == "operator new(unsigned long) | 1600 | 800" ]]; then
  echo "CP#0 : PASSED"
else
  echo "CP#1 : FAILED"
fi

ref=`grep abc *.txt | wc -l`
echo $ref
if [[ $ref == "0" ]]; then
  echo "CP#1 : PASSED"
else
  echo "CP#1 : FAILED"
fi
