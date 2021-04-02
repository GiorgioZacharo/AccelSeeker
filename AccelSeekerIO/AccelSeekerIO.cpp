//===------------------------- AccelSeekerIO.cpp -------------------------===//
//
//                     The LLVM Compiler Infrastructure
// 
// This file is distributed under the Università della Svizzera italiana (USI) 
// Open Source License.
//
// Author         : Georgios Zacharopoulos 
// Date Started   : May, 2020
//
//===----------------------------------------------------------------------===//
//
// This file identifies and evaluates candidates for HW acceleration.
//
// It is part of a tool that automatically identifies and selects HW accelerators directly from 
// the application source files. It is built within LLVM8 compiler infrastructure and consists of 
// Analysis Passes that estimate Software (SW) latency, Hardware (HW) latency, Area and I/O requirements. 
// This file identifies and evaluates candidates for HW acceleration.
//
// AccelSeeker Candidates IO requirements and nested Call Function Indexes.
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Analysis/RegionPass.h"
#include "llvm/Analysis/RegionInfo.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/RegionIterator.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Analysis/RegionIterator.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BlockFrequencyInfoImpl.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Transforms/Utils/Local.h"
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include "llvm/IR/CFG.h" 
#include "AccelSeekerIO.h"

#define DEBUG_TYPE "AccelSeekerIO"

using namespace llvm;

namespace {

  struct AccelSeekerIO : public FunctionPass {
    static char ID; // Pass Identification, replacement for typeid

    std::vector<Function *> Function_list; // Global Function List
    std::vector<StringRef> Function_Names_list; // Global Function List Names

    AccelSeekerIO() : FunctionPass(ID) {}

    // Run Analysis on the entire application (module).
    bool runOnFunction(Function &F){
 
     if (!isSystemCall(&F)){
	     errs() << "\n\n Function Name : " << F.getName() << "\n";

        errs() << "\n\n Initialize list with Funs : " << "\n";
        initFunctionList(&F);

        for (unsigned int i=0; i< Function_list.size(); i++)
          errs() << GetValueName(Function_list[i]) << "\n";

        errs() << "\n\n Get the calls within Functions and IO information : " << "\n";
        getAccelSeekerIO(&F);      
      }
      return false;
    }

    // Populate the list with all Functions of the app *except* for the System Calls.
    //
    bool initFunctionList(Function *F) {

      if (find_function(Function_list, F) == -1 && isSystemCall(F) == false){

        std::string Function_Name = GetValueName(F);

        Function_list.push_back(F);
        Function_Names_list.push_back(GetValueName(F));
      }
      return true;
    }

    // AccelSeeker IO Analysis (IO data Estimation, log Indexes and print Function Call Graphs).
    //
    void getAccelSeekerIO(Function *F) {

      std::string Function_Name = GetValueName(F);
     
      // Gather the data requierements of every Function.
      //
      long int InputData = getInputOfFunction(F);

       IO_file.open ("IO.txt", std::ofstream::out | std::ofstream::app); 
       IO_file << Function_Name << " " << InputData << "\n";
       IO_file.close();

      // Log the indexes of every function and every function call included in 
      // a given function to construct the conflict graph.
      //    
      int F_index = find_function(Function_list, F);

      myfile.open ("FCI.txt", std::ofstream::out | std::ofstream::app); 
      myfile << Function_Name << " " << F_index << " " ;
      myfile.close();
      getIndexesOfCalledFunctions(F);
      myfile.open ("FCI.txt", std::ofstream::out | std::ofstream::app); 
      myfile << "\n";
      myfile.close();

      // Print the call graph of every function.  
      // gv files for Caller -> Callee Relationship. 
      myfile.open (Function_Name +".gv", std::ofstream::out | std::ofstream::app);
      myfile << "digraph \"" << Function_Name << "\" {" << "\n";
      myfile << Function_Name << "[weight = 1, style = filled]" << "\n"; 
      myfile.close();
      printCallGraphsOfFunctions(F, Function_Name);
      myfile.open (Function_Name +".gv", std::ofstream::out | std::ofstream::app);
      myfile << "}" << "\n";
      myfile.close();    
    } 

    // Input Requirements from parameter List.
    //
    long int getInputOfFunction(Function *F) {
      long  int InputDataBits = 0; // Bits
      long int InputDataBytes = 0; // Bytes
      int arg_index=0;

      for (Function::arg_iterator AB = F->arg_begin(), AE = F->arg_end(); AB != AE; ++AB){

        llvm::Argument *Arg = &*AB;
        llvm::Type *Arg_Type = Arg->getType();
        long int InputDataOfArg = getTypeData(Arg_Type);
        // Enabled only for Debbugging.
        //errs() << "\n\n Argument : " << arg_index << "  --->  " << *AB << " -- " << *Arg_Type  << " --  \n ";
        //errs() << "\n\n Argument : " << arg_index << "  -- Input Data --  " << InputDataOfArg<< " \n "; 
        InputDataBits += InputDataOfArg;
        arg_index++;
       }

       InputDataBytes = InputDataBits / 8; 
       // Enabled only for Debbugging.
       //errs() << "\n\n Total Input Data Bits :  " << InputDataBits << " \n ";
       //errs() << "\n\n Total Input Data Bytes :  " << InputDataBytes << " \n ";
      return InputDataBytes;
    }


    // Record the Indexes of the Function Calls within each Function.
    //
    void getIndexesOfCalledFunctions(Function *F) {

      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
          if(CallInst *Call = dyn_cast<CallInst>(BI)) {            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {

                Function *Calee = Call->getCalledFunction();
                int fun_index = find_function(Function_list, Calee);

                if (fun_index >=0) {

                  myfile.open ("FCI.txt", std::ofstream::out | std::ofstream::app); 
                  myfile << fun_index <<" ";
                  myfile.close();
                  getIndexesOfCalledFunctions(Calee);
                }
                else{ // Not in the current list
                  // Write a function or reorder this.
                  Function_list.push_back(Calee);
                  int fun_index_new = find_function(Function_list, Calee);
                  
                  myfile.open ("FCI.txt", std::ofstream::out | std::ofstream::app); 
                  myfile << fun_index_new <<" ";
                  myfile.close();
                  getIndexesOfCalledFunctions(Calee);
                }
              }
            }
          } // End of if Call Instruction 
        } // End of For - BB Iterator
      } //End of For - Function Iterator
    }

    // Print the call graph of every function
    // (Generate gv files for Caller -> Callee Relationship.)
    //
    void printCallGraphsOfFunctions(Function *F, std::string TopFunName) {

      std::string Function_Name = GetValueName(F);
      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
          if(CallInst *Call = dyn_cast<CallInst>(BI)) {            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {

                Function *Calee = Call->getCalledFunction();
                std::string CaleeName =  GetValueName(Calee);
                
                myfile.open (TopFunName +".gv", std::ofstream::out | std::ofstream::app);
                myfile << CaleeName << "[weight = 1, style = filled]" << "\n"; 
                myfile << Function_Name << " -> " << CaleeName << " ; "  << "\n";
                myfile.close();

                printCallGraphsOfFunctions(Calee, TopFunName);
              }
            }
            
            else{

              Value* V=Call->getCalledValue();
              Value* SV = V->stripPointerCasts();

              if (!isIndirectSystemCall(SV) ) {

                std::string Indirect_Called_Name = GetValueName(SV);

                myfile.open (TopFunName +".gv", std::ofstream::out | std::ofstream::app);
                myfile << Indirect_Called_Name << "[weight = 1, style = filled]" << "\n"; 
                myfile << Function_Name << " -> " << Indirect_Called_Name << " ; "  << "\n";
                myfile.close();
              }
            } // End of Else branch.
          } // End of Call Instruction
        } // End of For - BB Iterator
      } // End of For - Function Iterator
    }

    virtual void getAnalysisUsage(AnalysisUsage& AU) const override {
              
      AU.addRequired<LoopInfoWrapperPass>();
      AU.addRequired<RegionInfoPass>();
      AU.addRequiredTransitive<RegionInfoPass>();
      AU.addRequiredTransitive<ScalarEvolutionWrapperPass>();
      AU.addRequired<BlockFrequencyInfoWrapperPass>();
      AU.setPreservesAll();
    } 
  };
}

char AccelSeekerIO::ID = 0;
static RegisterPass<AccelSeekerIO> X("AccelSeekerIO", "Identify IO Requirements of System Aware Accelerators");
