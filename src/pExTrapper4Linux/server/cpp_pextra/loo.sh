#!/bin/bash


count=1
while [ $count -le 50 ];
do
echo "$count"
count=`expr $count + 1`
./bcast &
done

