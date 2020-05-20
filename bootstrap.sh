#! /bin/sh

export LLVM_SRC_TREE="llvm-8.0.0/llvm-8.0.0.src/"

# Copy the folder containing the BBFreqInfo pass to LLVM source tree.
cp -r AccelSeeker  AccelSeekerIO  $LLVM_SRC_TREE/lib/Transforms/.
echo "add_subdirectory(AccelSeeker)" >> $LLVM_SRC_TREE/lib/Transforms/CMakeLists.txt 
echo "add_subdirectory(AccelSeekerIO)" >> $LLVM_SRC_TREE/lib/Transforms/CMakeLists.txt 
