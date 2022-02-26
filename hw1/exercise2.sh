#!/bin/bash

echo "Enter a sequence of numbers followed by 'end'"
max=0
valid=true
i=1
re='^[0-9]+$'
while [ $valid ]
do
  read num
  if [ $num = 'end' ];
  then
    break
  fi
  if ! [[ $num =~ $re ]]; 
  then
    if [ $num != 'end' ];
    then
      echo "Please enter only integer or 'end' !!"
      break
    fi
  fi
  if [ $i -eq 1 ]
  then
    max=$num
  else
    if [ $num -gt $max ]
    then
      max=$num
    fi
  fi
  i=$((i+1))
done

echo "Maximum: $max"

