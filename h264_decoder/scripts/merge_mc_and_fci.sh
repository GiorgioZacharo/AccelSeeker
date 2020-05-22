#!/bin/bash

# Prepare the AccelSeeker Candidates for the selection taken into account the overlapping rule.

rm FCI_CROPPED.txt 

FILE_MERIT_COST=MC.txt 			# Merit/Cost file.
FILE_INDEXES=FCI_CROPPED_OVERLAP_RULE.txt # Overlapping Rule file.

# Accel Candidates list file with Merit/Cost and Call Function Indexes. (Output file)
FILE_ACCELCANDS=MCI.txt

rm $FILE_ACCELCANDS

while read BENCH FUNC_NAME MERIT INPUT AREA INVOCATIONS; do  

    while read ACCEL_NAME INDEXES ; do

	if [ $FUNC_NAME = $ACCEL_NAME ]; then
	
	printf "$BENCH\t$FUNC_NAME\t$MERIT\t$AREA\t$INDEXES\n" >> $FILE_ACCELCANDS

	fi
    done<"$FILE_INDEXES"

done<$FILE_MERIT_COST

sed -i 's/\ /,/g' $FILE_ACCELCANDS
sed -i 's/\t/ /g' $FILE_ACCELCANDS

sort -k3 -n -r $FILE_ACCELCANDS

rm $FILE_INDEXES
