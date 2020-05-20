# AccelSeeker

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
 
 
 Makefiles

    There is a Makefile_region file for every benchmark that needs minor modifications
    in order to be used for each of them.

    The Makefile_region is included in the Makefile_orig of every benchmark.
    Makefile_orig is simply a copy of the original Makefile, wich has been slightly modified
    to use the Makefile_region. (to include the Makefile_Region) 


    
    profile

        The profile rule compiles all the needed objects and generates the instrumented
        version of the binary. Using the the bench.profdata file generated, the respective
        *.ir files are produced with the initial profiling annotation.


        e.g.
        
        make -f Makefile_orig profile
       
    region

        The region rule requires profile. It receives as input the *.ir files and loads the BB 
        frequency Annotation Pass and outputs the *.bbfreq.ll files. In sequence, these file are
        used to load the final Analysis of Region Identification Pass.


        e.g.
        
        make -f Makefile_orig region


    sort_regions
        
        Runs the shell script that executes make region, gets the relevant information for each Region
        and sorts them out according to density.

        e.g. 

            Good 576912 Dens 3898 Func BlockSAD Reg for.body6 => for.inc120 I 38 O 0 Loads 16 Stores 1
            Good 30906  Dens 2207 Func main Reg for.body3 => for.inc16 I 4 O 0 Loads 0 Stores 2 


Usage

    The Makefile is used as follows:

    e.g.
    
    make -f Makefile_orig profile
    make -f Makefile_orig region


   ** Modifications are needed to comply for every benchmark. **



# Authors

Georgios Zacharopoulos <georgios.zacharopoulos@usi.ch>
Date: January, 2020    
