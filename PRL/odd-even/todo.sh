#!/bin/bash

(./test.sh EXP)
i=1
while [ $i -lt 28 ]; do
cat measure$i.txt
((i++))
done
