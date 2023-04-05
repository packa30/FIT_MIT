#Author : Peter Grofčík, xgrofc00
#!/bin/bash

if [ $# -lt 1 ];then
    echo "Missing number of elements. Please specify first arg (./ots [number])"
    exit 1
else
  #preklad cpp zdrojaku
  mpic++ --prefix /usr/local/share/OpenMPI -o ots ots.cpp

  #vyrobenie suboru s random cislami spustenie a odstránenie -> else
  #v pripade prveho vstupneho argumentu rovneho EXP dochazka k vytvoreniu dat z merani 250*27 merani času radenia
  if [ $1 = 'EXP' ]; then
    i=1
    while [ $i -lt 28 ]; do
      k=0
      while [ $k -lt 250 ]; do
        dd if=/dev/random bs=1 count=$i of=numbers status=none
        mpirun --prefix /usr/local/share/OpenMPI -np $i ots 1
        rm -f numbers
        ((k++))
      done
      ((i++))
    done
    rm -f ots
  else
    numbers=$1;
    dd if=/dev/random bs=1 count=$numbers of=numbers status=none
    mpirun --prefix /usr/local/share/OpenMPI -np $numbers ots
    rm -f ots numbers
  fi;

fi;
