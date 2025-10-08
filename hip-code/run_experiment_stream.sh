#!/bin/bash -x
# This script aims to provide a way to launch a batch of calls to cuda-streamlines in order to compare sets of results for streamline calculation using streams against the implementaion that doesn't use them
# We need to vary the number of steps and the number of seeds, and perhaps the size of the buffer (that part isn't implemented yet as of the 25th January 2018)
# Fisrt, we need to define a base directory, and an executable name
ExpName=streamlines_streams
HomePath='/home/ch0ry/projects/cuda-streamlines_stream/'
NameExec='streamlines_stream'
# Note: in this specific case we need a extra part of the path, this is because of the way the program is built
ExtraPath='build/'
LogPath='logs/'
# next we define the number of steps, seeds, and the value of the time interval, but, since we need to vary them, we need a cycle, so we use an array
SeedsValuesArray=(500 1000 5000)
#SeedsValuesArray=(100 500 1000 5000 10000 50000 100000)
#StepsValuesArray=(100)
StepsValuesArray=(100 500 1000 5000 10000 50000 100000)
DtValuesArray=(0.002)
BufferDepth=(1)
# this is a variable to control vtp output
VTP_output=0
# number of repetitions
Repetitions=10
# The first cycle will take care of the variation in seed number
for seed in ${SeedsValuesArray[*]}; do
  #echo item: $seed
  # The second loop will vary the number of steps
  for step in ${StepsValuesArray[*]}; do
    #echo item $step
    #The third loop will deal with dt DtValues
    for dt in ${DtValuesArray[*]}; do
      #echo item: $dt
      #The fourth and last (for the time being) will define the different buffer sizes
      for bdepth in ${BufferDepth[*]}; do
        count=0
        while [ $count -lt $Repetitions ]; do
          #echo item $bdepth
          #here we create the command lines that will launch the different jobs, and also define the names of the files being generated
          #First, the command line, for the raw time
          Output_FILE=$HomePath$LogPath$ExpName"_Seeds"$seed"_steps"$step"_dt"$dt"_bdepth"$bdepth"_"$count
          echo $Output_FILE
          command1="(time "$HomePath$ExtraPath$NameExec" -s "$seed" -n "$step" -t "$dt" -v "$VTP_output" -b "$bdepth" &> "$Output_FILE") &>> "$Output_FILE
          command3="nvprof "$HomePath$ExtraPath$NameExec" -s "$seed" -n "$step" -t "$dt" -v "$VTP_output" -b "$bdepth" &> "$Output_FILE".txt"
          eval $command1
          eval $command3
          let count=count+1
        done
        Output_FILE=$HomePath$LogPath$ExpName"_Seeds"$seed"_steps"$step"_dt"$dt"_bdepth"$bdepth
        command2="nvprof -f --export-profile "$Output_FILE".nvvp "$HomePath$ExtraPath$NameExec" -s "$seed" -n "$step" -t "$dt" -v "$VTP_output" -b "$bdepth
        eval $command2
      done
    done
  done
done
echo "Total"$count
