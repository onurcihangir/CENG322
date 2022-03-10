#!/bin/bash
# take file name as argument
FILENAME=$1
# read line by line
while read LINE;
do
  # prompt as many * as the number in the line
  # without any spaces between them  
  for (( i = 0; i<$LINE; ++i )); do
    echo -n "*"
  done
  # new line
  echo
done < $FILENAME
