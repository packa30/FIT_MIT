#!/bin/bash
#  Project : Graph Sceleton
#  Course	: Functional and Logic Programming
#  Author  : Peter Grofcik
#  Login   : xgrofc00
#  Year    : 2020
# Time measurement script ./test.sh [measure]
# without measure creates output files in tests folder
# with measure deletes those files afterwards and outputs time cost for each test
make
for (( i = 0; i < 8; i++ )); do
  START=$(date +%s.%N)
  ./flp20-log < tests/test$i.in > tests/test$i.out
  END=$(date +%s.%N)
  DIFF=$(echo "$END - $START" | bc)
  if [ $# -gt 0 ];then
    if [ $1 = 'measure' ]; then
      echo "Test $i running time ~ $DIFF [s]"
    fi
  fi
done
if [ $# -gt 0 ];then
  if [ $1 = 'measure' ]; then
    make clean
  fi
fi
