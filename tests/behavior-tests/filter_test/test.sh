#!/bin/bash -eu
make clean
make
./test

ref=`grep __libc_start_main *.txt | wc -l`
echo $ref
if [[ $ref == "0" ]]; then
  echo "CP#1 : PASSED"
else
  echo "CP#1 : FAILED"
fi
