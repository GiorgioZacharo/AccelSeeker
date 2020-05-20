#!/bin/bash

# Prepare the Regions+ Candidates nodes for the selection taken into account the overlapping rule.

rm FCI_CLEAN.txt 

awk '{ while(++i<=NF) printf (!a[$i]++) ? $i FS : ""; i=split("",a); print ""}' FCI.txt > FCI_CLEAN.txt  # Remove duplicates 

#exit 0;

count=0

while read ARG1 ARG2 ARG3 ARG4 ARG5 ARG6 ARG7 ARG8 ARG9 ARG10 ARG11 ARG12 ARG13 ARG14 ARG15 ARG16 ARG17 ARG18 ARG19 ARG20  ; do

  for i in {1..20}; do
  #   if [ $ARG$i = "Unknown" ]; then
     if [ $count -eq 1 ]; then
        echo $ARG$i
     fi
  done

((count++))

done<FCI_CLEAN.txt



