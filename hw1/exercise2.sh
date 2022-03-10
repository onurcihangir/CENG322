#!/bin/bash

echo "Enter a sequence of numbers followed by 'end'"
MAX=0
VALID=true
i=1
RE='^[0-9]+$'
# infinite loop until break
while [ $VALID ]
do
  # take input
  read NUM
  # check if input is 'end'
  # if it is, break loop
  if [ $NUM = 'end' ];
  then
    break
  # check if input is integer
  # if it is not, break loop
  elif ! [[ $NUM =~ $RE ]]; 
  then
    echo "Please enter only integer or 'end' !!"
    break
  fi
  # if the first input is entered
  # then it is max number
  if [ $i -eq 1 ]
  then
    MAX=$NUM
  else
    # check if input is greater 
    # than max number
    # if it is, assign to max
    if [ $NUM -gt $MAX ]
    then
      MAX=$NUM
    fi
  fi
  # increment input number
  i=$((i+1))
done

echo "Maximum: $MAX"

