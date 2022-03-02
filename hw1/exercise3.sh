#!/bin/bash
DIR=""
#check if there is an argument
if [ $# -eq 0 ]
#if there is not, then working 
#directory is assigned to variable
then
  DIR=$(pwd)
#if there is an argument
else
  #check if argument is valid
  if [ ! -d $1 ]
  #if not valid
  then
    echo "Error: '$1' not found."
    exit 1
  else
  #if valid, assign to variable
    DIR=$1
  fi
fi

REMOVED=0

for FILE in $( ls $DIR ) #iterate all files in directory
do
  if [ -f $DIR/$FILE ] && [ ! -s $DIR/$FILE ] #if file exists and is empty
  then
    rm -rf $DIR/$FILE #remove the file
    REMOVED=$((REMOVED+1))
  fi
done

echo "$REMOVED zero-length files are removed from the directory: $DIR"
