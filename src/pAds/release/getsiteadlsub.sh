#!/bin/bash

while read targ
do
  echo $targ
  RET=`wget $targ -O- 2>&1 |grep pad.plustar.jp |while read line; do expr "$line" : '.*plustar-anal\(?[^ "]*\)'; done`
  echo $RET
done

