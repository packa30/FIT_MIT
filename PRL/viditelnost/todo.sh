#Author : Peter Grofčík, xgrofc00
#!/bin/bash

for (( h = 2; h <= 256; h=h*2 )); do
  input="measure$h.txt"
  k=0
  while IFS= read -r line
  do
    my_array[$k]=$line;
    ((k++))
  done < "$input"
  k=0;
  max=0;
  index=0;
  for (( l = 0; l < 3; l++ )); do
    for (( i = 0; i < 13-l; i++ )); do
      if [ $max -lt ${my_array[$i]} ]; then
        max=${my_array[$i]}
        index=$i
      fi
    done
    o=0;
    for (( i = 0; i < 13-l; i++ )); do
      if [ $index -ne $i ]; then
        my_array[$o]=${my_array[$i]}
        ((o++))
      fi
    done
  done
  for (( i = 0; i < 10; i++ )); do
    count=$(($count+${my_array[$i]}))
  done
  count=$(($count/10))
  echo $count
  my_array[$k]=$line;
  ((k++))
done
