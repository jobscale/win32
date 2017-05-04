#!/bin/bash

depth=2

./geturlsub.sh http://news.google.co.jp/ $depth >>html.list

uniq html.list >hhh.hhh
mv hhh.hhh html.list
