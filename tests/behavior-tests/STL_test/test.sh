#!/bin/bash -eu
make clean
make
./test

ref=`grep std::forward_iterator_tag *.txt`
echo $ref
if [[ $ref == "char* std::string::_S_construct<char const*>(char const*, char const*, std::allocator<char> const&, std::forward_iterator_tag) | 28 | 28" ]]; then
  echo "CP#0 : PASSED"
else
  echo "CP#1 : FAILED"
fi

ref=`grep __libc_start_main *.txt`
echo $ref
if [[ $ref == "__libc_start_main | 28 | 28" ]]; then
  echo "CP#2 : PASSED"
else
  echo "CP#2 : FAILED"
fi

make clean
