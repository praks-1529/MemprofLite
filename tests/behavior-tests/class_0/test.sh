#!/bin/bash -eu
make clean
make
./test

ref=`grep main *.txt`
echo $ref
if [[ $ref == "main | 44 | 44" ]]; then
  echo "CP#1 : PASSED"
else
  echo "CP#1 : FAILED"
fi
