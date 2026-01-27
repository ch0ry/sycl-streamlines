#!/bin/bash

echo "Testing number of steps"
echo "test, amount, time_elapsed" >results.csv
for ((i = 1000; i <= 10000; i = i + 1000)); do

  start_ns=$(date +%s%N)
  echo "Streamlines for $i steps"

  ./streamlines --nsteps $i --dt 0.002 >>results.csv

  end_ns=$(date +%s%N)
  duration_ms=$(((end_ns - start_ns) / 1000000))

  echo "Execution time in ms: $duration_ms"
  echo "nsteps, $i, $duration_ms" >>results.csv

done

for ((i = 10000; i <= 100000; i = i + 10000)); do

  start_ns=$(date +%s%N)
  echo "Streamlines for $i seeds"

  ./streamlines --nseeds $i --dt 0.002 >>results.csv

  end_ns=$(date +%s%N)
  duration_ms=$(((end_ns - start_ns) / 1000000))

  echo "Execution time in ms: $duration_ms"
  echo "nseeds, $i, $duration_ms" >>results.csv

done

sleep 5s
