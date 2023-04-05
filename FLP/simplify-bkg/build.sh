#!/bin/bash

make
i=1
GREEN='\033[0;32m'
RED='\033[0;31m'
NOCOLOR='\033[0m'
while [ $i -lt 12 ]; do
  ./simplify-bkg -1 tests/in/test$i.in > tests/out-actual/test$i-1.out
  ./simplify-bkg -2 tests/in/test$i.in > tests/out-actual/test$i-2.out

  cmp -s tests/out-expect/test$i-1.out tests/out-actual/test$i-1.out
  first=$(echo $?)
  cmp -s tests/out-expect/test$i-2.out tests/out-actual/test$i-2.out
  second=$(echo $?)

  if [ $first = 1 ] || [ $second = 1 ]; then
    echo -e "Test ${RED}$i ${NOCOLOR}failed."
  else
    echo -e "Test ${GREEN}$i ${NOCOLOR}is alright."
  fi
  ((i++))
done
if [ $# -eq 1  ];then
  if [ $1 = '-q' ]; then
    rm tests/out-actual/test*.out
  fi
fi
