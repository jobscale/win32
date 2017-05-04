#!/bin/bash

iexplore="/cygdrive/c/Program Files/Internet Explorer/iexplore.exe"

while read targ
do
  "$iexplore" $targ &
  sleep 10
  ss=`ps|grep iexplore`;for i in $ss; do kill $i; break; done
  ss=`ps|grep iexplore`;for i in $ss; do kill $i; break; done
done

