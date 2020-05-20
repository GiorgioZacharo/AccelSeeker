#!/bin/bash

# Prepare the Regions+ Candidates nodes for the selection taken into account the overlapping rule.

FILE_MERIT_COST=MC.txt
FILE_INDEXES=FCI_CROPPED_NEW.txt # Overlalpping Rule
#FILE_INDEXES=FCI_CROPPED.txt  # Strict Overlapping Rule
FILE_REGIONS=MCI.txt

#awk '{ while(++i<=NF) printf (!a[$i]++) ? $i FS : ""; i=split("",a); print ""}' FCI.txt > FCI_CLEAN.txt  # Remove duplicates 

#exit 0;

rm $FILE_REGIONS

while read BENCH FUNC_NAME MERIT INPUT AREA INVOCATIONS; do  

        #if [ $FUNC_NAME = $FUNC_NAME_IO ]; then
    while read REG_NAME INDEXES ; do

	if [ $FUNC_NAME = $REG_NAME ]; then
		MEM_AREA=0	
 	
	   if [ $INPUT -ge 32768 ]; then
		MEM_AREA=809702
	   else
		MEM_AREA=90320
	   fi	
	
        #TOT_AREA=$(( MEM_AREA + AREA ))
	
	printf "$BENCH\t$FUNC_NAME\t$MERIT\t$AREA\t$INDEXES\n" >> $FILE_REGIONS

	fi
    done<"$FILE_INDEXES"

done<$FILE_MERIT_COST

sed -i 's/\ /,/g' $FILE_REGIONS
sed -i 's/\t/ /g' $FILE_REGIONS

sort -k3 -n -r $FILE_REGIONS
