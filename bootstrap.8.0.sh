#! /bin/sh

set -e

if [ -d "llvm-8.0.0" ]; then
        echo "directory llvm-8.0.0 exists, unable to continue."
        echo "Remove the existing llvm-8.0.0 directory before bootstrapping."
        exit 1
fi

curl -O https://releases.llvm.org/8.0.0/llvm-8.0.0.src.tar.xz
curl -O https://releases.llvm.org/8.0.0/cfe-8.0.0.src.tar.xz
curl -O https://releases.llvm.org/8.0.0/compiler-rt-8.0.0.src.tar.xz

tar xf llvm-8.0.0.src.tar.xz
tar xf cfe-8.0.0.src.tar.xz
tar xf compiler-rt-8.0.0.src.tar.xz

rm cfe-8.0.0.src.tar.xz compiler-rt-8.0.0.src.tar.xz llvm-8.0.0.src.tar.xz

mv cfe-8.0.0.src llvm-8.0.0.src/tools/clang
mv compiler-rt-8.0.0.src llvm-8.0.0.src/projects/compiler-rt

mkdir "llvm-8.0.0"
mv llvm-8.0.0.src llvm-8.0.0/.

# Create build directory.
mkdir build
mv build llvm-8.0.0/.
cd llvm-8.0.0/build

# Build LLVM 8.0.0 and install it.
cmake $(pwd)/../llvm-8.0.0.src/
cmake --build .
#cmake --build . --target install # Option for root access - not necessary.
