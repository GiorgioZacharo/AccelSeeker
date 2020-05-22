#!/bin/bash


# Prepare the AccelSeeker Candidates for the selection taken into account the overlapping rule.
# Crop the non-duplicate (clean) Indexes file according to the selections that have been done.

# Files used as input from the AccelSeeker analysis
FILE_MERIT_COST=MC.txt
FILE_INDEXES_CLEAN=FCI_CLEAN.txt

# File Generated
FILE_INDEXES_CROPPED=FCI_CROPPED.txt

awk '{ while(++i<=NF) printf (!a[$i]++) ? $i FS : ""; i=split("",a); print ""}' FCI.txt > $FILE_INDEXES_CLEAN  # Remove duplicates 

# Minimum accepted merit
MIN=10

rm $FILE_INDEXES_CROPPED

while read BENCH FUNC_NAME MERIT INPUT AREA INVOCATIONS; do  
    
     while read REG_NAME INDEXES ; do

   	if [ $MERIT -gt $MIN ]; then

		if [ $FUNC_NAME = $REG_NAME ]; then
	
		printf "$REG_NAME\t$INDEXES\n" >> $FILE_INDEXES_CROPPED

		fi
	fi

    done<$FILE_INDEXES_CLEAN

done<$FILE_MERIT_COST

rm $FILE_INDEXES_CLEAN
