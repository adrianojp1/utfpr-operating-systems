#!/usr/bin/env zsh
mkdir -p temp
for file in espera-ocupada-nosync espera-ocupada-alternancia espera-ocupada-peterson
do
  echo $file
  make rebuild MAIN_FILE=$file
  ./pong > ./temp/$file.txt
done
