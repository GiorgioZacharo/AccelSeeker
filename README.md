# AccelSeekerTool

Overview

    The AccelSeeker© framework is a plugin of LLVM version (3.8) and consists of an LLVM Analysis Pass. It
    performs identification of valid candidates for acceleration (AccelCands) and estimates their performance
    in terms of speedup gains (cycles saved - merit) and hardware resources required (area - cost).

    The process is as follows:

    a) Profiling

        The existing tools of LLVM are used to generate an instrumented version of the binary of
        a provided benchmark.

        http://clang.llvm.org/docs/UsersManual.html#profiling-with-instrumentation

        e.g.

         clang -O3 -fprofile-instr-generate bench.c -o bench_instrumented

        The bench.profdata file generated and used to generate the respective *.ir files of the
        benchmark.

         bench_instrumented $(BENCH_COMMAND_LINE_PARAMETERS)
         llvm-profdata merge -output=$(BENCH).profdata default.profraw

         clang -S -emit-llvm -O3 -fprofile-instr-use=$(BENCH).profdata -o bench.ir bench.c


    b) AccelCands Identification and Merit and Cost estimation

        The Analysis Passes are being in use. We are fproviding as input the *.ir files that were produced
        in the previous step.

         opt -load ~giorgio/llvm_new/build/lib/BBFreqAnnotation.so -O3 -BBFreqAnnotation -stats -S *.ir > *.bbfreq.ll

        Now every Basic Block has the respective frequency annotated in the output *.bbfreq.ll files.
        This information is going to be used next.

         opt -load ~giorgio/llvm_new/build/lib/IdentifyAccelCands.so -IdentifyAccelCands -stats *.bbfreq.ll > /dev/null


        The output from loading this pass provides us with a full analysis of the AccelCands.


    c) AccelCands Selection (candidate selection)

       Candidate Selection is performed by using an exact selection algorithm (not included in this package).


Installation

First you need to copy all the necessary files to your LLVM source tree. You need to edit though this line: 

    export LLVM_SRC_TREE="path/to/llvm/source/root"

In order to provide the correct path to your LLVM source tree. 
 

    ./bootstrap.sh


Then you can recompile it using make and a new SO should be created in order to load the BBFreqInfo
pass.

	cd "path/to/llvm/build" && make

# Usage

	cd h264_ir_orig

	./run_sys_aw.sh

This script invokes the AccelSeeker Analysis passes and generates the files needed to construct the final Merit/Cost estimation.
The files generated are: FCI.txt  IO.txt  LA.txt

        ./generate_accelcands_list.sh

This script generates the Merit/Cost (MC) file along with the implementation of the Overlappping rule in the final Merit/Cost/Indexes (MCI) file.
The files generated are: MCI.txt  MC.txt

The MCI.txt file will be used by the exact selection algorithm in order to select the subsets of the AccelSeeker candidates list that maximize Merit (Speedup)
under various Costs (Area budgets or HW resources).

To delete all data files use:

        scripts/delete_all_data_files.sh 

# Author

Georgios Zacharopoulos georgios@seas.harvard.edu Date: May, 2020
