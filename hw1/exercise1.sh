#!/bin/bash
filename=$1
while read line;
do
  for (( i = 0; i<$line; ++i )); do
  echo -n "*"
  done
  echo
done < $filename
