#!/bin/bash

ALPHA=1750  # // Based on Memory Hierarchy of the Architecture. (1.75)
OVERHEAD=100

while read FUNC_NAME SW_LATENCY HW_LATENCY AREA ; do
	while read FUNC_NAME_IO INPUT OUTPUT; do	

	if [ $FUNC_NAME = $FUNC_NAME_IO ]; then

		IO_LATENCY_HW=$(( ALPHA * INPUT )) # CYCLES

		if [ $IO_LATENCY_HW -gt $HW_LATENCY ]; then
			MERIT=$(( SW_LATENCY - IO_LATENCY_HW  - $OVERHEAD ))
			#echo "$FF $AREA"
		else
			MERIT=$(( SW_LATENCY - HW_LATENCY - $OVERHEAD ))
			# CS=$(( TOT_LATENCY_SW - TOT_LATENCY_HW  - ($INV * $OVERHEAD) ))
		fi




		# # Cycles Saved computation.
		# CS=$(( TOT_LATENCY_SW - TOT_LATENCY_HW  - ($INV * $OVERHEAD) ))
		
		# if [ $FF -gt $LUT ]; then
		# 	AREA=$FF
		# 	#echo "$FF $AREA"
		# else
		# 	AREA=$LUT
		# fi
		
		echo  "$FUNC_NAME $MERIT $AREA" >> MC.txt
	fi

	done<IO.txt
done<LA.txt