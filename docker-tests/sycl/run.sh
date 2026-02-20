#!/bin/bash

echo "Testing number of steps"
echo "test, amount, time_elapsed" >results.csv
id=0
for ((j = 0; j < 10; j += 1)); do

  for ((i = 1000; i <= 10000; i = i + 1000)); do

    start_ns=$(date +%s%N)
    echo "Streamlines for $i steps"

    ./streamlines --nsteps $i --dt 0.002 >>results.csv

    end_ns=$(date +%s%N)
    duration_ms=$(((end_ns - start_ns) / 1000000))

    echo "Execution time in ms: $duration_ms"
    echo "$id,nsteps,$i,$duration_ms" >>results.csv

    valgrind --log-file="valgrind.out" ./streamlines --nsteps $i --dt 0.002 >>/dev/null
    python3 parse-valgrind.py $id

    id=$((id + 1))

  done

  for ((i = 10000; i <= 100000; i = i + 10000)); do

    start_ns=$(date +%s%N)
    echo "Streamlines for $i seeds"

    ./streamlines --nseeds $i --dt 0.002 >>results.csv

    end_ns=$(date +%s%N)
    duration_ms=$(((end_ns - start_ns) / 1000000))

    echo "Execution time in ms: $duration_ms"
    echo "$id,nseeds,$i,$duration_ms" >>results.csv

    valgrind --log-file="valgrind.out" ./streamlines --nseeds $i --dt 0.002 >>/dev/null
    python3 parse-valgrind.py $id

    id=$((id + 1))
  done

done

sleep 5s
