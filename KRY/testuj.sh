mkdir output_dir

for VARIABLE in 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
do
  test_name="test/test"$VARIABLE".txt"
  test_out="output_dir/test"$VARIABLE".txt"
  ref_out="out/test"$VARIABLE".out"
  ./kry < $test_name > $test_out
  paco=`cat $test_out`
  pato=`cat $ref_out`
  echo "PACO: "$paco
  echo "PATO: "$pato

done
