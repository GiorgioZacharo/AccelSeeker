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

    // Run on the whole app.
    bool runOnFunction(Function &F){
 
     if (!isSystemCall(&F)){
	     errs() << "\n\n Function Name : " << F.getName() << "\n";

        errs() << "\n\n Initialize list with Funs : " << "\n";
        initFunctionList(&F);

        for (unsigned int i=0; i< Function_list.size(); i++)
          errs() << GetValueName(Function_list[i]) << "\n";

        errs() << "\n\n Get the calls within Functions : " << "\n";
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


      virtual void getAccelSeekerIO(Function *F) {

  
        // LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
        // ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();

        //std::string Function_Name = F->getName();
        std::string Function_Name = GetValueName(F);
       
        // 1
        //

        long int InputData = getInputFunction(F);

         IO_file.open ("IO.txt", std::ofstream::out | std::ofstream::app); 
         IO_file << Function_Name << " " << InputData << "\n";
         IO_file.close();

      

    

      // 2
      //    
      int F_index = find_function(Function_list, F);
    	// if (Function_Name != "@decode_main" && Function_Name != "@main" 
    	//    && Function_Name != "@3"  
    	//    && Function_Name != "@7" && Function_Name != "@ProcessSlice") { 

          myfile.open ("FCI.txt", std::ofstream::out | std::ofstream::app); 
          myfile << Function_Name << " " << F_index << " " ;
          myfile.close();
            getIndexesOfCalledFunctions(F);

          myfile.open ("FCI.txt", std::ofstream::out | std::ofstream::app); 
          myfile << "\n";
          myfile.close(); 
    	// }




        //3 Print gv files for Caller -> Callee Relationship.

  //     if (Function_Name != "@decode_main" && Function_Name != "@main" 
  //          && Function_Name != "@3" 
		// && Function_Name != "@ProcessSlice"
	 //         ) { 
         myfile.open (Function_Name +".gv", std::ofstream::out | std::ofstream::app);
         myfile << "digraph \"" << Function_Name << "\" {" << "\n";
         myfile << Function_Name << "[weight = 1, style = filled]" << "\n"; 
         myfile.close();

        
         printRealFreqOfCalledFunsGraphs(F, Function_Name);

         myfile.open (Function_Name +".gv", std::ofstream::out | std::ofstream::app);
         myfile << "}" << "\n";
         myfile.close();

	 //    }

    
    } // End of RunOnFunction


    // Helper Function to log Function names and teh included calls in a text file.
    //
    //
    void getFunctionNamesCalls(Function *F, int Level) {

      std::string Function_Name = GetValueName(F);

      myfile.open ("FCN.txt", std::ofstream::out | std::ofstream::app); 
      myfile << "L"<< Level << ": " << Function_Name << "\t";
      myfile.close();


      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
         
           if(CallInst *Call = dyn_cast<CallInst>(BI)) {

            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {

                Function *Calee = Call->getCalledFunction();
                Level++;
                getFunctionNamesCalls(Calee, Level);
                Level--;         

              }
            }
            
            else{

              Value* V=Call->getCalledValue();
              Value* SV = V->stripPointerCasts();

              if (!isIndirectSystemCall(SV) ) {

       
                //StringRef Indirect_Called_Name = SV->getName();
                StringRef Indirect_Called_Name = GetValueName(SV);
                //errs() <<"\t" << F->getName() << "\t -->\t" << Indirect_Called_Name << "\n";

                int fun_index = find_function_name(Function_Names_list, Indirect_Called_Name);

                if (fun_index >=0) { // Is it in our list?

                  errs() << "Indirect Call in Our list! " << "\n";

                  Function *Calee = Function_list[fun_index];
                  Level++;
                  getFunctionNamesCalls(Calee, Level);
                  Level--;
      
                }
                else
                  errs() << "Indirect Call *not* in Our list! \t" << Indirect_Called_Name << "\n";

              }
            } // End of Else branch.
           
           } // End of Call Instruction
         } // End of For - BB Iterator

        } // End of For - Function Iterator
    }


    // Input Requirements from parameter List.
    //
    //
    long int getInputFunction(Function *F) {
      long  int InputData = 0; // Bits
      long int InputDataBytes = 0; // Bytes

      int arg_index=0;

      //Function::ArgumentListType & Arg_List = F->getArgumentList();

      //for (Function::arg_iterator AB = Arg_List.begin(), AE = Arg_List.end(); AB != AE; ++AB){
      for (Function::arg_iterator AB = F->arg_begin(), AE = F->arg_end(); AB != AE; ++AB){

        llvm::Argument *Arg = &*AB;
        llvm::Type *Arg_Type = Arg->getType();




        errs() << "\n\n Argument : " << arg_index << "  --->  " << *AB << " -- " << *Arg_Type  << " --  \n ";

        long int InputDataOfArg = getTypeData(Arg_Type);
        errs() << "\n\n Argument : " << arg_index << "  -- Input Data --  " << InputDataOfArg<< " \n "; 

        InputData += InputDataOfArg;
        arg_index++;

       }

       errs() << "\n\n Total Input Data Bits :  " << InputData << " \n ";
       InputDataBytes = InputData/8; 
       errs() << "\n\n Total Input Data Bytes :  " << InputDataBytes << " \n ";

      return InputDataBytes;
    }


    // Record the Indexes of the Function Calls within each Function.
    //
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
          } // End of IF
        } // End of For - BB Iterator
      }

    }

        // Get Real Frequency of each candidate's Calls to Functions. 
    // print Graphs!
    //
    void printRealFreqOfCalledFunsGraphs(Function *F, std::string TopFunName) {

      std::string Function_Name = GetValueName(F);

      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
         
           if(CallInst *Call = dyn_cast<CallInst>(BI)) {

            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {

                Function *Calee = Call->getCalledFunction();
                //std::string CaleeName = Calee->getName();
                std::string CaleeName =  GetValueName(Calee);
                
                myfile.open (TopFunName +".gv", std::ofstream::out | std::ofstream::app);
                // myfile << "N" << func_counter << "_" << CaleeName << "[weight = 1, style = filled]" << "\n"; 
                // myfile << "N" << func_counter << "_" << Function_Name << " -> " << CaleeName << " ; "  << "\n";
                myfile << CaleeName << "[weight = 1, style = filled]" << "\n"; 
                myfile << Function_Name << " -> " << CaleeName << " ; "  << "\n";
                myfile.close();

                

                printRealFreqOfCalledFunsGraphs(Calee, TopFunName);
              }
            }
            
            else{

              Value* V=Call->getCalledValue();
              Value* SV = V->stripPointerCasts();

              if (!isIndirectSystemCall(SV) ) {

       
                //std::string Indirect_Called_Name = SV->getName();
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
}}

char AccelSeekerIO::ID = 0;
static RegisterPass<AccelSeekerIO> X("AccelSeekerIO", "Identify IO Requirements of System Aware Accelerators");
