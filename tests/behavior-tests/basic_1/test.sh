#!/bin/bash -eu
make clean
make
./test

ref=`grep def *.txt`
echo $ref
if [[ $ref == "def() | 400 | 400" ]]; then
  echo "CP#0 : PASSED"
else
  echo "CP#1 : FAILED"
fi

ref=`grep abc *.txt`
echo $ref
if [[ $ref == "abc() | 400 | 400" ]]; then
  echo "CP#1 : PASSED"
else
  echo "CP#1 : FAILED"
fi

ref=`grep __libc_start_main *.txt`
echo $ref
if [[ $ref == "__libc_start_main | 400 | 400" ]]; then
  echo "CP#2 : PASSED"
else
  echo "CP#2 : FAILED"
fi
