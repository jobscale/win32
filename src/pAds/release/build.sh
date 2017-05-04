#!/bin/bash

sername="sv0.plustar.jp"
upload="/var/www/plustar/dl/pads/"
release="/var/www/plustar/dl/pads/modtest/"

yuicompressor="../../../../util/soft/yuicompressor/yuicompressor-2.4.6.jar"
templates_dir="../current/templates"

usage="usage ./build.sh [make][m][pl][remake][util][webbp][plu][plaun][inst][upload [{plus,(optional.exe)}]][release [(optional.exe)]]"

if [ $0 != "./build.sh" ]
then
  echo "[ERROR]: current running only."
  echo $usage
  exit 1
fi

if [ $# -eq 0 ] | [ $# -gt 2 ]
then
  echo $usage
  exit 1
fi

if [ "$1" = "cygup" ]
then
    echo scp /usr/bin/cygz.dll /usr/bin/cygwin1.dll /usr/bin/cyggcc_s-1.dll upload@$sername:$release
    scp /usr/bin/cygz.dll /usr/bin/cygwin1.dll /usr/bin/cyggcc_s-1.dll upload@$sername:$release
    exit $?
fi

if [ "$1" = "upload" ]
then

  if [ "$#" -eq 1 ]
  then
    echo scp ../../../pExtADs-*exe upload@$sername:$upload
    scp ../../../pExtADs-*exe upload@$sername:$upload
    exit $?
  fi

  if [ "$2" = "plus" ]
  then
    echo scp ../../../pExtADs.exe upload@$sername:$upload
    scp ../../../pExtADs.exe upload@$sername:$upload
    exit $?
  fi

  echo scp ../../../pExtADs-All.exe upload@$sername:$upload$2
  scp ../../../pExtADs-All.exe upload@$sername:$upload$2
  exit $?
fi

if [ "$1" = "release" ]
then

  if [ "$#" -eq 1 ]
  then
    echo $usage
    exit 1
  fi

  echo scp ../../../pExtADs-All.exe upload@$sername:$release$2
  scp ../../../pExtADs-All.exe upload@$sername:$release$2
  exit $?
fi

if [ "$1" = "m" ]
then
  cd ../current/; make; cd - >/dev/null

elif [ "$1" = "make" ]
then
  rm -f ~/.wgetrc
  wget http://p.plustar.jp/repv.php?t=padsvc -O ../current/version.h
  cd ../current/; make; cd - >/dev/null
  date -u

elif [ "$1" = "remake" ]
then
  rm -f ~/.wgetrc
  wget http://p.plustar.jp/repv.php?t=padsvc -O ../current/version.h
  if [ -e $yuicompressor ]
  then
  java -jar $yuicompressor --type js  --charset utf-8 -o ${templates_dir}/plustar-jqueryc  ${templates_dir}/plustar-jquery
  java -jar $yuicompressor --type js  --charset utf-8 -o ${templates_dir}/plustar-analc    ${templates_dir}/plustar-anal
  java -jar $yuicompressor --type js  --charset utf-8 -o ${templates_dir}/plustar-adsdkc   ${templates_dir}/plustar-adsdk
  java -jar $yuicompressor --type js  --charset utf-8 -o ${templates_dir}/plustar-showadsc ${templates_dir}/plustar-showads
  java -jar $yuicompressor --type js  --charset utf-8 -o ${templates_dir}/plustar-sdkc     ${templates_dir}/plustar-sdk
  java -jar $yuicompressor --type css --charset utf-8 -o ${templates_dir}/plustar-adcssc   ${templates_dir}/plustar-adcss
  else
  cp ${templates_dir}/plustar-jquery  ${templates_dir}/plustar-jqueryc
  cp ${templates_dir}/plustar-anal    ${templates_dir}/plustar-analc
  cp ${templates_dir}/plustar-adsdk   ${templates_dir}/plustar-adsdkc
  cp ${templates_dir}/plustar-showads ${templates_dir}/plustar-showadsc
  cp ${templates_dir}/plustar-sdk     ${templates_dir}/plustar-sdkc
  cp ${templates_dir}/plustar-adcss   ${templates_dir}/plustar-adcssc
  fi
  cd ../current/; make clean all; cd - >/dev/null
  date -u

elif [ "$1" = "lib" ]
then
  cp /usr/bin/cygz.dll .
  cp /usr/bin/cygwin1.dll .
  cp /usr/bin/cyggcc_s-1.dll .

elif [ "$1" = "pads" ]
then
  explorer ..\\pAds.sln

elif [ "$1" = "util" ]
then
  explorer ..\\pUtils\\pUtils.sln

elif [ "$1" = "webbp" ]
then
  explorer ..\\pUtils\\pWebBP.sln

elif [ "$1" = "plaun" ]
then
  explorer ..\\..\\pLauncher\\pLauncher.sln

elif [ "$1" = "inst" ]
then
  rm -f ~/.wgetrc
  wget http://p.plustar.jp/repv.php?t=padsupd -O upd
  cd ../../..; ./build-pads.bat 2>&1|nkf -u; chmod 666 pExtADs*exe; cd - >/dev/null

else
  echo "[ERROR]: invalid parametor."
  echo $usage

fi
