//===------------------------- IdentifyRegionsSA.cpp -------------------------===//
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
// This file identifies Single-Input, Single-Output Regions in a CFG of an
// application and computes the Data Flow Input and Output for each Region. 

// Regions+ - Extended notion of Regions.
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
#include "../Identify.h" // Header file for all 3 passes. (IdentifyRegions, IdentifyBbs, IdentifyFunctions)
#include "IdentifyRegionsSA.h"

#define DEBUG_TYPE "IdentifyRegions"

#define LEVEL 6

#define HW_COST_AVG

STATISTIC(RegionCounter, "The # of Regions Identified");

using namespace llvm;

namespace {

  struct IdentifyRegions : public FunctionPass {
    static char ID; // Pass Identification, replacement for typeid

    std::vector<Function *> Function_list; // Global Function List
    std::vector<long int> Function_HW_Cost_list; // Global Function List HW Latency (Cycles)
    std::vector<long int> Function_SW_Cost_list; // Global Function List SW Latency (Cycles)
    std::vector<long int> Function_HW_Area_list; // Global Function List HW Area Estimation (LUTs)
    std::vector<Function *> Function_Area_list; //  Global Function List for Area Estimation
    std::vector<Function *> Function_missing_list; //  Global Function List for Area Estimation
    std::vector<StringRef> Function_Names_list; // Global Function List Names
    std::vector<StringRef> Function_missing_list_names;
    // unsigned long long int SWCostSuperFunction = 0;

    //float globalCounter=0; // Testing!

    IdentifyRegions() : FunctionPass(ID) {}

    // Run on teh whole app.
    bool runOnFunction(Function &F){

      if (!isSystemCall(&F)){

        initFunctionList(&F);

        errs() << "\n\n Initialize list with Funs : " << "\n";

      for (int i=0; i< Function_list.size(); i++)
        errs() << Function_list[i]->getName() << "\n";

        errs() << "\n\n Get the calls within Functions : " << "\n";

        runOnFunction(&F);

        runOnFunctionLoop(&F);
      }
      return false;
    }


    // Populate the list with all Functions of the app *except* for the System Calls.
    //
    bool initFunctionList(Function *F) {

      if (find_function(Function_list, F) == -1 && isSystemCall(F) == false){

        std::string Function_Name = F->getName();

        Function_list.push_back(F);
        
        long int HWCostFunction = getHWCostOfFunction(F);
        Function_HW_Cost_list.push_back(HWCostFunction);

        long int SWCostFunction = getSWCostOfFunction(F);
        Function_SW_Cost_list.push_back(SWCostFunction);

        // myfile.open ("SW.txt", std::ofstream::out | std::ofstream::app); 
        // myfile << Function_Name << "\t" 
        //   << SWCostFunction << "\t"
        //   <<"\n";
        // myfile.close();

        unsigned int AreaFunction = getAreaofFunction(F);
        Function_HW_Area_list.push_back(AreaFunction);

        Function_Names_list.push_back(F->getName());
      }

      return true;
    }

    //bool runOnFunction(Function &F) override {
    bool runOnFunction(Function *F){

      errs() << "\n\n\tFunction Name is : " << F->getName() << "\n";
      errs() << "   **********************************************" << '\n';

     

      // Iterate over Regions in the Function
      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {


      // if(BB->getName() == "for.body96" )  
        getBBCall(&*BB, Function_list);    

  
       // Print the DFG Graphs.
       //DFGPrinterBB(&*BB); // Activate to print the DFG graphs.

      }


      // errs() << "   Valid Regions are : " << "\n" ;
      // for (int i=0; i< Region_list.size(); i++)
      //   errs() << " Goodness " << Goodness_list[i] << " Density " << Density_list[i] 
      //       << "   Reg_Name " << Region_list[i]->getNameStr() << "\n" ;


      return false;
    }


      virtual bool runOnFunctionLoop(Function *F) {

  
        LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
        ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();

        std::string Function_Name = F->getName();



        unsigned int NumberOfLoops = 0;
        unsigned int NumberOfArrays = 0;

   
        //getFunctionNumberOfLoopsandArrays(NumberOfLoops, NumberOfArrays, F, LI, SE);
        //errs() << "     Number Of loops  : " << NumberOfLoops   << '\n';
        //errs() << "     Number Of Arrays : " << NumberOfArrays  << '\n';

       
        // 1

        //  // if (NumberOfLoops) { // Might need to add NumberOfArrays as arguments inside the if statement!

        //  //    int InputLoop  = getInputDataLoopFunction(F, LI, SE, NumberOfLoops, NumberOfArrays);
        //  //    int OutputLoop = getOutputDataLoopFunction(F, LI, SE, NumberOfLoops);


        //  //    IO_file.open ("IO.txt", std::ofstream::out | std::ofstream::app); 
        //  //    IO_file << Function_Name << " " << InputLoop << " " << OutputLoop << "\n";
        //  //    IO_file.close();
        //  //  }

    
        // long int InputData = getInputFunction(F);

        // IO_file.open ("IO.txt", std::ofstream::out | std::ofstream::app); 
        // IO_file << Function_Name << " " << InputData << "\n";
        // IO_file.close();

        // 2 -- Analysis Powerhouse

                // errs() << "     Function HW Cycles Estimation "  << getHWCostOfFunction(F) << '\n';
                // errs() << "     Super Function HW Cycles Estimation "  << getHWCostOfSuperFunction(F) << '\n'; 
                
                // errs() << "     Function SW Cycles Estimation "  << getSWCostOfFunction(F) << '\n'; 
                // errs() << "     Super Function SW Cycles Estimation "  << getSWCostOfSuperFunction(F) << '\n'; 

                // errs() << "     Function Area Estimation "  << getAreaofFunction(F) << '\n'; 
                // errs() << "     Super Function Area Estimation "  << getAreaofSuperFunction(F) << '\n';        
      
        unsigned long long int SuperFunctionSWLatency =  logSWCostOfSuperFunction(F, LEVEL);

                // Function_missing_list.clear();
                // Function_missing_list_names.clear(); // Function Calls by reference.

        long int SuperFunctionHWLatency = logHWCostOfSuperFunction(F, LEVEL);

        long int SuperFunctionFreq      = getEntryCount(F);
        
     
        Function_Area_list.clear();
        long int SuperFunctionArea      = logAreaofSuperFunction(F, LEVEL);

        long int SuperFunctionInterfaceArea = getInterfaceAreaFunction(F);
        

        myfile.open ("LA.txt", std::ofstream::out | std::ofstream::app); 
        myfile << Function_Name << "\t" 
          << SuperFunctionSWLatency << "\t" 
          << SuperFunctionHWLatency << "\t" 
          << SuperFunctionArea << "\t"
          << SuperFunctionInterfaceArea << "\t"
          << SuperFunctionFreq 
          <<"\n";
        myfile.close();

      

      // 3
      //std::string Function_Name = F->getName();
      // int F_index = find_function(Function_list, F);


      // myfile.open ("FCI.txt", std::ofstream::out | std::ofstream::app); 
      // myfile << Function_Name << " " << F_index << " " ;
      // myfile.close();
      //   getIndexesOfCalledFunctions(F);

      // myfile.open ("FCI.txt", std::ofstream::out | std::ofstream::app); 
      // myfile << "\n";
      // myfile.close(); 

        //4
        // int Level = 0;
        // getFunctionNamesCalls(F, Level);  
        // myfile.open ("FCN.txt", std::ofstream::out | std::ofstream::app); 
        // myfile  << "\n|\n|\n";
        // myfile.close();

       // getFunctionNamesCalls(F);

        //5
        //int Level = 0; 
        //int EntryFuncFreq = 0;
        //float BBfreq=0;

        //getRealFreqOfCalledFuns(F,Level,BBfreq,EntryFuncFreq);
        // myfile.open ("FFCN.txt", std::ofstream::out | std::ofstream::app); 
        // myfile  << "\n|\n|\n";
        // myfile.close();

        //5b Print gv files for Caller -> Callee Relationship.

        // myfile.open (Function_Name +".gv", std::ofstream::out | std::ofstream::app);
        // myfile << "digraph \"" << Function_Name << "\" {" << "\n";
        // myfile << Function_Name << "[weight = 1, style = filled]" << "\n"; 
        // myfile.close();

        
        // printRealFreqOfCalledFunsGraphs(F, Function_Name);

        // myfile.open (Function_Name +".gv", std::ofstream::out | std::ofstream::app);
        // myfile << "}" << "\n";
        // myfile.close();



        //6
        //getInputFunction(F);
    
    }

  //  Get the Number of Arrays in A BB.
  //
  unsigned int GatherBBNumberOfArrays(BasicBlock *BB, std::vector<Value *> ArrayReferences) {

    unsigned int NumberOfArrays = 0;

    // Iterate inside the basic block.
    for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {

      // Load Info
      if(LoadInst *Load = dyn_cast<LoadInst>(&*BI)) {

        if (GetElementPtrInst *Source = dyn_cast<GetElementPtrInst>(&*Load->getOperand(0))) {

          // Load comes from an Array.
          if (Value *ArrayRef = Source->getPointerOperand()) {

            if (find_array(ArrayReferences, ArrayRef) == -1) {
              ArrayReferences.push_back(ArrayRef);
              NumberOfArrays++;
            }


          } // End of Array check.
        }
      }
    }

    return NumberOfArrays;
  }

  void getFunctionNumberOfLoopsandArrays (unsigned int &NumberOfLoops, 
     //  unsigned int &NumberOfArrays, Function *F ) {
     unsigned int &NumberOfArrays, Function *F, LoopInfo &LI, ScalarEvolution &SE ) {

    std::vector<Loop *> Loops;
    Loops.clear();
    std::vector<Value *> ArrayReferences;
    ArrayReferences.clear();

    for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

      BasicBlock *CurrentBlock = &*BB;

      // // Iterate inside the Loop.
      if (Loop *L = LI.getLoopFor(CurrentBlock)) {
        errs() << "\n     Num of Back Edges     : " << L->getNumBackEdges() << "\n";
        errs() << "     Loop Depth            : " << L->getLoopDepth() << "\n";
        errs() << "     Backedge Taken Count  : " << *SE.getBackedgeTakenCount(L) << '\n';
        errs() << "     Loop iterations       : " << SE.getSmallConstantTripCount(L) << "\n\n";

      NumberOfArrays += GatherBBNumberOfArrays(CurrentBlock, ArrayReferences); 

        if (find_loop(Loops, L) == -1 ){ 
            Loops.push_back(L);
            NumberOfLoops++;
          }
      }


    } // End of for

  }

     virtual unsigned int getDensityOfRegion(Region *R) {

      unsigned int DFGNodesRegion = 0;
      unsigned int GoodDFGNodesRegion = 0;
      unsigned int GoodnessRegion = 0;
      unsigned int DensityRegion = 0;


      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB)
        getGoodnessAndDensityOfRegionInBB(BB, &DFGNodesRegion, &GoodDFGNodesRegion, &GoodnessRegion);


      DensityRegion = static_cast<unsigned int> (GoodnessRegion / DFGNodesRegion) ; // Density of the Region.

      return DensityRegion;
    }

    // Get the Area extimation of a Region in LUTs.
    unsigned int getAreaofRegion(Region *R){

      unsigned int AreaOfRegion = 0;

      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB)
        AreaOfRegion += getAreaOfBB(BB);

      return AreaOfRegion;

    }

    // Helper Function to log Function names and teh included calls in a text file.
    //
    //
    void getFunctionNamesCalls(Function *F, int Level) {

      std::string Function_Name = F->getName();

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

       
                StringRef Indirect_Called_Name = SV->getName();
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

    // Log Function Names.
    //
    //  
    // void getFunctionNamesCalls(Function *F) {

    //   std::string Function_Name = F->getName();

    //   myfile.open ("FUN.txt", std::ofstream::out | std::ofstream::app); 
    //   myfile << Function_Name << "\n" ;
    //   myfile.close();

    // }

    // Interface Area Estimation - Master AXI Bus
    //
    //
    long int getInterfaceAreaFunction(Function *F) {
      
      long  int InputData = 0; // Bytes
      int arg_index=0;

      Function::ArgumentListType & Arg_List = F->getArgumentList();

      for (Function::arg_iterator AB = Arg_List.begin(), AE = Arg_List.end(); AB != AE; ++AB){

        llvm::Argument *Arg = &*AB;
        llvm::Type *Arg_Type = Arg->getType();

        errs() << "\n\n Argument : " << arg_index << "  --->  " << *AB << " -- " << *Arg_Type  << " --  \n ";

        if (isArray(Arg_Type) == true) {
          InputData += 700; // LUTs per M_AXI bus array.
           errs() << "\n\n Array found! : \n ";
        }
        
        arg_index++;
       }

  
       errs() << "\n\n Total Input Data Bytes :  " << InputData << " \n ";

      return InputData;
    }


    // Input from parameter List.
    //
    //
    long int getInputFunction(Function *F) {
      long  int InputData = 0; // Bits
      long int InputDataBytes = 0; // Bytes

      int arg_index=0;

      Function::ArgumentListType & Arg_List = F->getArgumentList();

      for (Function::arg_iterator AB = Arg_List.begin(), AE = Arg_List.end(); AB != AE; ++AB){

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

    // Get Input data by a Function.
    //
    //
    long int getInputDataFunction(Function *F) {
      long  int InputData = 0; // Bits
      long int InputDataBytes = 0; // Bytes

      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI(); 


      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
        BasicBlock *CurrentBlock = &*BB;

        float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(CurrentBlock).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
        int EntryFuncFreq = getEntryCount(F);
        float BBFreq = BBFreqFloat * static_cast<float>(EntryFuncFreq);

        errs() << " BB Name :  " << CurrentBlock->getName() << " \n ";
        errs() << " Entry Freq :  " << EntryFuncFreq << " \n ";
        errs() << " BB Freq :  " << BBFreqFloat << " \n ";
        errs() << " BB Total Freq :  " << BBFreq << " \n ";

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = CurrentBlock->begin(), BE = CurrentBlock->end(); BI != BE; ++BI) {

          // Load Info
          if(LoadInst *Load = dyn_cast<LoadInst>(&*BI)) {

            int InputLoad = Load->getType()->getPrimitiveSizeInBits();

            errs() << " InputLoad :  " << InputLoad << " \n ";

            InputData += InputLoad ;//* BBFreqFloat;

            errs() << " Input Data :  " << InputData << " \n\n ";

            // Array check!
            //
            if (GetElementPtrInst *Source = dyn_cast<GetElementPtrInst>(&*Load->getOperand(0))) {

                // Load comes from an Array.
                if (Value *ArrayRef = Source->getPointerOperand()) {

                  std::string ArrayRefName = ArrayRef->getName();

                  errs() << " Input Array Data :  " << ArrayRefName  << " " <<InputLoad <<  " X " << static_cast<int>(BBFreqFloat)  << " \n\n ";


                } // End of Array check.
              } // End of If GetElementPtrInst.
            //
            // End of Array Check!

          }
        }           
      }
      InputDataBytes = InputData/8;

      errs() << "\n\n Total Input Data Bytes :  " << InputDataBytes << " \n ";

      return InputDataBytes;
    }

     // Get the Delay Estimation for the Function -- Loop Unrolled --
    //
    //
    long int getHWCostOfFunctionLoopUnrolled(Function *F) {

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
      
              // if(count < find_bb(worklist, Succ) ) {  /// !!! CHANGED THIS !!!!!!
              if(count != find_bb(worklist, Succ) ) { // Changed it so that we can include all Nodes (Except for the ones that have a backedge to themselves)

                predecessor_bb.push_back(BB); // Populate send_node vector
                successor_bb.push_back(Succ); // Populate receive_node vector
              }
            }
          }
        }

        // errs() << "\n\n"  ;
        // for (int i=0; i< predecessor_bb.size(); i++) {
        //   errs() << " BB Edges in the Function : " << predecessor_bb[i]->getName() << "  --->     " <<  successor_bb[i]->getName() << "\n"; // My debugging Info!
        // }


        // BEGIN OF WORK IN PROGRESS

        // Converting a CFG with backedges to a DAG (Data Acyclic Graph)
        // Necessary in case BBs are not listed in order, as exppected. (In Function most likely this is not necessary.) 

        // WORKING ON THAT!!!
        int pos_successor = 0;
        for (std::vector<BasicBlock *>::iterator succ_iter = successor_bb.begin(); succ_iter != successor_bb.end(); ++succ_iter, pos_successor++) {
            BasicBlock *successor = *succ_iter;

            //errs() << "Counter : " << pos_successor << "\n" ;


            if (find_bb(worklist, successor) == -1) {                       // Succesor is *not* in our worklist.

              successor_bb.erase(successor_bb.begin() + pos_successor);             // deleting the edge
              predecessor_bb.erase(predecessor_bb.begin() + pos_successor);        //  deleting the edge
              succ_iter--;          // Be careful!!!
              pos_successor--;       // Be careful!!!
              continue;
            }

            int succ_pos_in_pred_list = find_bb(predecessor_bb, successor);
            //errs() << "  Successor position in pred list : " << succ_pos_in_pred_list << "\n";

            if ( succ_pos_in_pred_list > pos_successor || succ_pos_in_pred_list == -1)        // Maybe put >= instead of >
              continue;


            int new_position = find_bb(successor_bb, predecessor_bb[pos_successor]);  


             // errs() << " Begin" << "\n";
             // errs() << " new_position " << new_position << "\n";
             // errs() << " position successor " << pos_successor << "\n";    


            while (new_position < pos_successor && new_position !=-1) {

             // errs() << " new_position : " << new_position << "\n";
             // errs() << " position successor : " << pos_successor << "\n";

              //int new_pos = find_bb(successor_bb, predecessor_bb[pos_successor]);

              if (predecessor_bb[new_position] == successor) {

                successor_bb.erase(successor_bb.begin() + pos_successor);             // deleting the edge
                predecessor_bb.erase(predecessor_bb.begin() + pos_successor);        //  deleting the edge
                succ_iter--;            // Be careful!!!
                pos_successor--;         // Be careful!!!
                new_position = pos_successor; // End the search. Maybe insert a break.
              }

              else
                new_position = find_bb(successor_bb, predecessor_bb[new_position]); 

            }


        }

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
  
            //long int hw_cost;  // Remove it when you are done!

            while(BottomNodes.size()>0) {  
             
                CurrentNode = BottomNodes[0];
     
               // errs() << " \n\nCurrentNode " << CurrentNode->getName() << "\n";
               // errs() << " \nBottom List Size " << BottomNodes.size() << "\n";
                
                
                while (find_bb(successor_bb, CurrentNode) >=0) {

                  position = find_bb(successor_bb, CurrentNode); 
                  position_bottom_nodes = find_bb(BottomNodes, CurrentNode); // Should be zero.


                  // errs()  << CurrentNode->getName() << " " << HWCostPath[find_bb(worklist, CurrentNode)] ;
                  // errs() << " --> " << predecessor_bb[position]->getName() << " " << HWCostPath[find_bb(worklist, predecessor_bb[position])] << "\n";
                  // errs() << "Original \n" << CurrentNode->getName() << " " << HWCostPath[find_bb(worklist, CurrentNode)] ;
                  // errs() << " --> " << predecessor_bb[position]->getName() << " " << HWCostFunction[find_bb(worklist, predecessor_bb[position])] << "\n";


                  HWCostPath[find_bb(worklist, predecessor_bb[position])] = std::max( HWCostPath[find_bb(worklist, predecessor_bb[position])], HWCostFunction[find_bb(worklist, predecessor_bb[position])] + HWCostPath[find_bb(worklist, CurrentNode)] );
                  //errs() << " Updated " << predecessor_bb[position]->getName() << " " << HWCostPath[find_bb(worklist, predecessor_bb[position])];
                  
                  BasicBlock *Predecessor = predecessor_bb[position];

                  successor_bb.erase(successor_bb.begin() + position);            // deleting the last edge
                  predecessor_bb.erase(predecessor_bb.begin() + position);        //  deleting the last edge
                  
                  //errs() << "\nPredecessor in pred list  " <<  Predecessor->getName() << " " << find_bb(predecessor_bb, Predecessor) << "\n";

                  if (find_bb(predecessor_bb, Predecessor) == -1)
                    BottomNodes.push_back(Predecessor);
                  

                }

                //BottomNodes.erase(BottomNodes.begin()+ position_bottom_nodes);
                BottomNodes.erase(BottomNodes.begin());
                //errs() <<  "\n";
                
                //errs() << " Bottom Nodes in list : \n" ;
                // for (int i=0; i< BottomNodes.size(); i++) {
                //   errs() << " Node : " << BottomNodes[i]->getName()  << "\n"; // My debugging Info!
                // }

            }
          }
        }

        HardwareCost       = get_max_long_int(HWCostPath);                // Total Cycles spent on HW.
      }

      // In case that the Function has only one BB.
      // else if (worklist.size() == 1) {
      else {
        // DelayOfFunction      = getDelayOfBB(worklist[0]) * BBFreqPerIter[0];
        // DelayOfFunctionTotal = getDelayOfBB(worklist[0]) * BBFreqTotal[0];
        HardwareCost   = ceil( ( getDelayOfBB(worklist[0]) ) / NSECS_PER_CYCLE ) * BBFreqTotal[0];
      }

      return HardwareCost;
    }


  #ifdef HW_COST_AVG
    // Get the Delay Estimation for the Function. 
    // Sum of all BBs HW cost times execution frequency. (Average strategy)
    //
    long int getHWCostOfFunction(Function *F) {

      long int HardwareCost =0;
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI(); 


      // Populate worklist with Function's Basic Blocks and their respective BB's Frequencies. Both Per Iteration and Total.
      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));         
        HardwareCost   += ceil( ( getDelayOfBB(&*BB) ) / NSECS_PER_CYCLE ) * BBFreqFloat ;

      }

      return HardwareCost;
    }


    #else

    // Get the Delay Estimation for the Function. - Critical Path
    //
    //
    long int getHWCostOfFunction(Function *F) {

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
      
              // if(count < find_bb(worklist, Succ) ) {  /// !!! CHANGED THIS !!!!!!
              if(count != find_bb(worklist, Succ) ) { // Changed it so that we can include all Nodes (Except for the ones that have a backedge to themselves)

                predecessor_bb.push_back(BB); // Populate send_node vector
                successor_bb.push_back(Succ); // Populate receive_node vector
              }
            }
          }
        }

        // errs() << "\n\n"  ;
        // for (int i=0; i< predecessor_bb.size(); i++) {
        //   errs() << " BB Edges in the Function : " << predecessor_bb[i]->getName() << "  --->     " <<  successor_bb[i]->getName() << "\n"; // My debugging Info!
        // }


        // BEGIN OF WORK IN PROGRESS

        // Converting a CFG with backedges to a DAG (Data Acyclic Graph)
        // Necessary in case BBs are not listed in order, as exppected. (In Function most likely this is not necessary.) 

        // WORKING ON THAT!!!
        int pos_successor = 0;
        for (std::vector<BasicBlock *>::iterator succ_iter = successor_bb.begin(); succ_iter != successor_bb.end(); ++succ_iter, pos_successor++) {
            BasicBlock *successor = *succ_iter;

            //errs() << "Counter : " << pos_successor << "\n" ;


            if (find_bb(worklist, successor) == -1) {                       // Succesor is *not* in our worklist.

              successor_bb.erase(successor_bb.begin() + pos_successor);             // deleting the edge
              predecessor_bb.erase(predecessor_bb.begin() + pos_successor);        //  deleting the edge
              succ_iter--;          // Be careful!!!
              pos_successor--;       // Be careful!!!
              continue;
            }

            int succ_pos_in_pred_list = find_bb(predecessor_bb, successor);
            //errs() << "  Successor position in pred list : " << succ_pos_in_pred_list << "\n";

            if ( succ_pos_in_pred_list > pos_successor || succ_pos_in_pred_list == -1)        // Maybe put >= instead of >
              continue;


            int new_position = find_bb(successor_bb, predecessor_bb[pos_successor]);  


             // errs() << " Begin" << "\n";
             // errs() << " new_position " << new_position << "\n";
             // errs() << " position successor " << pos_successor << "\n";    


            while (new_position < pos_successor && new_position !=-1) {

             // errs() << " new_position : " << new_position << "\n";
             // errs() << " position successor : " << pos_successor << "\n";

              //int new_pos = find_bb(successor_bb, predecessor_bb[pos_successor]);

              if (predecessor_bb[new_position] == successor) {

                successor_bb.erase(successor_bb.begin() + pos_successor);             // deleting the edge
                predecessor_bb.erase(predecessor_bb.begin() + pos_successor);        //  deleting the edge
                succ_iter--;            // Be careful!!!
                pos_successor--;         // Be careful!!!
                new_position = pos_successor; // End the search. Maybe insert a break.
              }

              else
                new_position = find_bb(successor_bb, predecessor_bb[new_position]); 

            }


        }

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
  
            //long int hw_cost;  // Remove it when you are done!

            while(BottomNodes.size()>0) {  
             
                CurrentNode = BottomNodes[0];
     
               // errs() << " \n\nCurrentNode " << CurrentNode->getName() << "\n";
               // errs() << " \nBottom List Size " << BottomNodes.size() << "\n";
                
                
                while (find_bb(successor_bb, CurrentNode) >=0) {

                  position = find_bb(successor_bb, CurrentNode); 
                  position_bottom_nodes = find_bb(BottomNodes, CurrentNode); // Should be zero.


                  // errs()  << CurrentNode->getName() << " " << HWCostPath[find_bb(worklist, CurrentNode)] ;
                  // errs() << " --> " << predecessor_bb[position]->getName() << " " << HWCostPath[find_bb(worklist, predecessor_bb[position])] << "\n";
                  // errs() << "Original \n" << CurrentNode->getName() << " " << HWCostPath[find_bb(worklist, CurrentNode)] ;
                  // errs() << " --> " << predecessor_bb[position]->getName() << " " << HWCostFunction[find_bb(worklist, predecessor_bb[position])] << "\n";


                  HWCostPath[find_bb(worklist, predecessor_bb[position])] = std::max( HWCostPath[find_bb(worklist, predecessor_bb[position])], HWCostFunction[find_bb(worklist, predecessor_bb[position])] + HWCostPath[find_bb(worklist, CurrentNode)] );
                  //errs() << " Updated " << predecessor_bb[position]->getName() << " " << HWCostPath[find_bb(worklist, predecessor_bb[position])];
                  
                  BasicBlock *Predecessor = predecessor_bb[position];

                  successor_bb.erase(successor_bb.begin() + position);            // deleting the last edge
                  predecessor_bb.erase(predecessor_bb.begin() + position);        //  deleting the last edge
                  
                  //errs() << "\nPredecessor in pred list  " <<  Predecessor->getName() << " " << find_bb(predecessor_bb, Predecessor) << "\n";

                  if (find_bb(predecessor_bb, Predecessor) == -1)
                    BottomNodes.push_back(Predecessor);
                  

                }

                //BottomNodes.erase(BottomNodes.begin()+ position_bottom_nodes);
                BottomNodes.erase(BottomNodes.begin());
                //errs() <<  "\n";
                
                //errs() << " Bottom Nodes in list : \n" ;
                // for (int i=0; i< BottomNodes.size(); i++) {
                //   errs() << " Node : " << BottomNodes[i]->getName()  << "\n"; // My debugging Info!
                // }

            }
          }
        }

        HardwareCost       = get_max_long_int(HWCostPath);                // Total Cycles spent on HW.
      }

      // In case that the Function has only one BB.
      // else if (worklist.size() == 1) {
      else {
        // DelayOfFunction      = getDelayOfBB(worklist[0]) * BBFreqPerIter[0];
        // DelayOfFunctionTotal = getDelayOfBB(worklist[0]) * BBFreqTotal[0];
        HardwareCost   = ceil( ( getDelayOfBB(worklist[0]) ) / NSECS_PER_CYCLE ) * BBFreqTotal[0];
      }

      return HardwareCost;
    }

    #endif

    // Get the Delay Estimation for the Region.
    //
    //
    long int getHWCostOfRegion(Region *R) {

      float DelayOfRegion, DelayOfRegionTotal = 0;
      long int HardwareCost =0;
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI(); 
      std::vector<BasicBlock *> worklist, predecessor_bb, successor_bb;
      std::vector<float> BBFreqPerIter;
      std::vector<float> BBFreqTotal;
      std::vector<float> DelayRegionPathsPerIter, DelayRegionPathsTotal;
      std::vector<long int>   HWCostPath, HWCostBB;

      // Clear vectors.
      worklist.clear();
      predecessor_bb.clear();
      successor_bb.clear();
      BBFreqPerIter.clear();
      BBFreqTotal.clear();
      DelayRegionPathsPerIter.clear();
      DelayRegionPathsTotal.clear();
      HWCostPath.clear();
      HWCostBB.clear();

      // Populate worklist with Region's Basic Blocks and their respective BB's Frequencies. Both Per Iteration and Total.
      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {

        float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
        int EntryFuncFreq = getEntryCount(R->block_begin()->getParent());
        float BBFreq = BBFreqFloat * static_cast<float>(EntryFuncFreq);
        
        worklist.push_back(*BB);
        BBFreqPerIter.push_back(BBFreqFloat);
        BBFreqTotal.push_back(BBFreq);
        HWCostBB.push_back(ceil( ( getDelayOfBB(*BB) ) / NSECS_PER_CYCLE ) * BBFreqTotal[find_bb(worklist, *BB)] ); // HW Cost for each BB (Cyclified) 
        HWCostPath.push_back(ceil( ( getDelayOfBB(*BB) ) / NSECS_PER_CYCLE ) * BBFreqTotal[find_bb(worklist, *BB)] );

    
      }

      errs() << "\n";
      for (int i=0; i< worklist.size(); i++) {
        errs() << " BBs in Region " << worklist[i]->getName() << " Freq per Iter " << BBFreqPerIter[i] <<  
          " Freq Total " << BBFreqTotal[i] << " HW Cost BB " << HWCostBB[i] << "\n"; // My debugging Info!    
      }

      // Region has more than one BBs.
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
      
              // if(count < find_bb(worklist, Succ) ) {  /// !!! CHANGED THIS !!!!!!
              if(count != find_bb(worklist, Succ) ) { 
                predecessor_bb.push_back(BB); // Populate send_node vector
                successor_bb.push_back(Succ); // Populate receive_node vector
              }
            }
          }
        }

        errs() << "\n\n"  ;
        for (int i=0; i< predecessor_bb.size(); i++) {
          errs() << " BB Edges in the Region : " << predecessor_bb[i]->getName() << "  --->     " <<  successor_bb[i]->getName() << "\n"; // My debugging Info!
        }
        errs() << "\n" ;

        
        // BEGIN OF WORK IN PROGRESS

        // WORKING ON THAT!!!
        int pos_successor = 0;
        for (std::vector<BasicBlock *>::iterator succ_iter = successor_bb.begin(); succ_iter != successor_bb.end(); ++succ_iter, pos_successor++) {
            BasicBlock *successor = *succ_iter;

            errs() << "Counter : " << pos_successor << "\n" ;


            if (find_bb(worklist, successor) == -1) {                       // Succesor is *not* in our worklist.

              successor_bb.erase(successor_bb.begin() + pos_successor);             // deleting the edge
              predecessor_bb.erase(predecessor_bb.begin() + pos_successor);        //  deleting the edge
              succ_iter--;          // Be careful!!!
              pos_successor--;       // Be careful!!!
              continue;
            }

            int succ_pos_in_pred_list = find_bb(predecessor_bb, successor);
            errs() << "  Successor position in pred list : " << succ_pos_in_pred_list << "\n";

            if ( succ_pos_in_pred_list > pos_successor || succ_pos_in_pred_list == -1)        // Maybe put >= instead of >
              continue;


            int new_position = find_bb(successor_bb, predecessor_bb[pos_successor]);  


              errs() << " Begin" << "\n";
              errs() << " new_position " << new_position << "\n";
              errs() << " position successor " << pos_successor << "\n";    


            while (new_position < pos_successor && new_position !=-1) {

              errs() << " new_position : " << new_position << "\n";
              errs() << " position successor : " << pos_successor << "\n";

              //int new_pos = find_bb(successor_bb, predecessor_bb[pos_successor]);

              if (predecessor_bb[new_position] == successor) {

                successor_bb.erase(successor_bb.begin() + pos_successor);             // deleting the edge
                predecessor_bb.erase(predecessor_bb.begin() + pos_successor);        //  deleting the edge
                succ_iter--;            // Be careful!!!
                pos_successor--;         // Be careful!!!
                new_position = pos_successor; // End the search. Maybe insert a break.
              }

              else
                new_position = find_bb(successor_bb, predecessor_bb[new_position]); 

            }


        }

        // END OF WORK IN PROGRESS

        errs() << "\n\n   Updated Edges \n"  ;
        for (int i=0; i< predecessor_bb.size(); i++) {
          errs() << " BB Edges in the Region : " << predecessor_bb[i]->getName() << "  --->     " <<  successor_bb[i]->getName() << "\n"; // My debugging Info!
        }
        errs() << "\n" ;


        // Critical Path Estimation. 
        //       
        // 
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
  
            long int hw_cost;  // Remove it when you are done!

            while(BottomNodes.size()>0) {  
             
                CurrentNode = BottomNodes[0];
     
                errs() << " \n\nCurrentNode " << CurrentNode->getName() << "\n";
                errs() << " \nBottom List Size " << BottomNodes.size() << "\n";
                
                
                while (find_bb(successor_bb, CurrentNode) >=0) {

                  position = find_bb(successor_bb, CurrentNode); 
                  position_bottom_nodes = find_bb(BottomNodes, CurrentNode); // Should be zero.


                  errs()  << CurrentNode->getName() << " " << HWCostPath[find_bb(worklist, CurrentNode)] ;
                  errs() << " --> " << predecessor_bb[position]->getName() << " " << HWCostPath[find_bb(worklist, predecessor_bb[position])] << "\n";
                  errs() << "Original \n" << CurrentNode->getName() << " " << HWCostPath[find_bb(worklist, CurrentNode)] ;
                  errs() << " --> " << predecessor_bb[position]->getName() << " " << HWCostBB[find_bb(worklist, predecessor_bb[position])] << "\n";


                  HWCostPath[find_bb(worklist, predecessor_bb[position])] = std::max( HWCostPath[find_bb(worklist, predecessor_bb[position])], HWCostBB[find_bb(worklist, predecessor_bb[position])] + HWCostPath[find_bb(worklist, CurrentNode)] );
                  errs() << " Updated " << predecessor_bb[position]->getName() << " " << HWCostPath[find_bb(worklist, predecessor_bb[position])];
                  
                  BasicBlock *Predecessor = predecessor_bb[position];

                  successor_bb.erase(successor_bb.begin() + position);            // deleting the last edge
                  predecessor_bb.erase(predecessor_bb.begin() + position);        //  deleting the last edge
                  
                  errs() << "\nPredecessor in pred list  " <<  Predecessor->getName() << " " << find_bb(predecessor_bb, Predecessor) << "\n";

                  if (find_bb(predecessor_bb, Predecessor) == -1)
                    BottomNodes.push_back(Predecessor);
                  

                }

                //BottomNodes.erase(BottomNodes.begin()+ position_bottom_nodes);
                BottomNodes.erase(BottomNodes.begin());
                errs() <<  "\n";
                
                errs() << " Bottom Nodes in list : \n" ;
                for (int i=0; i< BottomNodes.size(); i++) {
                  errs() << " Node : " << BottomNodes[i]->getName()  << "\n"; // My debugging Info!
                }

            }
          }
        }

        HardwareCost       = get_max_long_int(HWCostPath);                // Total Cycles spent on HW.
      }

      // In case that the Region has only one BB.
      // else if (worklist.size() == 1) {
      else {
        //DelayOfRegion      = getDelayOfBB(worklist[0]) * BBFreqPerIter[0];
        //DelayOfRegionTotal = getDelayOfBB(worklist[0]) * BBFreqTotal[0];
        HardwareCost       = ceil( ( getDelayOfBB(worklist[0]) ) / NSECS_PER_CYCLE ) * BBFreqTotal[0];
      }



      return HardwareCost;
    }

    // Get Real Frequency of each candidate's Calls to Functions. 
    // print Graphs!
    //
    void printRealFreqOfCalledFunsGraphs(Function *F, std::string TopFunName) {

      std::string Function_Name = F->getName();

      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
         
           if(CallInst *Call = dyn_cast<CallInst>(BI)) {

            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {

                Function *Calee = Call->getCalledFunction();
                std::string CaleeName = Calee->getName();
                
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

       
                std::string Indirect_Called_Name = SV->getName();

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

    // Get Real Frequency of each candidate's Calls to Functions.
    //
    //
    void getRealFreqOfCalledFuns(Function *F, int Level, float BBfreq, float EntryFuncFreq) {

      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
      std::string Function_Name = F->getName();
      float CalleeFreq;

      //int GlobalFuncFreq = getEntryCount(F); // Testing!

      if (Level==0)
        CalleeFreq = getEntryCount(F);
      else
          CalleeFreq = static_cast<int> (BBfreq * EntryFuncFreq);

      //CalleeFreq = static_cast<float> (CalleeFreq /2); // Fixing frequency anomaly.

      // if (F->getName() == "write_luma")  // Testing!
      //   globalCounter += CalleeFreq/2;

      myfile.open ("FFCN.txt", std::ofstream::out | std::ofstream::app);     
      myfile << "L"<< Level << " :\t" << Function_Name << "\t" << CalleeFreq/2 ; //GlobalFuncFreq  << "\t";
      myfile.close();


      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
         
           if(CallInst *Call = dyn_cast<CallInst>(BI)) {

            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {

                Function *Calee = Call->getCalledFunction();
                float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
                Level++;
                getRealFreqOfCalledFuns(Calee, Level, BBFreqFloat, CalleeFreq);
                Level--;         

              }
            }
            
            else{

              Value* V=Call->getCalledValue();
              Value* SV = V->stripPointerCasts();

              if (!isIndirectSystemCall(SV) ) {

       
                StringRef Indirect_Called_Name = SV->getName();
                //errs() <<"\t" << F->getName() << "\t -->\t" << Indirect_Called_Name << "\n";

                int fun_index = find_function_name(Function_Names_list, Indirect_Called_Name);

                if (fun_index >=0) { // Is it in our list?

                  errs() << "Indirect Call in Our list! " << "\n";

                  Function *Calee = Function_list[fun_index];
                  float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
                  Level++;
                  getRealFreqOfCalledFuns(Calee, Level, BBFreqFloat, CalleeFreq);
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

    // Record the Indexes of the Function Calls within each Function.
    //
    //
    void getIndexesOfCalledFunctions(Function *F) {

      // std::string Function_Name = F->getName();
      // int F_index = find_function(Function_list, F);


      // myfile.open ("FCI.txt", std::ofstream::out | std::ofstream::app); 
      // myfile << Function_Name << " " << F_index << " " ;
      // myfile.close();


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

            // else{

            //   Value* V=Call->getCalledValue();
            //   Value* SV = V->stripPointerCasts();

            //   if (!isIndirectSystemCall(SV) ) {

       
            //     StringRef Indirect_Called_Name = SV->getName();
            //     //errs() <<"\t" << F->getName() << "\t -->\t" << Indirect_Called_Name << "\n";

            //     int fun_index = find_function_name(Function_Names_list, Indirect_Called_Name);

            //     if (fun_index >=0) { // Is it in our list?

            //     myfile.open ("FCI.txt", std::ofstream::out | std::ofstream::app); 
            //     myfile << fun_index <<" ";
            //     myfile.close();

            //     }

            //   }
            // } // End of Else branch.
          } // End of IF
        } // End of For - BB Iterator
      }

      // myfile.open ("FCI.txt", std::ofstream::out | std::ofstream::app); 
      // myfile << "\n";
      // myfile.close();   
    }

    // Log the HW Cost estimation of the Super Function -- Loop Unrolled --
    // Detect calls to other functioms and add their HW cost as well.
    //
    long int logHWCostOfSuperFunctionLoopUnrolled(Function *F,  int CurrentLevel) {

      std::ifstream hw_file;
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
      int CalleeFreq =0;
      int LevelSuperFunction=0;
      unsigned long long int HWCostSuperFunction=0;


      std::string Function_Name = F->getName();  
      int EntryCount = getEntryCount(F);
      //CalleeFreq = EntryCount;
  
      HWCostSuperFunction = getHWCostOfFunctionLoopUnrolled(F); //* CalleeFreq ;
      errs() << "--HW Cost1\t" << HWCostSuperFunction  << " " << getHWCostOfFunctionLoopUnrolled(F)  << CalleeFreq << " " << F->getName() << "\n\n";


      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
         
           if(CallInst *Call = dyn_cast<CallInst>(BI)) {

            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {


                if (LevelSuperFunction == 0)
                  LevelSuperFunction = CurrentLevel; // Assign the right Level of Calling Functions


                float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
                CalleeFreq = static_cast<int> (BBFreqFloat * 1);

                Function *Calee = Call->getCalledFunction();
                std::string Calee_Name = Calee->getName();
                unsigned long long int HWCostCalee = 0;
                double CalleeFreqRatio = 1;

                long int hw_latency;
                int entry_count;
                std::string fun_name;
                bool found = false;
                
                for (int i = CurrentLevel-1; i>=0; i--) {
                              
                  hw_file.open("HW_unrolled_" + std::to_string(i) + ".txt");
                  if(hw_file.fail()) { // checks to see if file opended 
                    errs() << "error" << "\n"; 
                    return 1; // no point continuing if the file didn't open...
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
                        //CalleeFreqRatio = 1;
                        fractpart = modf (CalleeFreqRatio , &intpart);
                        CalleeFreqRatio = fractpart > 1 - fractpart ? fractpart : 1 - fractpart;
                      }

                      if (Function_Name == "main" || Function_Name == "decode_main"  ) // Only for main
                        CalleeFreqRatio=1;


                      HWCostCalee  =  hw_latency * CalleeFreqRatio;
       
                       errs()  << "---HW Cost2\t" << HWCostSuperFunction  << " " << HWCostCalee << " " << hw_latency
                   << " " << CalleeFreq << " " << entry_count << " " << F->getName() << " " << Calee_Name << " CalleeFreqRatio " 
                    << format("%.8f",CalleeFreqRatio) <<  "\n\n" ;
                      // errs() << "HW Cost Missing! \t" << fun_index << " " << Calee_Name << " " 
                      //   << Function_HW_Cost_list[fun_index] << " file HW Cost " << hw_latency << " Freq : " << CalleeFreq << "\n";

                    }
                  } // End of while    

                   hw_file.close();

                   if (found)
                    break; // break when read from the top Level File. (Maximum Latency)

                } // End of for

                HWCostSuperFunction += HWCostCalee; // Final Computation of HWCostSuperFunction

              } // End of If - System Call
            }
           }    // End of Call Instruction
         }      // End of For - BB Iterator

        } // End of For - Function Iterator

      if (LevelSuperFunction == CurrentLevel &&  HWCostSuperFunction>0){
        errs() << "Writing to file " << CurrentLevel << "\n";

        myfile.open ("HW_unrolled_" + std::to_string(CurrentLevel) + ".txt", std::ofstream::out | std::ofstream::app); 
        myfile << Function_Name << "\t" 
            << HWCostSuperFunction << "\t"
            << EntryCount << "\t"
            <<"\n";
        myfile.close();    

        errs() << "------HW Cost 3\t" << HWCostSuperFunction<< " " << " " << CalleeFreq << " " << F->getName() << "\n\n";
      }

      return HWCostSuperFunction;
    }

    // Log the HW Cost estimation of the Super Function
    // Detect calls to other functioms and add their HW cost as well.
    //
    long int logHWCostOfSuperFunction(Function *F,  int CurrentLevel) {

      std::ifstream hw_file;
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
      int CalleeFreq =0;
      int LevelSuperFunction=0;
      unsigned long long int HWCostSuperFunction=0;


      std::string Function_Name = F->getName();  
      int EntryCount = getEntryCount(F);
      CalleeFreq = EntryCount;                  // Change 31/3/19
  
      HWCostSuperFunction = getHWCostOfFunction(F); //* CalleeFreq ;
      errs() << "--HW Cost1\t" << HWCostSuperFunction  << " " << getHWCostOfFunction(F)  << CalleeFreq << " " << F->getName() << "\n\n";


      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
         
           if(CallInst *Call = dyn_cast<CallInst>(BI)) {

            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {


                if (LevelSuperFunction == 0)
                  LevelSuperFunction = CurrentLevel; // Assign the right Level of Calling Functions


                float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
                //CalleeFreq = static_cast<int> (BBFreqFloat * 1);
                CalleeFreq = static_cast<int> (BBFreqFloat * EntryCount); // Change 31/3/19

                Function *Calee = Call->getCalledFunction();
                std::string Calee_Name = Calee->getName();
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
                    return 1; // no point continuing if the file didn't open...
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
                        //CalleeFreqRatio = 1;
                        fractpart = modf (CalleeFreqRatio , &intpart);
                        CalleeFreqRatio = fractpart > 1 - fractpart ? fractpart : 1 - fractpart;
                      }

                      if (Function_Name == "main" || Function_Name == "decode_main"  ) // Only for main
                        CalleeFreqRatio=1;


                      HWCostCalee  =  hw_latency * CalleeFreqRatio;
       
                       errs()  << "---HW Cost2\t" << HWCostSuperFunction  << " " << HWCostCalee << " " << hw_latency
                   << " " << CalleeFreq << " " << entry_count << " " << F->getName() << " " << Calee_Name << " CalleeFreqRatio " 
                    << format("%.8f",CalleeFreqRatio) <<  "\n\n" ;
                      // errs() << "HW Cost Missing! \t" << fun_index << " " << Calee_Name << " " 
                      //   << Function_HW_Cost_list[fun_index] << " file HW Cost " << hw_latency << " Freq : " << CalleeFreq << "\n";

                    }
                  } // End of while    

                   hw_file.close();

                   if (found)
                    break; // break when read from the top Level File. (Maximum Latency)

                } // End of for

                HWCostSuperFunction += HWCostCalee; // Final Computation of HWCostSuperFunction

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

        errs() << "------HW Cost 3\t" << HWCostSuperFunction<< " " << " " << CalleeFreq << " " << F->getName() << "\n\n";
      }

      return HWCostSuperFunction;
    }

    // Get the HW Cost estimation of the Super Function
    // Detect calls to other functioms and add their HW cost as well.
    //
    long int getHWCostOfSuperFunction(Function *F,  int Level, float BBfreq, int EntryFuncFreq) {

      long int HWCostSuperFunction = 0;

      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
      int CalleeFreq;

      if (Level==0 || BBfreq == 0)
        CalleeFreq = getEntryCount(F);      
      else
          CalleeFreq = static_cast<int> (BBfreq * EntryFuncFreq);

      HWCostSuperFunction += getHWCostOfFunction(F) * (CalleeFreq/2); // Fix!

      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
         
           if(CallInst *Call = dyn_cast<CallInst>(BI)) {

            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {

                Function *Calee = Call->getCalledFunction();
                int fun_index = find_function(Function_list, Calee);

                if (fun_index >=0)                 
                  HWCostSuperFunction  += Function_HW_Cost_list[fun_index] * (CalleeFreq/2); // Fix!
                //errs() << "HW Cost : " << Calee->getName() << "\t"  << Function_HW_Cost_list[fun_index] << "\t" << HWCostSuperFunction << "\n";
                
                float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));

                Level++;
                HWCostSuperFunction  += getHWCostOfSuperFunction(Calee, Level, BBFreqFloat, CalleeFreq); // It is Zero in most calls within a Function.
                Level--;


                


              }
            }
     
            
            else{

              Value* V=Call->getCalledValue();
              Value* SV = V->stripPointerCasts();

              if (!isIndirectSystemCall(SV) ) {

       
                StringRef Indirect_Called_Name = SV->getName();
                //errs() <<"\t" << F->getName() << "\t -->\t" << Indirect_Called_Name << "\n";

                int fun_index = find_function_name(Function_Names_list, Indirect_Called_Name);

                if (fun_index >=0) { // Is it in our list?


                  errs() << "Indirect Call in Our list! " << "\n";

                  Function *Calee = Function_list[fun_index];
                  HWCostSuperFunction  += Function_HW_Cost_list[fun_index] * (CalleeFreq/2); // Fix!

                  float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));

                  Level++;
                  HWCostSuperFunction  += getHWCostOfSuperFunction(Calee, Level, BBFreqFloat, CalleeFreq); // It is Zero in most calls within a Function.
                  Level--;
           
                  //errs() << "HW Cost 2 : " << "\t"  << HWCostSuperFunction << "\n";
                }
                else
                  errs() << "Indirect Call *not* in Our list! \t" << Indirect_Called_Name << "\n";

              }
            } // End of Else branch.
           
           } // End of Call Instruction
         } // End of For - BB Iterator

        } // End of For - Function Iterator

      return HWCostSuperFunction;
    }

    void getMissingFunctions(Function *F) {

      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
         
           if(CallInst *Call = dyn_cast<CallInst>(BI)) {

            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {

                Function *Calee = Call->getCalledFunction();
                int fun_index = find_function_name(Function_Names_list, Calee->getName());
          
                if (fun_index==-1) {
                  Function_missing_list.push_back(Calee);  
                  errs() << "Missing! :  " << Calee->getName() <<"\n";
                }
              }
            }
          
           else {

              Value* V=Call->getCalledValue();
              Value* SV = V->stripPointerCasts();

              if (!isIndirectSystemCall(SV) ) {
                StringRef Indirect_Called_Name = SV->getName();
                int fun_index = find_function_name(Function_Names_list, Indirect_Called_Name);

                if (fun_index==-1) {
                  Function_missing_list_names.push_back(Indirect_Called_Name);  
                  errs() << "Missing name! :  " << Indirect_Called_Name <<"\n";
                }

                  

              }
            } // End of Else

          }

        }
      }
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

      std::string Function_Name = F->getName();  
      EntryCount = getEntryCount(F);
      CalleeFreq = EntryCount;

      SWCostSuperFunction = getSWCostOfFunction(F) * (CalleeFreq);
      errs() << "--SW Cost1\t" << SWCostSuperFunction<< " " << getSWCostOfFunction(F) << " " << CalleeFreq << " " << F->getName() << "\n\n";


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
                std::string Calee_Name = Calee->getName();
                int fun_index = find_function(Function_list, Calee);
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
                      //CalleeFreqRatio = 1;
                      fractpart = modf (CalleeFreqRatio , &intpart); // Receive the biggest part of the complimentary to one fractal part.
                      CalleeFreqRatio = fractpart > 1 - fractpart ? fractpart : 1 - fractpart; 
                    }

                    if (Function_Name == "main" || Function_Name == "decode_main") // Only for main and decode_main
                      CalleeFreqRatio=1;

                    SWCostCalee  =  sw_latecy * CalleeFreqRatio;
            
                     errs()  << "---SW Cost2\t" << SWCostSuperFunction  << " " << SWCostCalee << " " << sw_latecy
                       << " " << CalleeFreq << " " << entry_count << " " << F->getName() << " " << Calee_Name << " CalleeFreqRatio " 
                        << format("%.8f",CalleeFreqRatio) <<  "\n\n" ;
                  }
                } // End of while    

                 sw_file.close();

                 if (found){
                  errs() << "Found you!" << "\n";
                  break; // break when read from the top Level File. (Maximum Latency)
                 }
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

        errs() << "------SW Cost 3\t" << SWCostSuperFunction  << " " << CalleeFreq << " " << F->getName() << "\n\n";
      }

      return SWCostSuperFunction;
    }


    // Get the SW Cost estimation of the Super Function
    // Detect calls to other functioms and add their SW cost as well.
    //
    unsigned long long int getSWCostOfSuperFunction(Function *F, int Level, float BBfreq, int EntryFuncFreq, unsigned long long int SWCostSuperFunction) {


      std::ifstream sw_file ;
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
      int CalleeFreq;
      unsigned long long int SWCostSuperFunctionLocal=0;
   

        if (Level > 0 )
          return SWCostSuperFunction;


      if (Level==0 || BBfreq == 0)
        CalleeFreq = getEntryCount(F);      
      else
          CalleeFreq = static_cast<int> (BBfreq * EntryFuncFreq);

      //int CalleeFreq =  (CalleeFreq/2 +1);  // Fix!

      errs() << "-SW Cost1\t" << SWCostSuperFunction<< " " << SWCostSuperFunctionLocal  << " " << CalleeFreq << " " << getSWCostOfFunction(F) << " " << F->getName() << "\n\n";
      SWCostSuperFunctionLocal = getSWCostOfFunction(F) * (CalleeFreq) ; // Fix!
      SWCostSuperFunction+= SWCostSuperFunctionLocal;
      errs() << "--SW Cost2\t" << SWCostSuperFunction<< " " << SWCostSuperFunctionLocal  << " " << CalleeFreq << " " << F->getName() << "\n\n";


      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
         
           if(CallInst *Call = dyn_cast<CallInst>(BI)) {

            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {

                Function *Calee = Call->getCalledFunction();
                std::string Calee_Name = Calee->getName();
                int fun_index = find_function(Function_list, Calee);

                unsigned long long int SWCostCalee = 0;

                // /int CalleeEntryFreq = getEntryCount(Calee); 
                double CalleeFreqRatio = 1;
          
              

                // Did we miss a calee that was not yet in oour Global Function List?
                //
                // if ((find_function(Function_missing_list, Calee)!=-1 )
                //       || find_function_name(Function_missing_list_names,Calee_Name)!=-1 ) {

                //     long int sw_latecy;
                //     std::string fun_name;      
                                
                //     sw_file.open("SW.txt");
                //     if(sw_file.fail()) { // checks to see if file opended 
                //       errs() << "error" << "\n"; 
                //       return 1; // no point continuing if the file didn't open...
                //     } 

                //     while(!sw_file.eof()) { 
                //       sw_file >> fun_name; // read first column number
                //       sw_file >> sw_latecy; // read second column number

                //       //errs() << " Open File!  "<< "\n";
                //      if( Calee_Name == fun_name && fun_name == "ProcessSlice" ) {

            
                //         SWCostSuperFunction  +=  sw_latecy; //* (CalleeFreq/2); // Fix
                //         errs() << "SW Cost Missing! \t" << fun_index << " " << Calee_Name << " " 
                //           << Function_SW_Cost_list[fun_index] << " file SW Cost " << sw_latecy << " Freq : " << CalleeFreq << "\n";

                //       }
                //     } // End of while    

                //      sw_file.close();      
                //   }   // End of If

                // else {
                   //if (fun_index !=-1 ) 
                     // SWCostCalee = Function_SW_Cost_list[fun_index] * (CalleeFreq); // Fix

                    long int sw_latecy;
                    int entry_count;
                    std::string fun_name;
                    
                                
                    sw_file.open("SW7.txt");
                    if(sw_file.fail()) { // checks to see if file opended 
                      errs() << "error" << "\n"; 
                      return 1; // no point continuing if the file didn't open...
                    } 

                    while(!sw_file.eof()) { 
                      sw_file >> fun_name; // read first column number
                      sw_file >> sw_latecy; // read second column number
                      sw_file >> entry_count; // read second column number

                      //errs() << " Open File!  "<< "\n";
                     if( Calee_Name == fun_name ){//&& fun_name == "ProcessSlice" ) {

                        CalleeFreqRatio =  (float)CalleeFreq / (float) entry_count;


                        double intpart, fractpart;

                        if (CalleeFreqRatio > 1) {
                          fractpart = modf (CalleeFreqRatio , &intpart);
                          CalleeFreqRatio = fractpart > 1 - fractpart ? fractpart : 1 - fractpart  ;
                        }


                        SWCostCalee  =  sw_latecy * CalleeFreqRatio;// CalleeFreqRatio; //(CalleeFreq/2+1); // Fix
                        //SWCostCalee  =  (sw_latecy) * (CalleeFreq/2+1); // Fix
                         errs()  << "---SW Cost3\t" << SWCostSuperFunction  << " " << SWCostCalee << " " << (sw_latecy)
                     << " " << CalleeFreq << " " << entry_count << " " << F->getName() << " " << Calee_Name << " CalleeFreqRatio " 
                      << format("%.8f",CalleeFreqRatio) <<  "\n\n" ;
                        // errs() << "SW Cost Missing! \t" << fun_index << " " << Calee_Name << " " 
                        //   << Function_SW_Cost_list[fun_index] << " file SW Cost " << sw_latecy << " Freq : " << CalleeFreq << "\n";

                      }
                    } // End of while    

                     sw_file.close();      


                   errs() << "----SW Cost4\t" << SWCostSuperFunction << " " << SWCostSuperFunctionLocal << " " << SWCostCalee 
                     << " " << CalleeFreq << " " << F->getName() << " " << Calee_Name << " Level " << Level  << " CalleeFreqRatio " 
                     << format("%.8f",CalleeFreqRatio) << "\n\n";

                  
          
                  

                    // errs() << "SW Cost 4 \t" << SWCostSuperFunction << " " << Calee->getName() << " "  
                    //      << fun_index  <<" " << Function_SW_Cost_list[fun_index] << " Freq : " << CalleeFreq << "\n";
                   // << " " << CalleeFreq/2 << " " << SWCostSuperFunction << "\n\n";
    
                    float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));

                    Level++;
                    SWCostCalee  += getSWCostOfSuperFunction(Calee, Level, BBFreqFloat, CalleeFreq, SWCostSuperFunction); // It is Zero in most calls within a Function.     
                    Level--;

                  //}

                    //SWCostSuperFunctionLocal +=SWCostCalee;
                    //SWCostSuperFunction += SWCostSuperFunctionLocal;
                    SWCostSuperFunction = SWCostCalee;
                    //SWCostSuperFunction += SWCostSuperFunction;
                     errs() << "------SW Cost5\t" << SWCostSuperFunction<< " " << SWCostSuperFunctionLocal << " " << SWCostCalee  
                      << " " << CalleeFreq << " " << F->getName() << " " << Calee_Name << "\n\n";
              
              }
            }
     
            
            // else{

            //   Value* V=Call->getCalledValue();
            //   Value* SV = V->stripPointerCasts();

            //   if (!isIndirectSystemCall(SV) ) {

       
            //     StringRef Indirect_Called_Name = SV->getName();
            //     //errs() <<"\t" << F->getName() << "\t -->\t" << Indirect_Called_Name << "\n";

            //     int fun_index = find_function_name(Function_Names_list, Indirect_Called_Name);

            //     if (fun_index >=0 ) { // Is it in our list?
            //       SWCostSuperFunction  += Function_SW_Cost_list[fun_index] * (CalleeFreq/2); // Fix!
            //       //SWCostSuperFunction  += Function_SW_Cost_list[fun_index] * (CalleeFreq); // Fix!

            //     //else{

            //       errs() << "Indirect Call in Our SW Functions list! " << "\n";

            //       Function *Calee = Function_list[fun_index];                  
            //       float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));

            //       Level++;
            //       SWCostSuperFunction  += getSWCostOfSuperFunction(Calee, Level, BBFreqFloat, CalleeFreq); // It is Zero in most calls within a Function.     
            //       Level--;
              
            //     }
            //     else
            //       errs() << "Indirect Call *not* in Our SW Functions list! \t" << Indirect_Called_Name << "\n";

            //   }
            // } // End of Else branch.
           
           } // End of Call Instruction
         } // End of For - BB Iterator

        } // End of For - Function Iterator

        //SWCostSuperFunction+= SWCostSuperFunctionLocal;
        errs() << "------SW Cost 6\t" << SWCostSuperFunction<< " " << SWCostSuperFunctionLocal  << " " << CalleeFreq << " " << F->getName() << "\n\n";

      return SWCostSuperFunction;
    }

    // Software Cost for Functions Estimation. - Corrected! (Right Calls)
   //
   long int getSWCostOfFunction(Function *F) {

      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
      long int Cost_Software_Function = 0;

      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        long int Cost_Software_BB = 0;
        float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(&*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
        //float BBFreq = BBFreqFloat * static_cast<float>(CalleeFreq);

        // if (BBFreqFloat<1)
        //   BBFreqFloat=1;

        // Calculate the Software Cost in Cycles multiplied with the respective frequency of the BB.
        Cost_Software_BB = static_cast<long int> (getSWCostOfBB(&*BB) * BBFreqFloat);
        Cost_Software_Function += Cost_Software_BB;
      }

        //errs() << " SW Cost:\t" << F->getName() << "\t" << Cost_Software_Function << "\n";
      return Cost_Software_Function;
    
    }

  
  // Get the Area extimation of a Super Function in LUTs.
  //
  unsigned int logAreaofSuperFunction(Function *F, int CurrentLevel) {

    std::ifstream area_file;
    int LevelSuperFunction=0;
    unsigned int AreaofSuperFunction = 0;

    std::string Function_Name = F->getName();  

    AreaofSuperFunction = getAreaofFunction(F);
    errs() << "-Area  Cost1\t" << AreaofSuperFunction<< " " << getAreaofFunction(F)  << " " << F->getName() << "\n\n";

    for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){

           if(CallInst *Call = dyn_cast<CallInst>(BI)) {
            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {

                if (LevelSuperFunction == 0)
                  LevelSuperFunction = CurrentLevel; // Assign the right Level of Calling Functions


                Function *Calee = Call->getCalledFunction();
                std::string Calee_Name = Calee->getName();
                unsigned long long int AreaCostCalee = 0;

                if (find_function(Function_Area_list, Calee) == -1) {
                

                  Function_Area_list.push_back(Calee);

                  long int area;
                  std::string fun_name;
                  bool found = false;
                  
                  for (int i = CurrentLevel-1; i>=0; i--) {
                              
                    area_file.open("AREA_" + std::to_string(i) + ".txt");
                    if(area_file.fail()) { // checks to see if file opended 
                      errs() << "error" << "\n"; 
                      return 1; // no point continuing if the file didn't open...
                    } 

                    while(!area_file.eof()) { 
                      area_file >> fun_name; // read first column number
                      area_file >> area; // read second column number

                      if( Calee_Name == fun_name ){
                        found = true;


                        AreaCostCalee  =  area;
                
                        errs()  << "---AREA Cost2\t" << AreaofSuperFunction  << " " << AreaCostCalee << " " << area
                           << " " << F->getName() << " " << Calee_Name  <<  "\n\n" ;
                      }
                    } // End of while    

                    area_file.close();

                    if (found){
                      errs() << "Found you!" << "\n";
                      break; // break when read from the top Level File. (Maximum Latency)
                    }
                  } // End of for

                  AreaofSuperFunction += AreaCostCalee; // Final Computation of AreaofSuperFunction
              }
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
            //<< EntryCount << "\t"
            <<"\n";
        myfile.close();    

        errs() << "------AREA Cost 3\t" << AreaofSuperFunction   << " " << F->getName() << "\n\n";
      }
    
    return AreaofSuperFunction;
  }

  // Get the Area extimation of a Super Function in LUTs.
  //
  unsigned int getAreaofSuperFunction(Function *F){

    unsigned int AreaofSuperFunction = 0;
    // std::vector<Function *> Function_Area_list; // Local Function List
    // Function_Area_list.clear();
    AreaofSuperFunction = getAreaofFunction(F); // It is zero in calls within the Function
    //errs() << "HW Area Call : " << F->getName() << " "   << AreaofSuperFunction << "\n";

    // return AreaofSuperFunction; // Testing!

    for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){

           if(CallInst *Call = dyn_cast<CallInst>(BI)) {
            
            if (Call->getCalledFunction()) {

              if (!isSystemCall(Call->getCalledFunction())) {

                Function *Calee = Call->getCalledFunction();

                if (find_function(Function_Area_list, Calee) == -1) {
                

                Function_Area_list.push_back(Calee);
               // int fun_index = find_function(Function_list, Calee);

                //AreaofSuperFunction  += Function_HW_Area_list[fun_index];
                //errs() << "HW Area : " << Calee->getName() << "\t"  << Function_HW_Area_list[fun_index] << "\t" << AreaofSuperFunction << "\n";
                if (getEntryCount(Calee) > 0)
                  AreaofSuperFunction  += getAreaofFunction(Calee); 
                //AreaofSuperFunction  += getAreaofSuperFunction(Calee); 
                //errs() << "HW Area 2 : " << "\t"  << AreaofSuperFunction<< "\n";
              }

              }
            }
                 
            else{

                Value* V=Call->getCalledValue();
                Value* SV = V->stripPointerCasts();

                if (!isIndirectSystemCall(SV) ) {

       
                  StringRef Indirect_Called_Name = SV->getName();
                  //errs() <<"\t" << F->getName() << "\t -->\t" << Indirect_Called_Name << "\n";

                  int fun_index = find_function_name(Function_Names_list, Indirect_Called_Name);

                  if (fun_index >=0) { // Is it in our list?


                  errs() << "Indirect Call in Our list! " << "\n";

                  Function *Calee = Function_list[fun_index];
                  
                  //AreaofSuperFunction  += Function_HW_Area_list[fun_index];
                 // errs() << "HW Area : " << Calee->getName() << "\t"  << Function_HW_Area_list[fun_index] << "\t" << AreaofSuperFunction << "\n";
                 if (getEntryCount(Calee) > 0)
                  AreaofSuperFunction  += getAreaofFunction(Calee); 
                  //AreaofSuperFunction  += getAreaofSuperFunction(Calee); 
                }
                else
                  errs() << "Indirect Call *not* in Our list! \t" << Indirect_Called_Name << "\n";

              }
            } // End of Else branch.
           
           } // End of Call Instruction 

         } // End of For - BB Iterator

        } // End of For - Function Iterator
    
    return AreaofSuperFunction;
  }



    // Get the Delay Estimation for the Region.
    //
    //
    float getDelayOfRegion(Region *R) {

      float DelayOfRegion, DelayOfRegionTotal = 0;
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI(); 
      std::vector<BasicBlock *> worklist, predecessor_bb, successor_bb;
      std::vector<float> BBFreqPerIter;
      std::vector<float> BBFreqTotal;
      std::vector<float> DelayRegionPathsPerIter, DelayRegionPathsTotal;

      // Clear vectors.
      worklist.clear();
      predecessor_bb.clear();
      successor_bb.clear();
      BBFreqPerIter.clear();
      BBFreqTotal.clear();
      DelayRegionPathsPerIter.clear();
      DelayRegionPathsTotal.clear();

      // Populate worklist with Region's Basic Blocks and their respective BB's Frequencies. Both Per Iteration and Total.
      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {

        float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
        int EntryFuncFreq = getEntryCount(R->block_begin()->getParent());
        float BBFreq = BBFreqFloat * static_cast<float>(EntryFuncFreq);
        
        worklist.push_back(*BB);
        BBFreqPerIter.push_back(BBFreqFloat);
        BBFreqTotal.push_back(BBFreq);
      }

      // errs() << "\n";
      // for (int i=0; i< worklist.size(); i++) {
      //   errs() << " BBs in Region " << worklist[i]->getName() << " Freq per Iter " << BBFreqPerIter[i] <<  
      //     " Freq Total " << BBFreqTotal[i] << "\n"; // My debugging Info!    
      // }

      // Region has more than one BBs.
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
      
              if(count < find_bb(worklist, Succ) ) { 

                predecessor_bb.push_back(BB); // Populate send_node vector
                successor_bb.push_back(Succ); // Populate receive_node vector
              }
            }
          }
        }

        // errs() << "\n\n"  ;
        // for (int i=0; i< predecessor_bb.size(); i++) {
        //   errs() << " BB Edges in the Region : " << predecessor_bb[i]->getName() << "  --->     " <<  successor_bb[i]->getName() << "\n"; // My debugging Info!
        // }



        // Critical Path Estimation. 
        //       
        // 
        for (std::vector<BasicBlock *>::iterator bb_iter = worklist.begin(); bb_iter != worklist.end(); ++bb_iter) {

          BasicBlock *BB = *bb_iter;

          // Find the end Nodes - Bottom-most Nodes (BBs) in the CFG Graph.
          if (find_bb(predecessor_bb, BB) == -1) {

            BasicBlock *EndNode = BB;

            BasicBlock *CurrentNode;
            int position;
            float delay_path_estimation, delay_path_estimation_total;

            while(find_bb(successor_bb, EndNode)>=0) {  

              CurrentNode = EndNode;
              position= 0;
              delay_path_estimation= getDelayOfBB(CurrentNode) * BBFreqPerIter[find_bb(worklist, CurrentNode)];
              delay_path_estimation_total = getDelayOfBB(CurrentNode) * BBFreqTotal[find_bb(worklist, CurrentNode)];

              while (find_bb(successor_bb, CurrentNode) >=0) {

                position = find_bb(successor_bb, CurrentNode); 
                delay_path_estimation = delay_path_estimation + (getDelayOfBB(predecessor_bb[position]) * BBFreqPerIter[find_bb(worklist, predecessor_bb[position])]);
                delay_path_estimation_total = delay_path_estimation_total + (getDelayOfBB(predecessor_bb[position]) * BBFreqTotal[find_bb(worklist, predecessor_bb[position])]);
                CurrentNode = predecessor_bb[position];

                // errs() << "Delay for this node " << format("%.8f", getDelayEstim(send_node[position])) << "\n"; // My debugging Info!    
                // errs() << "Delay path estim    " <<  format("%.8f", delay_path_estimation ) << "\n"; // My debugging Info!
                // errs() << " Current Node is:   " << *CurrentNode << "\n";
              }

              // errs() << "Delay path estim    " <<  format("%.8f", delay_path_estimation ) << "\n"; // My debugging Info!
              DelayRegionPathsPerIter.push_back(delay_path_estimation);
              DelayRegionPathsTotal.push_back(delay_path_estimation_total);
              successor_bb.erase(successor_bb.begin() + position);   // deleting the last edge
              predecessor_bb.erase(predecessor_bb.begin() + position);        //  deleting the last edge

              
            }
          }
        }

        DelayOfRegion = get_max(DelayRegionPathsPerIter);       // Per Iteration.
        DelayOfRegionTotal = get_max(DelayRegionPathsTotal);    // Total Delay.
      }

      // In case that the Region has only one BB.
      // else if (worklist.size() == 1) {
      else {
        DelayOfRegion      = getDelayOfBB(worklist[0]) * BBFreqPerIter[0];
        DelayOfRegionTotal = getDelayOfBB(worklist[0]) * BBFreqTotal[0];
      }

 

      // errs() << " Delay Estimation for Region per Iteration is : " << format("%.8f", DelayOfRegion)      << " nSecs" << "\n";
      // errs() << " Delay Estimation for Region Total         is : " << format("%.8f", DelayOfRegionTotal) << " nSecs" << "\n";

      // errs() << " DEPI " << format("%.8f", DelayOfRegion);
      // errs() << " DET " << format("%.8f", DelayOfRegionTotal) << " ";


      return DelayOfRegionTotal;
    }


    // Check to see if Region is Valid.
    virtual bool isRegionValid(Region *R) {

      if (R->getExit()) { // Check for Exit Block

        // Gather the input Data Flow for the Region.
        int Input = gatherInput(R);

        // Gather the output Data Flow for the Region.
        //
        // We do not consider the whole function as possible Region.
        // So Exit Block should not be NULL. (ExitBlock != NULL)
        int Output = gatherOutput(R);

        // Check if specified I/O Constraints are met.
        if (isRegionCallFree(R))
          return true;

      }

      return false;
    }

    // Check to see if Region is Valid.
    virtual bool isRegionCallFree(Region *R) {

      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) 
        if(!isBBCallFree(BB))
          return false;

      return true;
    }

    float getRegionTotalFreq (Region *R) {

      
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
      float RegionFreq = 0;
      bool backedge = false;
      Region::block_iterator BB_it_entry = R->block_begin();
      BasicBlock * BB_entry = *BB_it_entry;
      Function   *FunctionOfBB_entry = BB_entry->getParent();


      float BBEntryFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(BB_entry).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
      int EntryFuncFreq = getEntryCount(FunctionOfBB_entry);
      float BBEntryFreq = BBEntryFreqFloat * static_cast<float>(EntryFuncFreq); // Freq_Total


      // Case Entry of Region is Entry of Function.
      if (BB_entry == FunctionOfBB_entry->begin())
        return static_cast<float>(EntryFuncFreq);


      if (BB_entry->getSinglePredecessor())
        return BBEntryFreq;
      
      //else {
        for (pred_iterator PI = pred_begin(BB_entry), PE = pred_end(BB_entry); PI != PE; ++PI) {

          BasicBlock *BB_pred = *PI;
          
          if (R->contains(BB_pred)) {
            backedge = true;
            continue;
          }

          if (BranchInst *Branch = dyn_cast<BranchInst>(&*BB_pred->getTerminator())) {

            if (Branch->isUnconditional()) {

              float BBPredFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(BB_pred).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
              int   PredFuncFreq = getEntryCount(FunctionOfBB_entry);
              float BBPredFreq = BBPredFreqFloat * static_cast<float>(PredFuncFreq); // Freq_Total  

              RegionFreq += BBPredFreq;
            }
          }
        }

        if (!backedge)
          return BBEntryFreq;

        return RegionFreq;
    }

   // Software Cost for Regions Estimation.  **NEW**
   //
   long int getCostOnSoftwareRegion(Region *R) {

      Region::block_iterator BB_begin = R->block_begin();
      Function   *FunctionOfBB = BB_begin->getParent();

      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI();
      long int Cost_Software_Region = 0;

      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {

        long int Cost_Software_BB = 0;
        float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
        int EntryFuncFreq = getEntryCount(FunctionOfBB);
        float BBFreq = BBFreqFloat * static_cast<float>(EntryFuncFreq);

        // Calculate the Software Cost in Cycles multiplied with the respective frequency of the BB.
        Cost_Software_BB = static_cast<long int> (getSWCostOfBB(*BB) * BBFreq);
        Cost_Software_Region += Cost_Software_BB;
      }

      return Cost_Software_Region;
    }

    void PrintRegion(Region *R) {



      Region::block_iterator BB = R->block_begin();
      Function   *FunctionOfBB = BB->getParent();


      unsigned int BBRegionCounter = 0;
      unsigned int DFGNodesRegion = 0;
      unsigned int GoodDFGNodesRegion = 0;

      // Goodness and Density
      unsigned int   OptimalityRegion = 0;
      float DensityRegion = 0;
      unsigned int AreaOfRegion = getAreaofRegion(R);
      float RegionFreq           = getRegionTotalFreq(R);
      float DelayOfRegion        = getDelayOfRegion(R);
      float DelayOfRegionPerIter = getDelayOfRegionPerIter(R);

      // Costs to calculate Speedup.
      long int Cost_Software = 0;
      long int Cost_Hardware = 0;
      long int Overhead      = 0;

      std::vector<BasicBlock *> BB_list; // Worklist for Basic Blocks in Region.
      BB_list.clear();

      errs() << "\n\n"; 
      
      // if (R->isSimple()) {
      //   errs() << "   Simple Region **** "  << "\n";
      //   ++SimpleRegionCounter;
      // }
      errs() << "   **********************************************************************************" << '\n';
      errs() << "   Function Name is : " << FunctionOfBB->getName() << "\n";
      errs() << "   Region Depth  is : " << R->getDepth() << "\n";
      errs() << "   Region Name   is : " << R->getNameStr() << "\n\n";

      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {

        //printBasicBlock(BB);
        runOnBasicBlock(BB, &DFGNodesRegion, &GoodDFGNodesRegion, &OptimalityRegion);
        //getCostOnSoftware(BB, &Cost_Software);
        ++BBRegionCounter;

        BB_list.push_back(*BB);
  
      }


      DensityRegion = OptimalityRegion / DFGNodesRegion; // Density of the Region.

      Cost_Software = static_cast<long int> (getCostOnSoftwareRegion(R));
      Cost_Hardware = static_cast<long int> (getHWCostOfRegion(R));
      Overhead      = static_cast<long int> (RegionFreq * CALL_ACC_OVERHEAD);

      // Final "Speedup" of a Region.
      long int Speedup = Cost_Software - Cost_Hardware - Overhead;



      errs() << "   -------------------------------------------------------------" << '\n';
      errs() << "\n     BB Number is              : " << BBRegionCounter << "\n";
      errs() << "     Good DFG Nodes are        : " << GoodDFGNodesRegion << "\n" ; 
      errs() << "     DFG Nodes Number is       : " << DFGNodesRegion << "\n\n";
      errs() << "     Optimality of Region is   : " << OptimalityRegion << "\n";
      //errs() << "     Density of Region is      : " << DensityRegion << "\n\n";
      errs() << "   -------------------------------------------------------------" << '\n';
   
      errs() << "Good " << OptimalityRegion << " Dens " << static_cast<int>(DensityRegion) << " Func " << 
        FunctionOfBB->getName() << " Reg " << R->getNameStr() << " Speedup " << Speedup << " Cost_Software " << Cost_Software << 
         " Cost_Hardware " << Cost_Hardware << " Overhead " << Overhead << " Area " << AreaOfRegion << "\n"  ;

     // Write Regions Identified in Regions.txt file.
     std::string FuncName   = FunctionOfBB->getName(); 
     std::string RegionName = R->getNameStr();

      myrawfile.open ("Regions_raw.txt", std::ofstream::out | std::ofstream::app); 
      myrawfile << FuncName <<  "\t" << RegionName << "\t"  << AreaOfRegion << "\t";

      myrawfile << static_cast<int>(RegionFreq) << "\t";
      myrawfile << Speedup << "\t";
      myrawfile << Cost_Software << "\t";
      myrawfile << Cost_Hardware << " \t" << "\n";
      myrawfile.close();

     myfile.open ("Regions.txt", std::ofstream::out | std::ofstream::app); 
     myfile << FuncName << " " << RegionName << " "<< Speedup << " " << AreaOfRegion << " " ;
     myfile.close();

  }



    virtual bool RegionAnalysis (Region *R) {
      if (R->getExit()) { // Check for Exit Block

        std::vector<BasicBlock *> Function_BB_list;
        Function_BB_list.clear();

        Function *F = R->block_begin()->getParent();

        // Gather Block of the function.
        for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {

          if(find_bb(Function_BB_list, &*BB) == -1)
            Function_BB_list.push_back(&*BB);

        }

        //errs()<< "BB list size is : "  << Function_BB_list.size() << "\n"; // This was used for Debug!


        // Gather the input Data Flow for the Region.
        int Input = gatherInput(R);

        // Gather the output Data Flow for the Region.
        //
        // We do not consider the whole function as possible Region.
        // So Exit Block should not be NULL. (ExitBlock != NULL)
        int Output = gatherOutput(R);

        // Check if specified I/O Constraints are met.
        //if (Input<= User_Input && Output<=User_Output) {


          //float test_2 = getDelayOfRegion(R); // Test_2 Remove it after you are done!
          
          PrintRegion(R);
          // errs() << " I " << Input << " O " << Output ;
          // int InputData  = getInputData(R);
          // int OutputData = getOutputData(R);
          // errs() << " Func_Freq "  << getEntryCount(R->block_begin()->getParent()); // Function Entry Count Print!
          // errs() << " A " << getAreaofRegion(R) ;// Print Area of the Region
          //getDelayOfRegion(R);
          

          // errs() << "     Input   Data is :  " << InputData  << "\n";
          // errs() << "     Output  Data is :  " << OutputData << "\n";

          // Loops Information
          LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
          ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();

          unsigned int NumberOfLoops = 0;
          unsigned int NumberOfArrays = 0;

          getNumberOfLoopsandArrays(NumberOfLoops, NumberOfArrays, R, LI, SE);
          errs() << "     Number Of loops  : " << NumberOfLoops   << '\n';
          errs() << "     Number Of Arrays : " << NumberOfArrays  << '\n';

          if (NumberOfLoops) { // Might need to add NumberOfArrays as arguments inside the if statement!

            int InputLoop  = getInputDataLoop(R, LI, SE, NumberOfLoops, NumberOfArrays);
            int OutputLoop = getOutputDataLoop(R, LI, SE, NumberOfLoops);
          }

          // Print Static - Dynamic Classification.
          PrintSDClassification(R, LI, SE);
          errs() << "   **********************************************************************************" << '\n';

        //} // End of if User Input/Outut specified.
      } // End of If Exit Block check.



      return false;



    }


    void gatherRegionsGoodnessAndDensity(Region *R) {



      Region::block_iterator BB = R->block_begin();
      Function   *FunctionOfBB = BB->getParent();


      unsigned int BBRegionCounter = 0;
      unsigned int DFGNodesRegion = 0;
      unsigned int GoodDFGNodesRegion = 0;

      unsigned int   OptimalityRegion = 0;
      float DensityRegion = 0;


      // std::vector<BasicBlock *> BB_list; // Worklist for Basic Blocks in Region.
      // BB_list.clear();

      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {

        runOnBasicBlock(BB, &DFGNodesRegion, &GoodDFGNodesRegion, &OptimalityRegion);

      }

      DensityRegion = OptimalityRegion / DFGNodesRegion; // Density of the Region.


      errs() << "   -------------------------------------------------------------" << '\n';
      errs() << "\n     BB Number is              : " << BBRegionCounter << "\n";
      errs() << "     Good DFG Nodes are        : " << GoodDFGNodesRegion << "\n" ; 
      errs() << "     DFG Nodes Number is       : " << DFGNodesRegion << "\n\n";
      errs() << "     Optimality of Region is   : " << OptimalityRegion << "\n";
      errs() << "     Density of Region is      : " << DensityRegion << "\n\n";
      errs() << "   -------------------------------------------------------------" << '\n';
   
      errs() << "Good " << OptimalityRegion << " Dens " << static_cast<int>(DensityRegion) << " Func " << \
        FunctionOfBB->getName() << " Reg " << R->getNameStr() ;
    }



    // Get the Delay Estimation for the Region.
    //
    //
    float getDelayOfRegionPerIter(Region *R) {

      float DelayOfRegion, DelayOfRegionTotal = 0;
      BlockFrequencyInfo *BFI = &getAnalysis<BlockFrequencyInfoWrapperPass>().getBFI(); 
      std::vector<BasicBlock *> worklist, predecessor_bb, successor_bb;
      std::vector<float> BBFreqPerIter;
      std::vector<float> BBFreqTotal;
      std::vector<float> DelayRegionPathsPerIter, DelayRegionPathsTotal;

      // Clear vectors.
      worklist.clear();
      predecessor_bb.clear();
      successor_bb.clear();
      BBFreqPerIter.clear();
      BBFreqTotal.clear();
      DelayRegionPathsPerIter.clear();
      DelayRegionPathsTotal.clear();

      // Populate worklist with Region's Basic Blocks and their respective BB's Frequencies. Both Per Iteration and Total.
      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {

        float BBFreqFloat = static_cast<float>(static_cast<float>(BFI->getBlockFreq(*BB).getFrequency()) / static_cast<float>(BFI->getEntryFreq()));
        int EntryFuncFreq = getEntryCount(R->block_begin()->getParent());
        float BBFreq = BBFreqFloat * static_cast<float>(EntryFuncFreq);
        
        worklist.push_back(*BB);
        BBFreqPerIter.push_back(BBFreqFloat);
        BBFreqTotal.push_back(BBFreq);
      }

      // errs() << "\n";
      // for (int i=0; i< worklist.size(); i++) {
      //   errs() << " BBs in Region " << worklist[i]->getName() << " Freq per Iter " << BBFreqPerIter[i] <<  
      //     " Freq Total " << BBFreqTotal[i] << "\n"; // My debugging Info!    
      // }

      // Region has more than one BBs.
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
      
              if(count < find_bb(worklist, Succ) ) { 

                predecessor_bb.push_back(BB); // Populate send_node vector
                successor_bb.push_back(Succ); // Populate receive_node vector
              }
            }
          }
        }

        // errs() << "\n\n"  ;
        // for (int i=0; i< predecessor_bb.size(); i++) {
        //   errs() << " BB Edges in the Region : " << predecessor_bb[i]->getName() << "  --->     " <<  successor_bb[i]->getName() << "\n"; // My debugging Info!
        // }



        // Critical Path Estimation. 
        //       
        // 
        for (std::vector<BasicBlock *>::iterator bb_iter = worklist.begin(); bb_iter != worklist.end(); ++bb_iter) {

          BasicBlock *BB = *bb_iter;

          // Find the end Nodes - Bottom-most Nodes (BBs) in the CFG Graph.
          if (find_bb(predecessor_bb, BB) == -1) {

            BasicBlock *EndNode = BB;

            BasicBlock *CurrentNode;
            int position;
            float delay_path_estimation, delay_path_estimation_total;

            while(find_bb(successor_bb, EndNode)>=0) {  

              CurrentNode = EndNode;
              position= 0;
              delay_path_estimation= getDelayOfBB(CurrentNode) * BBFreqPerIter[find_bb(worklist, CurrentNode)];
              delay_path_estimation_total = getDelayOfBB(CurrentNode) * BBFreqTotal[find_bb(worklist, CurrentNode)];

              while (find_bb(successor_bb, CurrentNode) >=0) {

                position = find_bb(successor_bb, CurrentNode); 
                delay_path_estimation = delay_path_estimation + (getDelayOfBB(predecessor_bb[position]) * BBFreqPerIter[find_bb(worklist, predecessor_bb[position])]);
                delay_path_estimation_total = delay_path_estimation_total + (getDelayOfBB(predecessor_bb[position]) * BBFreqTotal[find_bb(worklist, predecessor_bb[position])]);
                CurrentNode = predecessor_bb[position];

                // errs() << "Delay for this node " << format("%.8f", getDelayEstim(send_node[position])) << "\n"; // My debugging Info!    
                // errs() << "Delay path estim    " <<  format("%.8f", delay_path_estimation ) << "\n"; // My debugging Info!
                // errs() << " Current Node is:   " << *CurrentNode << "\n";
              }

              // errs() << "Delay path estim    " <<  format("%.8f", delay_path_estimation ) << "\n"; // My debugging Info!
              DelayRegionPathsPerIter.push_back(delay_path_estimation);
              DelayRegionPathsTotal.push_back(delay_path_estimation_total);
              successor_bb.erase(successor_bb.begin() + position);   // deleting the last edge
              predecessor_bb.erase(predecessor_bb.begin() + position);        //  deleting the last edge

              
            }
          }
        }

        DelayOfRegion = get_max(DelayRegionPathsPerIter);       // Per Iteration.
        DelayOfRegionTotal = get_max(DelayRegionPathsTotal);    // Total Delay.
      }

      // In case that the Region has only one BB.
      // else if (worklist.size() == 1) {
      else {
        DelayOfRegion      = getDelayOfBB(worklist[0]) * BBFreqPerIter[0];
        DelayOfRegionTotal = getDelayOfBB(worklist[0]) * BBFreqTotal[0];
      }

 

      // errs() << " Delay Estimation for Region per Iteration is : " << format("%.8f", DelayOfRegion)      << " nSecs" << "\n";
      // errs() << " Delay Estimation for Region Total         is : " << format("%.8f", DelayOfRegionTotal) << " nSecs" << "\n";

      // errs() << " DEPI " << format("%.8f", DelayOfRegion);
      // errs() << " DET " << format("%.8f", DelayOfRegionTotal) << " ";


      return DelayOfRegion;
    }


    virtual void getNumberOfLoopsandArrays (unsigned int &NumberOfLoops, unsigned int &NumberOfArrays, Region *R, LoopInfo &LI, ScalarEvolution &SE ) {

      std::vector<Loop *> Loops;
      Loops.clear();
      std::vector<Value *> ArrayReferences;
      ArrayReferences.clear();

      // Loops Category
      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {

        BasicBlock *CurrentBlock = *BB;

        // Iterate inside the Loop.
        if (Loop *L = LI.getLoopFor(CurrentBlock)) {
            errs() << "\n     Num of Back Edges     : " << L->getNumBackEdges() << "\n";
            errs() << "     Loop Depth            : " << L->getLoopDepth() << "\n";
            errs() << "     Backedge Taken Count  : " << *SE.getBackedgeTakenCount(L) << '\n';
            errs() << "     Loop iterations       : " << SE.getSmallConstantTripCount(L) << "\n\n";

            NumberOfArrays += GatherNumberOfArrays(CurrentBlock, ArrayReferences); 

            if (find_loop(Loops, L) == -1 ){ 
                Loops.push_back(L);
                NumberOfLoops++;
              }


        }
      } // End of for

    }
   

    virtual int getInputData(Region *R) {

      int InputData = 0;
      int NumberOfLoads = 0;

    for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {
      BasicBlock *CurrentBlock = *BB;

      // Iterate inside the basic block.
      for(BasicBlock::iterator BI = CurrentBlock->begin(), BE = CurrentBlock->end(); BI != BE; ++BI) {

        //if(Instruction *Inst = dyn_cast<Instruction>(&*BI)) {

         // Do not consider Branch Instructions.
         if (dyn_cast<BranchInst>(&*BI))
          continue;

          // Load Info
          if(LoadInst *Load = dyn_cast<LoadInst>(&*BI)) {

            // Non-Atomic and Non-Volatile Load.
            // if (Load->isSimple())
            //   errs() << "   Simple Load  " << '\n';

            InputData += Load->getType()->getPrimitiveSizeInBits();
            ++NumberOfLoads;


          }


      }
    }

    errs() << " Loads " << NumberOfLoads ;

      return InputData;
    }

    virtual int getInputDataLoop(Region *R, LoopInfo &LI, ScalarEvolution &SE, unsigned int NumberOfLoops, unsigned int NumberOfArrays) {

      int InputData = 0;
      int NumberOfLoads = 0;

      int *LoopIterationsArray   = new int[NumberOfLoops] ();
      std::string *ArrayRefNames = new std::string[NumberOfArrays] ();
      int *ArrayLoads            = new int[NumberOfArrays] ();  // Could use std::vector instead.

      int indexNamesArray = 0;

      std::vector<Value *> ArrayReferences;
      ArrayReferences.clear();

      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {
        BasicBlock *CurrentBlock = *BB;
        int BBLoads = 0;
        unsigned int loop_depth =0;

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = CurrentBlock->begin(), BE = CurrentBlock->end(); BI != BE; ++BI) {

          if (Loop *L = LI.getLoopFor(CurrentBlock)) {
          
            loop_depth = L->getLoopDepth();

            // Check Number Of Loops!
            if (NumberOfLoops>=1) {

              LoopIterationsArray[loop_depth-1] = SE.getSmallConstantTripCount(L);

              // Load Info
              if(LoadInst *Load = dyn_cast<LoadInst>(&*BI)) {

                if (GetElementPtrInst *Source = dyn_cast<GetElementPtrInst>(&*Load->getOperand(0))) {

                  // Load comes from an Array.
                  if (Value *ArrayRef = Source->getPointerOperand()) {

                    std::string ArrayRefName = ArrayRef->getName();

                    if (find_array(ArrayReferences, ArrayRef) == -1) {
                      ArrayReferences.push_back(ArrayRef);
                      ArrayRefNames[indexNamesArray]=ArrayRefName;
                      indexNamesArray++;
                    }

                    for (unsigned int i=0; i<NumberOfArrays; i++)
                      if (ArrayRefName == ArrayRefNames[i])
                        ArrayLoads[i]++;


                  } // End of Array check.
                }



              int InputLoad = Load->getType()->getPrimitiveSizeInBits();

              if (NumberOfLoops==1)
                InputLoad = InputLoad * LoopIterationsArray[loop_depth-1];
              
              else
                for (unsigned int i=0; i<loop_depth; i++)
                  InputLoad = InputLoad * LoopIterationsArray[i];
              

                  

                InputData +=InputLoad;
                ++NumberOfLoads;
                ++BBLoads;


              }
            }           
          }
        }

        if (BBLoads && NumberOfArrays) {

          // Print for Total Loads in a Basic Block.
          errs() << "     Input Data for " << CurrentBlock->getName() << " is   :  " << BBLoads ;  
          if (NumberOfLoops>1) {
            for (unsigned int j=0; j<loop_depth; j++)
              errs() << " X "  << LoopIterationsArray[j];
          }

          else
            errs() << " X "  << LoopIterationsArray[loop_depth-1];
        
          errs() << "\n\n";

          // Print for each Array separately.
          if (NumberOfLoops>=1) {
            for (unsigned int i=0; i<NumberOfArrays; i++) {

              if (ArrayLoads[i]) {
                errs() << "     Input Data for Array "<< ArrayRefNames[i] << "  is   :  " << ArrayLoads[i];

                if (NumberOfLoops==1)
                  errs() << " X "  << LoopIterationsArray[loop_depth-1];

                else
                  for (unsigned int j=0; j<loop_depth; j++)
                    errs() << " X "  << LoopIterationsArray[j];
              

                errs() << "\n";
              } 
            }           
          }        
          errs() << "\n\n";
        }
      }

      errs() << "     Loads                  :  " << NumberOfLoads << '\n';
      errs() << "     Input Data is (Bytes)  :  " << InputData / 8 << "\n\n";

      // Clean Up.
      delete [] LoopIterationsArray;
      delete [] ArrayRefNames;
      delete [] ArrayLoads;

      return InputData;
    }

     virtual int getOutputData(Region *R) {

      int OutputData = 0;
      int NumberOfStores = 0;

      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {
        BasicBlock *CurrentBlock = *BB;

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = CurrentBlock->begin(), BE = CurrentBlock->end(); BI != BE; ++BI) {

          ///if(Instruction *Inst = dyn_cast<Instruction>(&*BI)) {

           // Do not consider Branch Instructions.
           if (dyn_cast<BranchInst>(&*BI))
            continue;

            // Store Info
            if(StoreInst *Store = dyn_cast<StoreInst>(&*BI)) {

              OutputData += Store->getOperand(0)->getType()->getPrimitiveSizeInBits();
              ++NumberOfStores;

            }

        }
      }

      errs() << " Stores " << NumberOfStores  ;
  

      return OutputData;
    }

    virtual int getOutputDataLoop(Region *R, LoopInfo &LI, ScalarEvolution &SE, unsigned int NumberOfLoops) {

      int OutputData = 0;
      int NumberOfStores = 0;
      int LoopIterationsArray[10]= {0};
      // std::vector<Loop *> Loops;
      // Loops.clear();

      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {
        BasicBlock *CurrentBlock = *BB;

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = CurrentBlock->begin(), BE = CurrentBlock->end(); BI != BE; ++BI) {

          if (Loop *L = LI.getLoopFor(CurrentBlock)) {
          
            unsigned int loop_depth = L->getLoopDepth();

            // Check Number Of Loops!
            if (NumberOfLoops>1) {

              LoopIterationsArray[loop_depth-1] = SE.getSmallConstantTripCount(L);

              // Load Info
              if(StoreInst *Store = dyn_cast<StoreInst>(&*BI)) {

              int OutputStore = Store->getOperand(0)->getType()->getPrimitiveSizeInBits();

              for (unsigned int i=0; i<loop_depth; i++)
                OutputStore = OutputStore * LoopIterationsArray[i];
                  

                OutputData +=OutputStore;
                ++NumberOfStores;


              }
            }
            
            else {

              LoopIterationsArray[0] = SE.getSmallConstantTripCount(L);  
              
              // Load Info
              if(StoreInst *Store = dyn_cast<StoreInst>(&*BI)) {

              int OutputStore = Store->getOperand(0)->getType()->getPrimitiveSizeInBits();

              OutputStore = OutputStore * LoopIterationsArray[0];
                  
              OutputData +=OutputStore;
              ++NumberOfStores;


              }
            }

          }

        }
      }

      errs() << "     Stores                  :  " << NumberOfStores << '\n';
      errs() << "     Output Data is (Bytes)  :  " << OutputData / 8 << "\n\n";

      return OutputData;
    }


    // @brief  Gather Output Data Flow for the region.
    //
    // @param  R    The Region for which we are gathering information.
    //
    // @return int  The number of output Data instances (instructions) of the Region.
    virtual int gatherOutput(Region *R) {

      auto *TLIP = getAnalysisIfAvailable<TargetLibraryInfoWrapperPass>();
      TargetLibraryInfo *TLI = TLIP ? &TLIP->getTLI() : nullptr;
      int Output_number = 0;
      std::vector<Instruction *> ext_out; // Worklist for external output instructions.
      ext_out.clear();

      // Iterate over the Region's Basic Blocks.
      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {

          if(Instruction *Inst = dyn_cast<Instruction>(&*BI)) {

            // Do not consider Branch Instructions.
            if (dyn_cast<BranchInst>(&*BI))
              continue;

            if (!isInstructionTriviallyDead(Inst, TLI)) {      

              // Get the Users of the instruction.
              for (User *U : Inst->users()) {

                
        
                if (Instruction *User_Inst = dyn_cast<Instruction>(U)) {

                  // If the User is not inside this Region then it is considered as output. 
                  if (!(R->contains(User_Inst))) {

                    if (Inst && find_inst(ext_out, Inst) == -1) {
                      ext_out.push_back(Inst);
                      ++Output_number;
                      //errs()<< "    Output Instruction : " << "\t" << Inst->getName() << "\n";
                    }
                  }
                }
              }
            }
          }
        }
      }

      //errs() << "\n   Output Alive Number is  : " << Output_number << "\n\n";
      return Output_number;           
    }


    // @brief  Gather Input Data Flow for the region.
    //
    // @param  R    The Region for which we are gathering information.
    //
    // @return int  The number of Input Data instances (instructions) of the Region.
    virtual int gatherInput(Region *R) {

      auto *TLIP = getAnalysisIfAvailable<TargetLibraryInfoWrapperPass>();
      TargetLibraryInfo *TLI = TLIP ? &TLIP->getTLI() : nullptr;
      int Input_number  = 0;
      std::vector<Value *> ext_in;
      ext_in.clear();

      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {

          if(Instruction *Inst = dyn_cast<Instruction>(&*BI)) {

            // Do not consider Branch Instructions.
            if (dyn_cast<BranchInst>(&*BI))
              continue;

            if (!isInstructionTriviallyDead(Inst, TLI)) {      

              // Iterate over each operand of each Instruction.
              for (unsigned int i=0; i<Inst->getNumOperands(); i++) {

                Value *Operand = Inst->getOperand(i);

                // Exclude operands that represent constants.(signed integers) 
                if (Inst->getOperand(i)->getValueID() == 11)
                  continue; 

                // Iterate over all the instructions of the Region and compare the operand to them.
                bool local = compareInstrToOperand(R, Inst->getOperand(i));

                // Data Flow is incremented if the operand is not coming from a local Instruction.
                if (!local && Operand) {

                  if (find_op(ext_in, Operand) == -1) {
                    ext_in.push_back(Operand);
                    ++Input_number;
                    //errs()<< "     Input Operand : " << "\t" << Inst->getOperand(i)->getName() << "\n";
                  }
                }
              }
            }
          }
        }
      }

      //errs() << "\n   Input  Alive Number is  : " << Input_number << "\n\n"; 
      
      DEBUG(errs() << "I am here!\n");
      return Input_number;
    }

    // @brief  Compare Instructions of Basic Block to Operand.
    //
    // If the operand of a BB is not coming from a local instruction
    // of the same BB, then it is being received by a predecessor BB. 
    //
    // @param @param  R  The Region for which we are gathering information.
    //        Value The  Operand that we are comparing.
    //        int   The  Data Flow Number that represents either the
    //              input or the output value for the Region. 
    //
    // @return void
    virtual bool compareInstrToOperand(Region *R, Value *Operand) {

      bool local = false;

      for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB) {

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {

          if(Instruction *Inst = dyn_cast<Instruction>(&*BI)) {

            // Do not consider Branch Instructions.
            if (dyn_cast<BranchInst>(&*BI))
              continue;

            // Compare Operand with Instructions in BB.   
            // if (Operand->getName() != "") {
            //   if (Operand->getName() == Inst->getName())
            //     local = true;
            // }
            
            else
              if (Operand)
                if (Operand == Inst)
                  local = true;
          }
        }
      }

      return local;

    }

    virtual unsigned int GatherNumberOfArrays(BasicBlock *BB, std::vector<Value *> ArrayReferences) {

      unsigned int NumberOfArrays = 0;

      // Iterate inside the basic block.
      for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {

        // Load Info
        if(LoadInst *Load = dyn_cast<LoadInst>(&*BI)) {

          if (GetElementPtrInst *Source = dyn_cast<GetElementPtrInst>(&*Load->getOperand(0))) {

            // Load comes from an Array.
            if (Value *ArrayRef = Source->getPointerOperand()) {

              if (find_array(ArrayReferences, ArrayRef) == -1) {
                ArrayReferences.push_back(ArrayRef);
                NumberOfArrays++;
              }


            } // End of Array check.
          }
        }
      }

      
      return NumberOfArrays;
    }

    int getEntryCount(Function *F) {

      int entry_freq = 0;

      if (F->hasMetadata()) {

        MDNode *node = F->getMetadata("prof");

        if (MDString::classof(node->getOperand(0))) {
          auto mds = cast<MDString>(node->getOperand(0));
          std::string metadata_str = mds->getString();

          if (metadata_str == "function_entry_count"){
            if (ConstantInt *CI = mdconst::dyn_extract<ConstantInt>(node->getOperand(1))) {
              entry_freq = CI->getSExtValue();
              //errs() <<" Func_Freq " << entry_freq << " "; //  Turn it back on mayne.
            }              

          }
        }
      }

      return entry_freq;
    }


    virtual void getAnalysisUsage(AnalysisUsage& AU) const override {
              
        AU.addRequired<LoopInfoWrapperPass>();
        AU.addRequired<RegionInfoPass>();
        AU.addRequired<DependenceAnalysis>();
        AU.addRequiredTransitive<RegionInfoPass>();
        AU.addRequiredTransitive<ScalarEvolutionWrapperPass>();
        AU.addRequired<BlockFrequencyInfoWrapperPass>();
        AU.setPreservesAll();
    } 
  };
}

char IdentifyRegions::ID = 0;
static RegisterPass<IdentifyRegions> X("IdentifyRegionsSA", "Identify Valid System Aware Regions+");
