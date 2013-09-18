#!/bin/bash -eu
make clean
make
./test

ref=`grep __libc_start_main *.txt`
echo $ref
if [[ $ref == "__libc_start_main | 44 | 44" ]]; then
  echo "CP#1 : PASSED"
else
  echo "CP#1 : FAILED"
fi
