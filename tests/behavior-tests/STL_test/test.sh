#!/bin/bash -eu
make clean
make
./test

ref=`grep std::forward_iterator_tag *.txt | wc -l`
echo $ref
#if [[ $ref == "char* std::string::_S_construct<char const*>(char const*, char const*, std::allocator<char> const&, std::forward_iterator_tag) | 28 | 28" ]]; then
if [[ $ref == "0" ]]; then
  echo "CP#0 : PASSED"
else
  echo "CP#1 : FAILED"
fi

make clean
