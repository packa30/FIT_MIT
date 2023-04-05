#Author : Peter Grofčík, xgrofc00
#!/bin/bash

for (( i = 2; i < 256; i=i*2 )); do
  input="measure$i.txt"
  cat $input >> measurefin.txt
done
