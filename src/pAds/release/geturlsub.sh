#!/bin/bash

if [ "$1" = "" ]; then exit; fi

cou=$(expr ${2:-0})
if [ $cou -lt 0 ]; then exit; fi

cou=$(expr ${cou:-0} - 1)

wget -O- "$1" 2>/dev/null|grep http://|while read line
do
expr "$line" : '.*"\(http://[^ "]*\)'|grep -v .gif$|grep -v .jpg$|grep -v .ico$|grep -v .png$|grep -v .js$|grep -v .css$|while read nex
do

if [ "$nex" = "" ]; then break; fi
if [ "$nex" = "http://www." ]; then break; fi
if [ "$(expr $nex : '.*\(plustar.jp\)')" != "" ]; then break; fi
if [ "$(expr $nex : '.*\(w3.org\)')" != "" ]; then break; fi

echo "$nex"
if [ $cou -lt 1 ]; then break; fi
$0 "$nex" "$cou"

done
done

