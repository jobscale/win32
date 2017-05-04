#!/bin/bash

while read targ
do
  echo -n $targ
  RET=`wget $targ -O- 2>&1 |grep pad.plustar.jp |while read line; do expr "$line" : '.*plustar-anal?\([^ "]*\)'; done`
  echo $RET
  #if [ $RET = "" ]
  #then
  #  echo "?a=0&b=0&t=0&c=0.000"
  #fi
done

