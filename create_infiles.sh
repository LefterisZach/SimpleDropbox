#!/bin/bash

DIR_NAME=$1
NUM_FILES=$2
NUM_DIRS=$3
LEVELS=$4

num_re='^[0-9]+$'
ERROR=0
if ! [[ $NUM_FILES =~ $num_re ]] ; then
   echo "error: NUM_FILES is not a number" >&2; ERROR=1
fi
if ! [[ $NUM_DIRS =~ $num_re ]] ; then
   echo "error: NUM_DIRS is not a number" >&2; ERROR=1
fi
if ! [[ $LEVELS =~ $num_re ]] ; then
   echo "error: LEVELS is not a number" >&2; ERROR=1
fi
if  [[ "$ERROR" -eq "1" ]]; then
    echo "Exiting"; exit;
fi

mkdir -p $DIR_NAME

declare -a my_array
my_array=($DIR_NAME)


val=0
while [[ "$val" -lt "$NUM_DIRS" ]] ; do

  dir2="$DIR_NAME/"
  for (( j = 1 ; j <= $LEVELS ; j++ ))
  do
    name_length=$(echo $((1 + RANDOM % 8)))
    rand_name=$(cat /dev/urandom | base64 | tr -dc 'a-zA-Z0-9~!@#$%^&*_-' | head -c $name_length)
    dir2+="$rand_name"
    my_array+=($dir2)
    mkdir $dir2
    dir2+="/"
    echo $dir2

    ((val++))

    if [[ "$val" -eq "$NUM_DIRS" ]]; then
      break
    fi
  done

  echo ""
done


check=${#my_array[@]}
echo ""

count=0
index=0

while [[ "$count" -lt "$NUM_FILES" ]] ; do
filename_length=$(echo $((1 + RANDOM % 8)))
filename=$(cat /dev/urandom | base64 | tr -dc 'a-zA-Z0-9~!@#$%^&*_-' | head -c $filename_length)
length=$(shuf -i 1000-128000 -n 1)
echo "$filename.txt"
echo "$(cat /dev/urandom | base64 | tr -dc 'a-zA-Z0-9~!@#$%^&*_-' | head -c $length)" > ${my_array[$index]}/$filename.txt
((count++))
((index++))
if [[ "$index" -eq "$check" ]]; then
index=0
fi
done


exit 0
