#!/bin/bash

# Crop the non-duplicate (clean) Indexes file according to the selections that have been done.

#FILE_MERIT_COST=MC_selection.txt
FILE_MERIT_COST=MC.txt
FILE_INDEXES_CLEAN=FCI_CLEAN.txt
FILE_INDEXES_CROPPED=FCI_CROPPED.txt

#awk '{ while(++i<=NF) printf (!a[$i]++) ? $i FS : ""; i=split("",a); print ""}' FCI.txt > FCI_CLEAN.txt  # Remove duplicates 

#exit 0;
MIN=5

rm $FILE_INDEXES_CROPPED

while read BENCH FUNC_NAME MERIT INPUT AREA INVOCATIONS; do  
    
     while read REG_NAME INDEXES ; do

   	if [ $MERIT -gt $MIN ]; then

#echo $FUNC_NAME "   " $$MERIT
		if [ $FUNC_NAME = $REG_NAME ]; then
	
		printf "$REG_NAME\t$INDEXES\n" >> $FILE_INDEXES_CROPPED

		fi
	fi

    done<$FILE_INDEXES_CLEAN

done<$FILE_MERIT_COST

