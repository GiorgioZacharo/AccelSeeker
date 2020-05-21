############### This is the System Aware AccelSeeker Analysis on the whole app ##############
#
#
#    Georgios Zacharopoulos <georgios@seas.harvard.edu>
#    Date: January, 2020
#    Harvard University / Universita' della Svizzera italiana (USI Lugano)
############################################################################################### 

#!/bin/bash

# LLVM build directory - Edit this line.
# LLVM_BUILD=path/to/llvm/build
LLVM_BUILD=../llvm-8.0.0//build

TOP_LEVEL=6
IRDIR=IR

#cd $IRDIR
#$LLVM_BUILD/bin/llvm-link -S  *.ir -o ../h264.ir
#cd ..



# Collects IO informarmation, Indexes info and generates .gv call graph files for every function.
$LLVM_BUILD/bin/opt -load $LLVM_BUILD/lib/AccelSeekerIO.so -AccelSeekerIO -stats    > /dev/null  h264.ir
mkdir gvFiles; mv *.gv gvFiles/.


# Collects SW, HW and AREA estimation bottom up.
for ((i=0; i <= $TOP_LEVEL ; i++)) ; do
	echo "$i"
 printf "$i" > level.txt

$LLVM_BUILD/bin/opt -load $LLVM_BUILD/lib/AccelSeeker.so -AccelSeeker -stats    > /dev/null  h264.ir
done

cp LA_$TOP_LEVEL.txt LA.txt; mkdir analysis_data; mv SW_*.txt HW_*.txt AREA_*.txt LA_*.txt analysis_data/.  
rm level.txt

exit 0;

# Delete all files
rm *.txt; rm -r gvFiles
