#!/bin/bash -eu
make clean
make
./test
ref=`grep recur *.txt`
echo $ref
if [[ $ref == "recur(int) | 400 | 400" ]]; then
  echo "PASSED"
else
  echo "FAILED"
fi

make clean
