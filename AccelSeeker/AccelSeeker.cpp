//===------------------------- AccelSeeker.cpp -------------------------===//
//
//                     The LLVM Compiler Infrastructure
// 
// This file is distributed under the Università della Svizzera italiana (USI) 
// Open Source License.
//
// Author         : Georgios Zacharopoulos 
// Date Started   : June, 2018
//
//===----------------------------------------------------------------------===//
//
// This file identifies and evaluates candidates for HW acceleration.
//
// It is part of a tool that automatically identifies and selects HW accelerators directly from 
// the application source files. It is built within LLVM8 compiler infrastructure and consists of 
// Analysis Passes that estimate Software (SW) latency, Hardware (HW) latency, Area and I/O requirements. 
//
// AccelSeeker Candidates.
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
#include "AccelSeeker.h"

#define DEBUG_TYPE "AccelSeeker"
#define HW_COST_AVG // Defined for a HW average estimation.
STATISTIC(FunctionCounter, "The # of Functions Identified");

using namespace llvm;

namespace {

  struct AccelSeeker : public FunctionPass {
    static char ID; // Pass Identification, replacement for typeid

    std::vector<Function *> Function_list; // Global Function List
    std::vector<long int> Function_HW_Cost_list; // Global Function List HW Latency (Cycles)
    std::vector<long int> Function_SW_Cost_list; // Global Function List SW Latency (Cycles)
    std::vector<long int> Function_HW_Area_list; // Global Function List HW Area Estimation (LUTs)
    std::vector<Function *> Function_Area_list; //  Global Function List for Area Estimation
    std::vector<StringRef> Function_Names_list; // Global Function List Names

    AccelSeeker() : FunctionPass(ID) {}

    // Run Analysis on the entire application (module).
    //
    bool runOnFunction(Function &F){
      std::ifstream level_file;       // File used to log the data for every level of analysis. (Bottom - Up)
      int level;

      level_file.open("level.txt");
      if(level_file.fail()) {        // checks to see if file opened .
        errs() << "error" << "\n";
        return 1;                   // No point continuing if the file didn't open.
      }
      
      while(!level_file.eof()) 
        level_file >> level; // read first column number
 
      if (!isSystemCall(&F)){ 

        errs() << "\n\n Function Name : " << F.getName() << "\n\n\n Initialize list with Funs : \n";
        initFunctionList(&F);
    
        for (unsigned int i=0; i< Function_list.size(); i++)
          errs() << GetValueName(Function_list[i]) << "\n"; // Print the Functions encountered so far.
        
        runAccelSeeker(&F, level); // Run the AccelSeeker Analysis
      } 
      
      FunctionCounter++;
      return false;
    }


    // Populate the list with all Functions of the app *except* for the System Calls.
    //
    void initFunctionList(Function *F) {

      if (find_function(Function_list, F) == -1 && isSystemCall(F) == false){

        std::string Function_Name = GetValueName(F);
        Function_list.push_back(F);
      
        long int HWCostFunction = getHWCostOfFunction(F);
        Function_HW_Cost_list.push_back(HWCostFunction);

        long int SWCostFunction = getSWCostOfFunction(F);
        Function_SW_Cost_list.push_back(SWCostFunction);

        unsigned int AreaFunction = getAreaofFunction(F);
        Function_HW_Area_list.push_back(AreaFunction);

        Function_Names_list.push_back(GetValueName(F));
      }
    }

    // AccelSeeker Analysis (SW. HW, AREA Estimation).
    //
    void runAccelSeeker(Function *F, int LEVEL) {

      //std::string Function_Name = F->getName();
      std::string Function_Name = GetValueName(F);
       
      // -- Analysis Powerhouse.
      
      // SW Estimation.
      unsigned long long int SuperFunctionSWLatency =  logSWCostOfSuperFunction(F, LEVEL);

      // HW Estimation.
      long int SuperFunctionHWLatency = logHWCostOfSuperFunction(F, LEVEL);

      // Frequency of Invocation Extraction from annotated IR. (Dynamic Profiling info)
      long int SuperFunctionFreq      = getEntryCount(F);
       
    
      // Area Estimation.
      Function_Area_list.clear();
      long int SuperFunctionArea      = logAreaofSuperFunction(F, LEVEL);       

      myfile.open ("LA_" + std::to_string(LEVEL) + ".txt", std::ofstream::out | std::ofstream::app);
	    myfile << Function_Name << "\t" 
         << SuperFunctionSWLatency << "\t" 
         << SuperFunctionHWLatency << "\t" 
         << SuperFunctionArea << "\t"
         << SuperFunctionFreq 
         <<"\n";
      myfile.close();
    }


    // Get the Delay Estimation for the Function. 
    // Sum of all BBs HW cost times execution frequency. (Average strategy)
    //
    long int getHWCostOfFunction(Function *F) {

    #ifdef HW_COST_AVG  

      long int HardwareCost =0;
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI(); 

      // Populate worklist with Function's Basic Blocks and their respective BB's Frequencies. Both Per Iteration and Total.
      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));         
        HardwareCost     += ceil( ( getDelayOfBB(&*BB) ) / NSECS_PER_CYCLE ) * BBFreqFloat;
      }

      return HardwareCost;
    

    #else

    // Get the Delay Estimation for the Function. - Critical Path.
    //  

      float DelayOfFunction, DelayOfFunctionTotal = 0;
      long int HardwareCost =0;
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI(); 
      std::vector<BasicBlock *> worklist, predecessor_bb, successor_bb;
      std::vector<float> BBFreqPerIter;
      std::vector<float> BBFreqTotal;
      std::vector<float> DelayFunctionPathsPerIter, DelayFunctionPathsTotal;
      std::vector<long int>   HWCostPath, HWCostFunction;

      // Clear vectors.
      worklist.clear();
      predecessor_bb.clear();
      successor_bb.clear();
      BBFreqPerIter.clear();
      BBFreqTotal.clear();
      DelayFunctionPathsPerIter.clear();
      DelayFunctionPathsTotal.clear();
      HWCostPath.clear();
      HWCostFunction.clear();

      // Populate worklist with Function's Basic Blocks and their respective BB's Frequencies. Both Per Iteration and Total.
      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {


        float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
        //int EntryFuncFreq = getEntryCount(F);
        float BBFreq = BBFreqFloat; //* static_cast<float>(EntryFuncFreq);
        
        worklist.push_back(&*BB);
        BBFreqPerIter.push_back(BBFreqFloat);
        BBFreqTotal.push_back(BBFreq);
        HWCostFunction.push_back(ceil( ( getDelayOfBB(&*BB) ) / NSECS_PER_CYCLE ) * BBFreqTotal[find_bb(worklist, &*BB)] ); // HW Cost for each BB (Cyclified) 
        HWCostPath.push_back(ceil( ( getDelayOfBB(&*BB) ) / NSECS_PER_CYCLE ) * BBFreqTotal[find_bb(worklist, &*BB)] );  // maybe add static cast long int for BBFreqTotal
      }

      // Function has more than one BBs.
      if (worklist.size() > 1) {

        // Find Relations among BBs.
        //
        // Predecessor --> Successor
        //
        //
        int count =0;  
        
        for (std::vector<BasicBlock *>::iterator bb_iter = worklist.begin(); bb_iter != worklist.end(); ++bb_iter, count++) {

          if(BasicBlock *BB = *bb_iter) {
            // Getting the Succeror BBs of each BB in the worklist.
            for (succ_iterator SI = succ_begin(BB), SE = succ_end(BB); SI != SE; ++SI) { 
              
              BasicBlock *Succ = *SI;      
              if(count != find_bb(worklist, Succ) ) { // Changed it so that we can include all Nodes (Except for the ones that have a backedge to themselves)
                predecessor_bb.push_back(BB); // Populate send_node vector
                successor_bb.push_back(Succ); // Populate receive_node vector
              } // End of if.
            } // End of for.
          } // End  od if.
        }   // End of for.

        // Start of the Updated part.

        // Converting a CFG with backedges to a DAG (Data Acyclic Graph)
        // Necessary in case BBs are not listed in order, as expected.
        int pos_successor = 0;
        for (std::vector<BasicBlock *>::iterator succ_iter = successor_bb.begin(); succ_iter != successor_bb.end(); ++succ_iter, pos_successor++) {
          BasicBlock *successor = *succ_iter;

          if (find_bb(worklist, successor) == -1) {                       // Succesor is *not* in our worklist.

            successor_bb.erase(successor_bb.begin() + pos_successor);             // deleting the edge
            predecessor_bb.erase(predecessor_bb.begin() + pos_successor);        //  deleting the edge
            succ_iter--;          // Setting the iterator right after deletion.
            pos_successor--;       // Setting the iterator right after deletion.
            continue;
          }

          int succ_pos_in_pred_list = find_bb(predecessor_bb, successor);
          if ( succ_pos_in_pred_list > pos_successor || succ_pos_in_pred_list == -1)     // Maybe use >= instead of >
            continue;

          int new_position = find_bb(successor_bb, predecessor_bb[pos_successor]);  
          while (new_position < pos_successor && new_position !=-1) {

            if (predecessor_bb[new_position] == successor) {

              successor_bb.erase(successor_bb.begin() + pos_successor);             // deleting the edge
              predecessor_bb.erase(predecessor_bb.begin() + pos_successor);        //  deleting the edge
              succ_iter--;            // Setting the iterator right after deletion.
              pos_successor--;         // Setting the iterator right after deletion.
              new_position = pos_successor; // End the search. Maybe insert a break.
            }

            else
              new_position = find_bb(successor_bb, predecessor_bb[new_position]); 
          }
        } // End of for


        for (std::vector<BasicBlock *>::iterator bb_iter = worklist.begin(); bb_iter != worklist.end(); ++bb_iter) {

          BasicBlock *BB = *bb_iter;

          // Find the end Nodes - Bottom-most Nodes (BBs) in the CFG Graph.
          if (find_bb(predecessor_bb, BB) == -1) {

            BasicBlock *EndNode = BB;
            std::vector<BasicBlock *> BottomNodes;
            BottomNodes.clear();
            BottomNodes.push_back(EndNode);

            BasicBlock *CurrentNode;
            int position, position_bottom_nodes = 0;

            while(BottomNodes.size()>0) {  
             
              CurrentNode = BottomNodes[0];
   
              while (find_bb(successor_bb, CurrentNode) >=0) {

                position = find_bb(successor_bb, CurrentNode); 
                position_bottom_nodes = find_bb(BottomNodes, CurrentNode); // Should be zero.
                // HW Cost Path - Updated the Maximum Path.
                HWCostPath[find_bb(worklist, predecessor_bb[position])] = std::max( HWCostPath[find_bb(worklist, predecessor_bb[position])], HWCostFunction[find_bb(worklist, predecessor_bb[position])] + 
                    HWCostPath[find_bb(worklist, CurrentNode)] );
                // Set predecessor.
                BasicBlock *Predecessor = predecessor_bb[position];

                successor_bb.erase(successor_bb.begin() + position);            // deleting the last edge
                predecessor_bb.erase(predecessor_bb.begin() + position);        //  deleting the last edge

                if (find_bb(predecessor_bb, Predecessor) == -1)
                  BottomNodes.push_back(Predecessor);
                
              }
              BottomNodes.erase(BottomNodes.begin());
            }
          }
        }

        HardwareCost       = get_max_long_int(HWCostPath);                // Total Cycles spent on HW.
      }
      // In case that the Function has only one BB.
      // else if (worklist.size() == 1) {
      else
        HardwareCost   = ceil( ( getDelayOfBB(worklist[0]) ) / NSECS_PER_CYCLE ) * BBFreqTotal[0];

      return HardwareCost;

    #endif
    }
  
    // Log the HW Cost estimation of the Super Functio.
    // Detects calls to other functions and adds their HW cost as well.
    //
    long int logHWCostOfSuperFunction(Function *F,  int CurrentLevel) {

      std::ifstream hw_file;
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
      int CalleeFreq =0;
      int LevelSuperFunction=0;
      unsigned long long int HWCostSuperFunction=0;

      //std::string Function_Name = F->getName();  
      std::string Function_Name = GetValueName(F);  
      int EntryCount = getEntryCount(F);
      CalleeFreq = EntryCount; // This has been changed - May need to be updated in the future.
  
      HWCostSuperFunction = getHWCostOfFunction(F);
      errs() << "--HW Cost1\t" << HWCostSuperFunction  << " CostFunction " << getHWCostOfFunction(F) 
        << " Freq "   << CalleeFreq << " " << GetValueName(F) << "\n\n";

      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
           if(CallInst *Call = dyn_cast<CallInst>(BI)) {    
            if (Call->getCalledFunction()) {
              if (!isSystemCall(Call->getCalledFunction())) {

                if (LevelSuperFunction == 0)
                  LevelSuperFunction = CurrentLevel; // Assign the right Level of Calling Functions.

                float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
                CalleeFreq = static_cast<int> (BBFreqFloat * EntryCount); // This has been updated.

                Function *Calee = Call->getCalledFunction();
                std::string Calee_Name = GetValueName(Calee);
                unsigned long long int HWCostCalee = 0;
                double CalleeFreqRatio = 1;

                long int hw_latency;
                int entry_count;
                std::string fun_name;
                bool found = false;
                
                for (int i = CurrentLevel-1; i>=0; i--) {
                              
                  hw_file.open("HW_" + std::to_string(i) + ".txt");
                  if(hw_file.fail()) { // checks to see if file opended 
                    errs() << "error" << "\n"; 
                    return 1; // no point continuing if the file didn't open.
                  } 

                  while(!hw_file.eof()) { 
                    hw_file >> fun_name; // read first column number
                    hw_file >> hw_latency; // read second column number
                    hw_file >> entry_count; // read second column number

                   if( Calee_Name == fun_name ){
                    found = true;

                    if (entry_count<=0)
                      entry_count = 1;

                    if (CalleeFreq<=0)
                      CalleeFreq = 1;

                    CalleeFreqRatio =  (float)CalleeFreq / (float) entry_count;
                    double intpart, fractpart;

                    if (CalleeFreqRatio > 1) {
                        fractpart = modf (CalleeFreqRatio , &intpart);
                        CalleeFreqRatio = fractpart > 1 - fractpart ? fractpart : 1 - fractpart;
                    }

                    if (Function_Name == "main" || Function_Name == "decode_main"  ) // Only for main
                      CalleeFreqRatio=1;

                      HWCostCalee  =  hw_latency * CalleeFreqRatio;
                    } // End of if.
                  } // End of while    

                   hw_file.close();

                   if (found)
                    break; // break when read from the top Level File. (Maximum Latency)

                } // End of for

                HWCostSuperFunction += HWCostCalee; // Final Computation of HWCostSuperFunction/

              } // End of If - System Call
            }
           }    // End of Call Instruction
         }      // End of For - BB Iterator
        } // End of For - Function Iterator

      if (LevelSuperFunction == CurrentLevel &&  HWCostSuperFunction>0){
        errs() << "Writing to file " << CurrentLevel << "\n";

        myfile.open ("HW_" + std::to_string(CurrentLevel) + ".txt", std::ofstream::out | std::ofstream::app); 
        myfile << Function_Name << "\t" 
            << HWCostSuperFunction << "\t"
            << EntryCount << "\t"
            <<"\n";
        myfile.close();    

        errs() << "------HW Cost 3\t" << HWCostSuperFunction<< " " << " " << CalleeFreq << " " << GetValueName(F) << "\n\n";
      }

      return HWCostSuperFunction;
    }


    // Log SW estimation of the Super Function
    // Detect calls to other functioms and add their SW cost as well.
    //
    unsigned long long int logSWCostOfSuperFunction(Function *F, int CurrentLevel) {

      std::ifstream sw_file;
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
      int CalleeFreq, EntryCount;
      int LevelSuperFunction=0;
      unsigned long long int SWCostSuperFunction=0;

      std::string Function_Name = GetValueName(F);  
      EntryCount = getEntryCount(F);
      CalleeFreq = EntryCount;

      // A fix for specofic case of function merged Apps - May remove it (H.264).
      if (CalleeFreq==0)
	     CalleeFreq=1;	 
	
      SWCostSuperFunction = getSWCostOfFunction(F) * (CalleeFreq);
      // Enabled only for Debbugging.
      //errs() << "--SW Cost1\t" << " Cost SuperF " << SWCostSuperFunction<< " CostF " 
      //  << getSWCostOfFunction(F) << " Freq  " << CalleeFreq << " " << GetValueName(F) << "\n\n";

      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
         if(CallInst *Call = dyn_cast<CallInst>(BI)) {    
            if (Call->getCalledFunction()) {
              if (!isSystemCall(Call->getCalledFunction())) {
                
                if (LevelSuperFunction == 0)
                  LevelSuperFunction = CurrentLevel; // Assign the right Level of Calling Functions

                float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
                CalleeFreq = static_cast<int> (BBFreqFloat * EntryCount);

                Function *Calee = Call->getCalledFunction();
                std::string Calee_Name = GetValueName(Calee);
                unsigned long long int SWCostCalee = 0;
                double CalleeFreqRatio = 1;

                long int sw_latecy;
                int entry_count;
                std::string fun_name;
                bool found = false;
                
                for (int i = CurrentLevel-1; i>=0; i--) {
                            
                  sw_file.open("SW_" + std::to_string(i) + ".txt");
                  if(sw_file.fail()) { // checks to see if file opended 
                    errs() << "error" << "\n"; 
                    return 1; // no point continuing if the file didn't open...
                  } 

                  while(!sw_file.eof()) { 
                    sw_file >> fun_name; // read first column number
                    sw_file >> sw_latecy; // read second column number
                    sw_file >> entry_count; // read second column number

                    if( Calee_Name == fun_name ){
                      
                      found = true;

                      if (entry_count<=0)
                        entry_count = 1;

                      CalleeFreqRatio =  (float)CalleeFreq / (float) entry_count;

                      double intpart, fractpart;
                      if (CalleeFreqRatio > 1) { // Fixing CalleeFreqRatio in case it is over 1. 
                        fractpart = modf (CalleeFreqRatio , &intpart); // Receive the biggest part of the complimentary to one fractal part.
                        CalleeFreqRatio = fractpart > 1 - fractpart ? fractpart : 1 - fractpart; 
                      }

                      if (Function_Name == "main" || Function_Name == "decode_main") // Only for main and decode_main
                        CalleeFreqRatio=1;

                      SWCostCalee  =  sw_latecy * CalleeFreqRatio;
                    }
                  } // End of while    
                  sw_file.close();

                  if (found)
                    break; // break when read from the top Level File. (Maximum Latency)
                } // End of for

                SWCostSuperFunction += SWCostCalee; // Final Computation of SWCostSuperFunction
              } // End of If - System Call
            }           
          }    // End of Call Instruction
        }      // End of For - BB Iterator
      }        // End of For - Function Iterator


      if (LevelSuperFunction == CurrentLevel &&  SWCostSuperFunction>0){
        errs() << "Writing to file " << CurrentLevel << "\n";

        myfile.open ("SW_" + std::to_string(CurrentLevel) + ".txt", std::ofstream::out | std::ofstream::app); 
        myfile << Function_Name << "\t" 
            << SWCostSuperFunction << "\t"
            << EntryCount << "\t"
            <<"\n";
        myfile.close();    

        // Enabled only for Debbugging.
        //errs() << "------ SW Cost 2\t" << SWCostSuperFunction  << " " << CalleeFreq << " " << GetValueName(F) << "\n\n";
      }

      return SWCostSuperFunction;
    }

    // Get the Delay pf SW Estimation for the Function. 
    // Sum of all BBs SW cost multiplied by ther dynamic execution frequency.
    //
    long int getSWCostOfFunction(Function *F) {

      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
      long int Cost_Software_Function = 0;

      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        long int Cost_Software_BB = 0;
        float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
 
        // Calculate the Software Cost in Cycles multiplied with the respective frequency of the BB.
        Cost_Software_BB = static_cast<long int> (getSWCostOfBB(&*BB) * BBFreqFloat);
        Cost_Software_Function += Cost_Software_BB;
      }

      return Cost_Software_Function;
    }

  
  // Get the Area extimation of a Super Function in LUTs.
  //
  unsigned int logAreaofSuperFunction(Function *F, int CurrentLevel) {

    std::ifstream area_file;
    int LevelSuperFunction=0;
    unsigned int AreaofSuperFunction = 0;
    std::string Function_Name = GetValueName(F);  

    AreaofSuperFunction = getAreaofFunction(F);
    // Enabled only for Debbugging.
    //errs() << "-Area  Cost1\t" << AreaofSuperFunction<< " " << getAreaofFunction(F)  << " " << GetValueName(F) << "\n\n";

    for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
        // Iterate inside the basic block.
      for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
         if(CallInst *Call = dyn_cast<CallInst>(BI)) {        
          if (Call->getCalledFunction()) {
            if (!isSystemCall(Call->getCalledFunction())) {

              if (LevelSuperFunction == 0)
                LevelSuperFunction = CurrentLevel; // Assign the right Level of Calling Functions

              Function *Calee = Call->getCalledFunction();
              std::string Calee_Name = GetValueName(Calee);
              unsigned long long int AreaCostCalee = 0;

              if (find_function(Function_Area_list, Calee) == -1) {
              
                Function_Area_list.push_back(Calee);

                long int area;
                std::string fun_name;
                bool found = false;
                
                for (int i = CurrentLevel-1; i>=0; i--) {
                            
                  area_file.open("AREA_" + std::to_string(i) + ".txt");
                  if(area_file.fail()) { // checks to see if file opened.
                    errs() << "error" << "\n"; 
                    return 1; // no point continuing if the file didn't open.
                  } 

                  while(!area_file.eof()) { 
                    area_file >> fun_name; // read first column number
                    area_file >> area; // read second column number

                    if (Calee_Name == fun_name){
                      found = true;

                      AreaCostCalee  =  area;
                    } // End of if.
                  } // End of while    

                  area_file.close();

                  if (found)
                    break; // break when read from the top Level File. (Maximum Latency)
                } // End of for

                AreaofSuperFunction += AreaCostCalee; // Final Computation of AreaofSuperFunction
              } // End of If.
            } // End of If - System Call
          }                       
         } // End of Call Instruction 
       } // End of For - BB Iterator
      } // End of For - Function Iterator

      if (LevelSuperFunction == CurrentLevel &&  AreaofSuperFunction>0){
        errs() << "Writing to file " << CurrentLevel << "\n";

        myfile.open ("AREA_" + std::to_string(CurrentLevel) + ".txt", std::ofstream::out | std::ofstream::app); 
        myfile << Function_Name << "\t" 
            << AreaofSuperFunction << "\t"
            <<"\n";
        myfile.close();    
      }
    return AreaofSuperFunction;
  }

  // Retrieves the Frequency of invocation for every function by extracting the metadata
  // carrying dynamic profiling information, annotated in the IR.
  //
  int getEntryCount(Function *F) {

    int entry_freq = 0;
    if (F->hasMetadata()) {

      MDNode *node = F->getMetadata("prof");

      if (MDString::classof(node->getOperand(0))) {
        auto mds = cast<MDString>(node->getOperand(0));
        std::string metadata_str = mds->getString();

        if (metadata_str == "function_entry_count")
          if (ConstantInt *CI = mdconst::dyn_extract<ConstantInt>(node->getOperand(1))) {
            entry_freq = CI->getSExtValue();
          }              
      }
    }
    return entry_freq;
  }

  virtual void getAnalysisUsage(AnalysisUsage& AU) const override {
              
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<RegionInfoPass>();
   // AU.addRequired<DependenceAnalysis>();
    AU.addRequiredTransitive<RegionInfoPass>();
    // AU.addRequiredTransitive<ScalarEvolutionWrapperPass>();
    AU.addRequired<BlockFrequencyInfoWrapperPass>();
    AU.setPreservesAll();
    } 
  };
}

char AccelSeeker::ID = 0;
static RegisterPass<AccelSeeker> X("AccelSeeker", "Identify Valid System Aware Accelerators");
