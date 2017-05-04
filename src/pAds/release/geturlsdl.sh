#!/bin/bash

depth=2

./geturlsub.sh "http://search.yahoo.co.jp/search?p=アダルト" $depth >>htmladl.list

uniq htmladl.list >hug.ppp
mv hug.ppp html.list
