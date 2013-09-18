#!/bin/bash -eu
make clean
make
./test

if [[ -e "exec_info.db" ]]; then
  echo "CP#0 : FAILED"
else
  echo "CP#0 : PASSED"
fi
make clean
