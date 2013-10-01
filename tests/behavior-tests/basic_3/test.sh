#!/bin/bash -eu
make clean
make
MPL_RPT_EXPLICIT=1
export MPL_RPT_EXPLICIT
./test

ref=`grep def *.txt`
echo $ref
if [[ $ref == "def() | 800 | 0" ]]; then
  echo "CP#0 : PASSED"
else
  echo "CP#1 : FAILED"
fi

ref=`grep main *.txt`
echo $ref
if [[ $ref == "main | 400 | 400" ]]; then
  echo "CP#1 : PASSED"
else
  echo "CP#1 : FAILED"
fi

ref=`grep abc *.txt | wc -l`
echo $ref
if [[ $ref == "0" ]]; then
  echo "CP#2 : PASSED"
else
  echo "CP#2 : FAILED"
fi
