# AccelSeeker

# Overview

The AccelSeeker© framework is a tool for automatically identifying and selecting HW accelerators directly from 
the application source files. It is built within LLVM8 compiler infrastructure and consists of Analysis Passes
that estimate Software (SW) latency, Hardware (HW) latency, Area and I/O requirements. Subsequently, an exact 
selection algorithm selects the subset of HW accelerators that maximizes performance (speedup) under a user
defined area (HW resources) budget.

If you use AccelSeeker in your research, we would appreciate a citation to:

Compiler-Assisted Selection of Hardware Acceleration Candidates from Application Source Code.           
Georgios Zacharopoulos, Lorenzo Ferretti, Giovanni Ansaloni, Giuseppe Di Guglielmo, Luca Carloni, Laura Pozzi.      
https://ieeexplore.ieee.org/abstract/document/8988767

# Installation

First we need to install all necessary tools. (LLVM8 and AccelSeeker Analysis passes)

If you already have LLVM8 installed you may skip A. and proceed to B. Otherwise please proceed to A. (suggested).

A.

    ./bootstrap.8.0.sh


The bootstrap.8.0.sh script downloads and builds LLVM8, which is needed to compile and load the AccelSeeker passes. 

If, for any reason, you move/rename LLVM8 source tree, then you have to modify the
"LLVM_BUILD" Paths in the Makefile and the run_sys_aw.sh script, that invokes the AccelSeeker passes, inside the 
directory for each benchmark accordingly. 


-- If LLVM8 is already installed: --

B.

All necessary files containing the analysis passes need to be copied to the LLVM8 source tree. This line requires to be edited according to the path of your LLVM8 installation:

    export LLVM_SRC_TREE="path/to/llvm/source/root"

In order to provide the correct path to your LLVM source tree. 
 
    ./bootstrap_AS_passes.sh

LLVM8 can then be recompiled using make and a new Shared Object (SO) should be created in order to load the AccelSeeker passes.

    cd "path/to/llvm/build" && make


# Usage

For testing the H.264 decoder https://github.com/adsc-hls/synthesizable_h264 synthesizable version  Xinheng Liu et al by University of Illinois at Urbana-Champaign is used.

    cd h264_ir_orig

First we make sure that the line in "run_sys_aw.sh" points to the path of the LLVM8 build directory:

    LLVM_BUILD=path/to/llvm/build

1) Identification of Candidates for Acceleration and Estimation of Latency, Area and I/O    
    
    ./run_sys_aw.sh

This script invokes the AccelSeeker Analysis passes and generates the files needed to construct the final Merit/Cost estimation.
The files generated are: FCI.txt  IO.txt  LA.txt

2) Merit, Cost Estimation of Candidates for Acceleration and Application of the Overlappping Rule

    ./generate_accelcands_list.sh

This script generates the Merit/Cost (MC) file along with the implementation of the Overlappping rule in the final Merit/Cost/Indexes (MCI) file.
The files generated are: MCI.txt  MC.txt


3) Selection of Candidates for Acceleration

The MCI.txt file can be used subsequently by the exact selection algorithm in order to select the subsets of the AccelSeeker candidates list that maximize Merit (Speedup) under various Costs (Area budgets or HW resources).

    ../accel_selection_algo_src/accel-find MCI.txt AREA_BUDGET

e.g.  ../accel_selection_algo_src/accel-find MCI.txt 1000

To delete all data files use:

    ../scripts/delete_all_data_files.sh 


# Methodology

AccelSeeker performs identification of valid candidates for acceleration (AccelCands) and estimates their performance in terms of speedup gains (cycles saved or hereafter called Merit) and hardware resources required 
(area or hereafter called Cost).

The process is as follows:

    a) Dynamic Profiling of the runtime.

    The existing tools of LLVM are used to generate an instrumented version of the binary of a provided benchmark.

        http://clang.llvm.org/docs/UsersManual.html#profiling-with-instrumentation

        e.g.

         clang -O3 -fprofile-instr-generate bench.c -o bench_instrumented

    The bench.profdata file generated and used to generate the respective *.ir files of the benchmark.

         bench_instrumented $(BENCH_COMMAND_LINE_PARAMETERS)
         llvm-profdata merge -output=$(BENCH).profdata default.profraw

         clang -S -emit-llvm -O3 -fprofile-instr-use=$(BENCH).profdata -o bench.ir bench.c


    b) AccelCands Identification and performance/area estimation (Merit/Cost estimation).

    The Analysis Passes are invoked. We are providing as input the *.ir files that were generated in the previous step.

    The output from loading this pass provides us with a full analysis of the AccelCands regarding their Merit, Cost estimation and the application of the overlapping rule, so that candidates whose computation completely
    overlaps with the computation of other candidates are mutually exclusive.


    c) AccelCands Selection (HW accelerators selection)

    The Selection phase takes place where a subset of the initial set of potential candidates for acceleration
    are selected, so that their cumulative speedup is maximized and their cumulative area does not exceed a user-defined area budget.


** Modifications are needed to comply for every benchmark. **

# Author

Georgios Zacharopoulos georgios@seas.harvard.edu Date: May, 2020
