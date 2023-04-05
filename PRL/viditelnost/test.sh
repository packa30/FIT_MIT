#Author : Peter Grofčík, xgrofc00
#!/bin/bash
function log2 {
    local x=0
    for (( y=$1-1 ; $y > 0; y >>= 1 )) ; do
        let x=$x+1
    done
    echo $x
}

if [ $# -lt 1 ];then
    echo "Missing number of elements. Please specify first arg (./ots [number])"
    exit 1
else
  #v pripade prveho vstupneho argumentu rovneho EXP dochazka k vytvoreniu dat z merani
  if [ $1 = 'EXP' ]; then
    #preklad cpp zdrojaku
    mpic++ --prefix /usr/local/share/OpenMPI -DMEASURE=1 -o vid vid.cpp
    my_array[0]=$((100 + RANDOM % 500))
    counter=2
    for (( i = 1; i < 32; i++ )); do
      my_array[$i]=$((100 + RANDOM % 500))
      log2in=$(log2 $i+1)
      log2in=$(((2**$log2in)/2))
      if [[ counter -eq 2 ]]; then
        for (( k = 0; k < 100; k++ )); do
          mpirun --prefix /usr/local/share/OpenMPI -np $log2in vid ${my_array[@]} >> measure$(($i+1)).txt
        done
        counter=0
      fi
      ((counter++))
    done
    rm -f vid
  else
    #preklad cpp zdrojaku
    mpic++ --prefix /usr/local/share/OpenMPI -o vid vid.cpp
    k=0;
    IFS=',' read -ra ADDR <<< $1
    for i in "${ADDR[@]}"; do
      my_array[$k]=$i
      ((k++))
    done
    if [ $k -lt 2 ]; then
      echo "Please specify atleast 2 numbers or there is nothing to do really"
    elif [ $k -gt 32 ]; then
      echo "Due to merlin maxin 25 proc. and this implementation not covering sequencial alg. part for 1 proc this can only work with 32 numbers (16 proc max)"
    else
      log2in=$(log2 $k)
      log2in=$(((2**$log2in)/2))
      mpirun --prefix /usr/local/share/OpenMPI -np $log2in vid ${my_array[@]}
    fi
    rm -f vid
  fi;
fi;
