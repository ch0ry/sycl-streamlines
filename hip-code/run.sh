#!/bin/bash
cd code

hipcc streamlines.cu.hip -o ../streamlines \
  -O3 -ffast-math \
  -I/usr/include/hdf5/serial \
  -L/usr/lib/x86_64-linux-gnu/hdf5/serial \
  -Wl,-rpath,/usr/lib/x86_64-linux-gnu/hdf5/serial \
  -lhdf5_hl_cpp -lhdf5_cpp -lhdf5_hl -lhdf5 \
  -lcrypto -lcurl -lpthread -lsz -lz -ldl -lm

cd ..
# echo "Testing number of steps"
echo "id,nsteps,nseeds,rk4,field" >results.csv
touch valgrind.out
id=0
for ((j = 0; j < 10; j += 1)); do

  for ((i = 1000; i <= 10000; i = i + 1000)); do

    # start_ns=$(date +%s%N)
    # echo "Streamlines for $i steps"

    ./streamlines --nsteps $i --dt 0.002 --id $id >>results.csv

    # end_ns=$(date +%s%N)
    # duration_ms=$(((end_ns - start_ns) / 1000000))

    # echo "Execution time in ms: $duration_ms"
    # echo "$id,nsteps,$i,$duration_ms" >>results.csv

    valgrind --log-file="valgrind.out" ./streamlines --nsteps $i --dt 0.002 >>/dev/null
    python3 parse-valgrind.py $id

    id=$((id + 1))

  done

  for ((i = 10000; i <= 100000; i = i + 10000)); do

    # start_ns=$(date +%s%N)
    # echo "Streamlines for $i seeds"

    ./streamlines --nseeds $i --dt 0.002 --id $id >>results.csv

    # end_ns=$(date +%s%N)
    # duration_ms=$(((end_ns - start_ns) / 1000000))
    #
    # echo "Execution time in ms: $duration_ms"
    # echo "$id,nseeds,$i,$duration_ms" >>results.csv

    valgrind --log-file="valgrind.out" ./streamlines --nseeds $i --dt 0.002 >>/dev/null
    python3 parse-valgrind.py $id

    id=$((id + 1))
  done

done

sleep 5s
