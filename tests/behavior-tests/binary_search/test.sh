#!/bin/bash -eu
make clean
make
./test > op.txt

ref=`grep output op.txt`
echo $ref
if [[ $ref == "output=18446744073709551615 5 5 10 10 15 15 20 20 25 25 30 30" ]]; then
  echo "CP#0 : PASSED"
else
  echo "CP#0 : FAILED"
fi
