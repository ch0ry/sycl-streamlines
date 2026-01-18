#!/bin/bash

echo "Testing number of steps"
echo "test, amount, time_elapsed" >resultsHIP.csv
for ((i = 1000; i <= 10000; i = i + 1000)); do

  start_ns=$(date +%s%N)
  echo "Streamlines for $i steps"

  ~/sycl-streamlines/hip-code/build/streamlines_hip --nsteps $i --dt 0.002

  end_ns=$(date +%s%N)
  duration_ms=$(((end_ns - start_ns) / 1000000))

  echo "Execution time in ms: $duration_ms"
  echo "nsteps, $i, $duration_ms" >>resultsHIP.csv

done

for ((i = 10000; i <= 100000; i = i + 10000)); do

  start_ns=$(date +%s%N)
  echo "Streamlines for $i seeds"

  ~/sycl-streamlines/hip-code/build/streamlines_hip --nseeds $i --dt 0.002

  end_ns=$(date +%s%N)
  duration_ms=$(((end_ns - start_ns) / 1000000))

  echo "Execution time in ms: $duration_ms"
  echo "nseeds, $i, $duration_ms" >>resultsHIP.csv

done
