//===------------------------- IdentifyRegions.h -------------------------===//
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

#define DEBUG_TYPE "IdentifyRegions"

using namespace llvm;

std::ofstream IO_file; // File that I/O info is written.

#define M_AXI_ARRAY 700 // LUTs per M_AXI bus array.

namespace {
  int find_bb_name(std::vector<BasicBlock *> list, BasicBlock *BB) {

      for (unsigned i = 0; i < list.size(); i++)
        if (list[i]->getName() == BB->getName())
          return i;
      
    
    return -1;
  }

  int find_op(std::vector<Value *> list, Value *op) {

      for (unsigned i = 0; i < list.size(); i++) 
        if (list[i] == op)
          return i;
      
    
    return -1;
  }

  int find_array(std::vector<Value *> ArrayReferences, Value *ArrayRef) {

    for (unsigned i = 0; i < ArrayReferences.size(); i++) 
      if (ArrayReferences[i] == ArrayRef)
        return i;
      
    
    return -1;
  }

  int find_region(std::vector<Region *> list, Region *Reg) {

    for (unsigned i = 0; i < list.size(); i++)
      if (list[i] == Reg)
        return i;
      
    
    return -1;
  }

  int find_function(std::vector<Function *> list, Function *Fun) {

    for (unsigned i = 0; i < list.size(); i++)
      if (list[i] == Fun)
        return i;
      
    
    return -1;
  }

  int find_function_name(std::vector<StringRef> list, StringRef Fun_name) {

    for (unsigned i = 0; i < list.size(); i++)
      if (list[i] == Fun_name)
        return i;
      
    
    return -1;
  }

  //enum Sys { "llvm.lifetime.start", "llvm.lifetime.end", "printf"n "llvm.memset.p0i8.i64", "llvm.memcpy.p0i8.p0i8.i64", "exit" };

  // bool isSystemCall(Function *F)
  // {

  //   switch (F->getName()) {

  //   case "llvm.lifetime.start":
  //     return true;

  //   case "llvm.lifetime.end":
  //     return true;

  //   case "printf":
  //     return true;

  //   case "llvm.memset.p0i8.i64":
  //     return true;

  //   case "llvm.memcpy.p0i8.p0i8.i64":
  //     return true;

  //   case "exit":
  //     return true;

  //   default: 
  //     return false;

  //   }// end of switch.
  // }

  // Check for System Calls or other than the application's functions.
  //
  bool isSystemCall(Function *F)
  {

    if (F->getName() == "llvm.lifetime.start")
      return true;

    else if (F->getName() == "llvm.lifetime.end")
      return true;

    else if (F->getName() == "llvm.memset.p0i8.i64")
      return true;

    else if (F->getName() == "llvm.memcpy.p0i8.p0i8.i64")
      return true;

    else if (F->getName() == "printf")
      return true;

    else if (F->getName() == "exit")
      return true;

     // H.264
    else if (F->getName() == "__assert_fail")
      return true;

    else if (F->getName() == "fwrite")
      return true;

    else if (F->getName() == "fflush")
      return true;
    else if (F->getName() == "fopen64")
      return true;

     else if (F->getName() == "fclose")
      return true;
    else if (F->getName() == "puts")
      return true;

    else if (F->getName() == "calloc")
      return true;
    else if (F->getName() == "no_mem_exit")
      return true;
    else if (F->getName() == "free_pointer")
      return true;
    else if (F->getName() == "free")
      return true;


    // H.264 - Synthesized
     else if (F->getName() == "llvm.bswap.i32")
      return true;
     else if (F->getName() == "fputc")
      return true;
     else if (F->getName() == "strlen")
      return true;
    else if (F->getName() == "fopen")
      return true;
    else if (F->getName() == "feof")
      return true;
    else if (F->getName() == "fgetc")
      return true;
    else if (F->getName() == "fseek")
      return true;
    else if (F->getName() == "fprintf")
      return true;
    else if (F->getName() == "sprintf")
      return true;
    else if (F->getName() == "system")
      return true;
     else if (F->getName() == "strcpy")
      return true;
    else if (F->getName() == "processinterMbType")  // Non synthesizable
      return true;
    else if (F->getName() == "inter_luma_double_skip")  // Non synthesizable
       return true;

    // H.264 JM-8.6 
    else if (F->getName() == "biari_init_context")  // Non synthesizable
       return true;
    else if (F->getName() == "intrapred")  // Non synthesizable
       return true;
    else if (F->getName() == "intrapred_chroma")  // Non synthesizable
       return true;
     else if (F->getName() == "itrans")  // Non synthesizable
       return true;
      else if (F->getName() == "itrans_2")  // Non synthesizable
       return true;

     
    // else if (F->getName() == "decode_main")  // Non synthesizable
    //   return true;
    //     else if (F->getName() == "ProcessSlice")  // Non synthesizable
    //   return true;
    //     else if (F->getName() == "main")  // Non synthesizable
     // return true;
     else if (F->getName() == "intrapred_luma_16x16")  // Non synthesizable
      return true;
    


    else
      return false;

  }

    bool isIndirectSystemCall(Value *SV)
  {

    if (SV->getName() == "llvm.lifetime.start")
      return true;

    else if (SV->getName() == "llvm.lifetime.end")
      return true;

    else if (SV->getName() == "llvm.memset.p0i8.i64")
      return true;

    else if (SV->getName() == "llvm.memcpy.p0i8.p0i8.i64")
      return true;

    else if (SV->getName() == "printf")
      return true;

    else if (SV->getName() == "exit")
      return true;

    // H.264
    else if (SV->getName() == "__assert_fail")
      return true;

    else if (SV->getName() == "fwrite")
      return true;

    else if (SV->getName() == "fflush")
      return true;
    else if (SV->getName() == "fopen64")
      return true;
    else if (SV->getName() == "fclose")
      return true;
    else if (SV->getName() == "puts")
      return true;
     else if (SV->getName() == "calloc")
      return true;
    else if (SV->getName() == "no_mem_exit")
      return true;
    else if (SV->getName() == "free_pointer")
      return true;

       // H.264 - Synthesized
     else if (SV->getName() == "llvm.bswap.i32")
      return true;
     else if (SV->getName() == "fputc")
      return true;
    else if (SV->getName() == "strlen")
      return true;
    else if (SV->getName() == "fopen")
      return true;
    else if (SV->getName() == "feof")
      return true;
    else if (SV->getName() == "fgetc")
      return true;
     else if (SV->getName() == "fseek")
      return true;
    else if (SV->getName() == "fpintf")
      return true;
    else if (SV->getName() == "spintf")
      return true;
    else if (SV->getName() == "system")
      return true;
    else if (SV->getName() == "strcpy")
      return true;
    else if (SV->getName() == "processinterMbType") // Non synthesizable
      return true;


    else
      return false;

  }

  //
  //
  bool structNameIsValid(llvm::Type *type) {

    if (type->getStructName() == "struct._IO_marker")
      return 0;
    if (type->getStructName() == "struct._IO_FILE")
      return 0;


    return 1;
  }

  // Gather the data of the Array type.
  //
  long int getTypeArrayData(llvm::Type *type) {

    long int array_data=0;
    int TotalNumberOfArrayElements = 1;

    while (type->isArrayTy()) {

      llvm::Type *array_type    = type->getArrayElementType();
      int NumberOfArrayElements     = type->getArrayNumElements();
      int SizeOfElement           = array_type->getPrimitiveSizeInBits();

     errs() << "\n\t Array " << *array_type << " "  << NumberOfArrayElements<< " " << SizeOfElement  << " \n ";

      TotalNumberOfArrayElements *= NumberOfArrayElements;

      if (SizeOfElement) {
        array_data = TotalNumberOfArrayElements * SizeOfElement;
        return array_data ;
      }
      else
        type = array_type;
    }
    return array_data;  
  }

  long int getTypeData(llvm::Type *type){

    long int arg_data =0;

    if ( type->isPointerTy()){
      //errs() << "\n\t Pointer Type!  " << " \n --------\n";


      llvm::Type *Pointer_Type = type->getPointerElementType();
      arg_data+=getTypeData(Pointer_Type);
    }

    // Struct Case
    else if ( type->isStructTy()) {

      long int struct_data=0;
      unsigned int NumberOfElements = type->getStructNumElements();

      for (int i=0; i<NumberOfElements; i++){

        llvm::Type *element_type = type->getStructElementType(i);
        // errs() << "\n\t Struct -- Arg: " << i << " " << *element_type << " "
        //     << type->getStructName() << " \n ";

        if (structNameIsValid(type))
          struct_data +=  getTypeData(element_type);
  
      }
      arg_data = struct_data;
      //return arg_data;    
    }

    // Scalar Case
    else if ( type->getPrimitiveSizeInBits()) {
      //errs() << "\n\t Primitive Size  " <<  type->getPrimitiveSizeInBits()  << " \n ";
      arg_data = type->getPrimitiveSizeInBits();
      //return arg_data;

    }
 
    // Vector Case
    else if ( type->isVectorTy()) {
      //errs() << "\n\t Vector  " <<  type->getPrimitiveSizeInBits()  << " \n ";
      arg_data = type->getPrimitiveSizeInBits();
      //return arg_data;
    }


    // Array Case
    else if(type->isArrayTy()) {
      arg_data = getTypeArrayData(type);
      //errs() << "\n\t Array Data " << arg_data << " \n ";
      //return arg_data;
    }

    return arg_data;
  }

  // It type an Array type? 
  //
  bool isArray(llvm::Type *type){

    if ( type->isPointerTy()){
      llvm::Type *Pointer_Type = type->getPointerElementType();
       errs() << "\n\t Pointer " << " \n ";
      isArray(Pointer_Type);
    }

    // Array Case
    else if(type->isArrayTy()) {
       errs() << "\n\t Array " << " \n ";
      return true;
    }

    else
      return false;
  }
    

  // int find_bb(std::vector<BasicBlock *> list, BasicBlock *BB) {

  //   for (unsigned i = 0; i < list.size(); i++)
  //     if (list[i] == BB)
  //       return i;
      
    
  //   return -1;
  // }
  
  // Loop Unrolling Factor Analysis
  //
  void LoopUnrollingFactorAnalysis(BasicBlock *BB, LoopInfo &LI, ScalarEvolution &SE, float BBFreq) {

    unsigned int NumberOfIterations=0;
    std::vector<Loop *> Loops;
    Loops.clear();


      if (Loop *L = LI.getLoopFor(BB)) {
      
        unsigned int loop_depth = L->getLoopDepth();


          NumberOfIterations = SE.getSmallConstantTripCount(L);
          //errs() << " Loop  Iterations :  " << LoopIterationsArray[loop_depth-1] << " \n ";

        errs() << " Loop  Info :  " <<  NumberOfIterations << " " << BBFreq<< " \n ";

      }


  }

  bool printBasicBlock(Region::block_iterator &BB) {

    errs() << "     BB Name\t\t\t:\t" << BB->getName() << " \n";

    return false;
  }


  int getInputDataLoopFunction(Function *F, LoopInfo &LI, ScalarEvolution &SE, unsigned int NumberOfLoops, unsigned int NumberOfArrays) {

    int InputData = 0; // Bits
    int InputDataBytes = 0; // Bytes
    int NumberOfLoads = 0;

    int *LoopIterationsArray   = new int[NumberOfLoops] ();
    std::string *ArrayRefNames = new std::string[NumberOfArrays] ();
    int *ArrayLoads            = new int[NumberOfArrays] ();  // Could use std::vector instead.

    //errs() << " Number of Loops  " << NumberOfLoops << "\n";

    int indexNamesArray = 0;

    std::vector<Value *> ArrayReferences;
    ArrayReferences.clear();

    for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
      BasicBlock *CurrentBlock = &*BB;
      int BBLoads = 0;
      unsigned int loop_depth =0;

      errs() << " BB Name :  " << CurrentBlock->getName() << " \n ";

      // Iterate inside the basic block.
      for(BasicBlock::iterator BI = CurrentBlock->begin(), BE = CurrentBlock->end(); BI != BE; ++BI) {


        if (Loop *L = LI.getLoopFor(CurrentBlock)) {
        
          loop_depth = L->getLoopDepth();

          // Check Number Of Loops!
          if (NumberOfLoops>=1) {

            LoopIterationsArray[loop_depth-1] = SE.getSmallConstantTripCount(L);
            //errs() << " Loop  Iterations :  " << LoopIterationsArray[loop_depth-1] << " \n ";

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

              //errs() << "  Testing Input Load 1 : " << InputLoad << "\n";  

            if (NumberOfLoops==1)
              InputLoad = InputLoad * LoopIterationsArray[loop_depth-1];
            
            else
              for (unsigned int i=0; i<loop_depth; i++)
                InputLoad += InputLoad * LoopIterationsArray[i]; // fixed += instead of =
            

             //errs() << "  Testing Input Load 2 : " << InputLoad << "\n";     

              InputData +=InputLoad;
              //errs() << "  Testing Input Data : " << InputData << "\n";
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

    InputDataBytes = InputData/8;

    return InputDataBytes;
  }

   int getOutputDataLoopFunction(Function *F, LoopInfo &LI, ScalarEvolution &SE, unsigned int NumberOfLoops) {

      int OutputData = 0;
      int OutputDataBytes = 0;
      int NumberOfStores = 0;
      int LoopIterationsArray[10]= {0};
      // std::vector<Loop *> Loops;
      // Loops.clear();

      for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
        BasicBlock *CurrentBlock = &*BB;

        // Iterate inside the basic block.
        for(BasicBlock::iterator BI = CurrentBlock->begin(), BE = CurrentBlock->end(); BI != BE; ++BI) {

          if (Loop *L = LI.getLoopFor(CurrentBlock)) {
          
            unsigned int loop_depth = L->getLoopDepth();

            // Check Number Of Loops!
            if (NumberOfLoops>1) {

              LoopIterationsArray[loop_depth-1] = SE.getSmallConstantTripCount(L);
              //errs() << " Ouput : Loop  Iterations :  " << LoopIterationsArray[loop_depth-1] << " \n ";

              // Load Info
              if(StoreInst *Store = dyn_cast<StoreInst>(&*BI)) {

              int OutputStore = Store->getOperand(0)->getType()->getPrimitiveSizeInBits();

              //errs() << "  Ouput Data : " << OutputStore << "\n";

              for (unsigned int i=0; i<loop_depth; i++)
                OutputStore += OutputStore * LoopIterationsArray[i];

              //errs() << "  Ouput Data 2 : " << OutputStore << "\n";
                  

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

      OutputDataBytes = OutputData/8;
      return OutputDataBytes;
    }


  // Returns true if Block iterator of Region contains no Call Instructions.
  //
  bool isBBCallFree(Region::block_iterator &BB) {

    // Iterate inside the basic block.
    for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
       if(dyn_cast<CallInst>(&*BI))
        return false;
    }

    return true;
  }



  // Returns the name of the function called if there is a call Instruction (Direct and indirect).
  //
  bool getBBCall(BasicBlock *BB, std::vector<Function *> Function_list) {

    //errs() << " BB Name : " << BB->getName() << "\n";

    StringRef Function_Name = BB->getParent()->getName();

    // Iterate inside the basic block.
    for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI){
       
       if(CallInst *Call = dyn_cast<CallInst>(BI)) {

        
        if (Call->getCalledFunction()) {

          if (!isSystemCall(Call->getCalledFunction())) {

            StringRef Called_Name = Call->getCalledFunction()->getName();
          
            errs() << "\t" << Function_Name << "\t -->\t" << Called_Name << "\n";


          }
        }
          //errs() << "\t Call Name :   " << Call->getCalledFunction()->getName() << "\n\n";
        
        else{

          Value* V=Call->getCalledValue();
          Value* SV = V->stripPointerCasts();

          if (!isIndirectSystemCall(SV) ) {

              StringRef Indirect_Called_Name = SV->getName();
        
              errs() <<"\t" << Function_Name << "\t -->\t" << Indirect_Called_Name << "\n";

          }
        }
        // errs() << "\t call operand 0 :   " << *Call->getOperand(0) << "\n"  ;
       }
     } // End of For

    return true;
  }

  // Get Area Estimation of BB in μM^2.
  //
  unsigned int getAreaOfBBInUMSq(BasicBlock *BB) {

    unsigned int AreaOfBB = 0;

    // Iterate inside the basic block.
    for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI)
      AreaOfBB += getAreaEstimInUMSq(&*BI); 

    return AreaOfBB;
  }



  // Region Function - Might remove it.
  //
  bool runOnBasicBlock(Region::block_iterator &BB, unsigned int *DFGNodesRegion, unsigned int *GoodDFGNodesRegion, unsigned int *OptimalityRegion) {

      unsigned int GoodDFGNodesBB = 0;
      unsigned int GoodnessBB = 0;
      int32_t freq = 0; 

      // Iterate inside the basic block.
      for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {
        
        *DFGNodesRegion = *DFGNodesRegion + 1;

        if (!isMarked(&*BI)) {
          *GoodDFGNodesRegion = *GoodDFGNodesRegion + 1;
          GoodDFGNodesBB++;
        }
      }

      if (MDNode *node = BB->getTerminator()->getMetadata("freq")) {

        if (MDString::classof(node->getOperand(0))) {
          auto mds = cast<MDString>(node->getOperand(0));
          std::string metadata_str = mds->getString();
          freq = std::stoi(metadata_str);
        }

      }

      GoodnessBB = GoodDFGNodesBB * freq ;
      *OptimalityRegion = *OptimalityRegion + GoodnessBB; // Density

      // errs() << "     Good Nodes of BB are\t:\t" << GoodDFGNodesBB << " \n";
      // errs() << "     Frequency of BB is  \t:\t" << freq << " \n";
      // errs() << "     Goodness of BB is   \t:\t" << GoodnessBB << " \n\n";
      
      //++BBCounter;
      return false;
  }

  // Get Area Estimation for a Block iterator of a Region.
  unsigned int getAreaOfBB(Region::block_iterator &BB) {

    unsigned int AreaOfBB = 0;

    // Iterate inside the basic block.
    for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI)
      AreaOfBB += getAreaEstim(&*BI);

    return AreaOfBB;
  }

    // Get Area Estimation for a Block iterator of a Function.
  unsigned int getAreaOfBBInFunction(Function::iterator &BB) {

    unsigned int AreaOfBB = 0;

    // Iterate inside the basic block.
    for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI)
      AreaOfBB += getAreaEstim(&*BI);

    return AreaOfBB;
  }

    // Get the Area extimation of a Function in LUTs.
  //
  unsigned int getAreaofFunction(Function *F){

    unsigned int AreaofFunction = 0;

    for(Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
      //AreaofFunction += getAreaOfBBInUMSq(&*BB);
      AreaofFunction += getAreaOfBBInFunction(BB);

    return AreaofFunction;
  }

  // Get the Area extimation of a Region in LUTs.
  unsigned int getAreaofRegion(Region *R){

    unsigned int AreaOfRegion = 0;

    for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB)
      AreaOfRegion += getAreaOfBB(BB);

    return AreaOfRegion;

  }

  
  // Region Function - Might remove it.
  //
  bool getGoodnessAndDensityOfRegionInBB(Region::block_iterator &BB, unsigned int *DFGNodesRegion, unsigned int *GoodDFGNodesRegion, unsigned int *OptimalityRegion) {

    unsigned int GoodDFGNodesBB = 0;
    unsigned int GoodnessBB = 0;
    int32_t freq = 0; 

    // Iterate inside the basic block.
    for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {
      
      *DFGNodesRegion = *DFGNodesRegion + 1;

      if (!isMarked(&*BI)) {
        *GoodDFGNodesRegion = *GoodDFGNodesRegion + 1;
        GoodDFGNodesBB++;
      }
    }

    if (MDNode *node = BB->getTerminator()->getMetadata("freq")) {

      if (MDString::classof(node->getOperand(0))) {
        auto mds = cast<MDString>(node->getOperand(0));
        std::string metadata_str = mds->getString();
        freq = std::stoi(metadata_str);
      }

    }

    GoodnessBB = GoodDFGNodesBB * freq ;
    *OptimalityRegion = *OptimalityRegion + GoodnessBB; 
    
    return false;
  }

  // Gather the number of BBs in a Region.
  unsigned int getBBsOfRegion(Region *R) {

    unsigned int BBsofRegion=0;

    for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB)
      BBsofRegion++;

    return BBsofRegion;
  }


  // Gather the number of DFG Nodes in a Region.
  unsigned int getDFGNodesOfRegion(Region *R) {

    unsigned int DFGNodes=0;

    for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB){

      for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI)
        DFGNodes++;
    }

    return DFGNodes;
  }

  // Gather the number of Loops in a Region.
  unsigned int getLoopsOfRegion(Region *R, LoopInfo &LI) {

    unsigned int NumberOfLoops=0;
    std::vector<Loop *> Loops;
    Loops.clear();

    for(Region::block_iterator BB = R->block_begin(), E = R->block_end(); BB != E; ++BB){

      if (Loop *L = LI.getLoopFor(*BB)){

      if (find_loop(Loops, L) == -1 ){ 
          Loops.push_back(L);
          NumberOfLoops++;
        }
      }
    }

    return NumberOfLoops;
  }

}
