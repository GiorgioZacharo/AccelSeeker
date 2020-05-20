//===------------------------- AccelSeeker.h -------------------------===//
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
//
// AccelSeeker Candidates.
//
//===----------------------------------------------------------------------===//

using namespace llvm;

std::ofstream myfile; // File that Region Info are written.

#define M_AXI_ARRAY 700                     // LUTs per M_AXI bus array.
#define NSECS_PER_CYCLE         10         // nSecs Per Cycle 100 MHz.
#define SYS_AWARE

namespace {

static std::string GetValueName(const Value *V) {
  if (V) {
    std::string name;
    raw_string_ostream namestream(name);
    V->printAsOperand(namestream, false);
    return namestream.str();
  } else
    return "[null]";
}

  int find_inst(std::vector<Instruction *> list, Instruction *inst) {

    for (unsigned i = 0; i < list.size(); i++)
      if (list[i] == inst)
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


  float get_max(std::vector<float> DelayPaths) {

    float max =0;

    for (unsigned i = 0; i < DelayPaths.size(); i++) {
      
      if (DelayPaths[i] > max)
        max = DelayPaths[i];
    }
  
    return max;
  }


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
    
    //else if (F->getName() == "total_zeros")  // Non synthesizable
//	return true;

   else if (GetValueName(F) == "@0")  // Non synthesizable
      return true;
   else if (GetValueName(F) == "@1")  // Non synthesizable
      return true;
   else if (GetValueName(F) == "@2")  // Non synthesizable
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

  //===---------------------------------------------------===//
  //
  //  Delay SW Estimation for each DFG Node/Istruction in Cycles. **NEW FEATURE**
  //
  //===---------------------------------------------------===//

  float getCycleSWDelayEstim(Instruction *Inst)    
   
  {

    switch (Inst->getOpcode()) {

    case Instruction::GetElementPtr: 
      return 0;

    case Instruction::Br:
      return 1;

    case Instruction::Alloca:
      return 1;

    case Instruction::PHI:
      return 1;

    case Instruction::Store:
      return 1;

    case Instruction::Load:
      return 1;

    case Instruction::Call:
      return 1;

    case Instruction::Fence:
      return 1;

    case Instruction::LandingPad:
      return 1;

    case Instruction::AtomicCmpXchg:
      return 1;

    case Instruction::AtomicRMW: 
      return 1;

    case Instruction::ExtractValue: 
      return 1;

    case Instruction::InsertValue:
      return 1;

    case Instruction::Switch: // Similar as branching.
      return 1;
    
    case Instruction::IndirectBr: 
      return 1;

    case Instruction::Invoke: 
      return 1;

    case Instruction::Resume: 
      return 1;

    case Instruction::Unreachable: 
      return 0;

    case Instruction::Ret:
      return 1;

    case Instruction::ShuffleVector: // * more complex *
      return 1;

    case Instruction::ExtractElement:
      return 1;

    case Instruction::InsertElement:
      return 1;

    case Instruction::Add:
      return 1;

    case Instruction::FAdd:
      return 2;

    case Instruction::Sub:
      return 1;

    case Instruction::FSub:  // This is not modelled properly (Double as rough estimation)
      return 2;

    case Instruction::Mul:
      return 1;

    case Instruction::FMul: // This is not modelled properly (Double as rough estimation)

    case Instruction::UDiv:
      return 6;

    case Instruction::SDiv:
      return 6;

    case Instruction::FDiv: // This is not modelled properly (Double as rough estimation)
      return 12;

    case Instruction::URem:
      return 6;

    case Instruction::SRem:
      return 6;

    case Instruction::FRem: // This is not modelled properly (Double as rough estimation)
      return 12;

    case Instruction::Shl: {

      Value *Operand_two = Inst->getOperand(1);

      if (Operand_two->getType()->isSingleValueType()) // Shift by a single value (e.g. integer)
        return 0;

      return 1;
    
    }
    
    case Instruction::LShr: {
      
      Value *Operand_two = Inst->getOperand(1);

      if (Operand_two->getType()->isSingleValueType()) // Shift by a single value (e.g. integer)
        return 0;
      
      return 1;
    }

    case Instruction::AShr:{
      
      Value *Operand_two = Inst->getOperand(1);

      if (Operand_two->getType()->isSingleValueType()) // Shift by a single value (e.g. integer)
        return 0;
      
      return 1;
    }

    case Instruction::And:
      return 1;

    case Instruction::Or:
      return 1;

    case Instruction::Xor:
      return 1;

    case Instruction::Select: // This is my estimation!
      return 1;

    case Instruction::ICmp: // Check type of ICmp. (Equality or Relational) 
    {
      ICmpInst *Icmp = dyn_cast<ICmpInst>(&*Inst);

      if (Icmp->isEquality())
        return 1;
      else
        return 1;       
    }

    case Instruction::FCmp: // This is my estimation!
      return 1;

    case Instruction::ZExt:
      return 0;

    case Instruction::SExt:
      return 0;

    case Instruction::FPToUI:
      return 0;

    case Instruction::FPToSI:
      return 0;

    case Instruction::FPExt:
      return 0;

    case Instruction::PtrToInt:
      return 0;

    case Instruction::IntToPtr:
      return 0;

    case Instruction::SIToFP:
      return 0;

    case Instruction::UIToFP:
      return 0;

    case Instruction::Trunc:
      return 0;

    case Instruction::FPTrunc:
      return 0;

    case Instruction::BitCast:
      return 0;

    default:  // Assume default as 0.
      return 0;

    }// end of switch.
  }

  //===---------------------------------------------------===//
  //
  //
  //  Delay HW Estimation for each DFG Node/Istruction in nSecs.
  //
  //
  //===---------------------------------------------------===//

  float getDelayEstim(Instruction *Inst)
  
  {

    switch (Inst->getOpcode()) {

    case Instruction::GetElementPtr: 
      return 0;

    case Instruction::Br:
      return 0;

    case Instruction::Alloca:
      return 0;

    case Instruction::PHI:
    // #ifdef OLD_SETUP
    //   return 5.299;
    // #endif
    #ifdef SYS_AWARE
      return 4.3;
    #else
      return 0.23;
    #endif    

    case Instruction::Store:
      return 0;

    case Instruction::Load:
      return 0;

    case Instruction::Call:
      return 0;

    case Instruction::Fence:
      return 0;

    case Instruction::LandingPad:
      return 0;

    case Instruction::AtomicCmpXchg:
      return 0;

    case Instruction::AtomicRMW: 
      return 0;

    case Instruction::ExtractValue: 
      return 0;

    case Instruction::InsertValue:
      return 0;

    case Instruction::Switch: // ceil(log2(Number of Cases)) * 5.2999
    {

      SwitchInst *Switch = dyn_cast<SwitchInst>(&*Inst);
      unsigned int NumCases = Switch->getNumCases();
      // #ifdef OLD_SETUP
      //   return ceil(log2(NumCases)) * 5.2999 ;
      // #endif
      #ifdef SYS_AWARE
        return ceil(log2(NumCases)) * 4.3 ;
      #else
      return ceil(log2(NumCases)) * 0.23 ;
      #endif
    }
      
    case Instruction::IndirectBr: 
      return 0;

    case Instruction::Invoke: 
      return 0;

    case Instruction::Resume: 
      return 0;

    case Instruction::Unreachable: 
      return 0;

    case Instruction::Ret:
      return 0;

    case Instruction::ShuffleVector:
      return 0;

    case Instruction::ExtractElement:
      return 0;

    case Instruction::InsertElement:
      return 0;


    case Instruction::Add:
      // #ifdef OLD_SETUP
      //   return 9.323;
      // #endif
      #ifdef SYS_AWARE
        return 5.3;
      #else
      return 0.92;
      #endif

    case Instruction::FAdd: // This is not modelled properly. Or mark as forbidden.
      // #ifdef OLD_SETUP
      //   return 9.323;
      // #endif
      #ifdef SYS_AWARE
        return 5.3;
      #else
      return 0.92;
      #endif

    case Instruction::Sub:
      // #ifdef OLD_SETUP
      //   return 9.323;
      // #endif
      #ifdef SYS_AWARE
        return 5.3;
      #else
      return 0.92;
      #endif

    case Instruction::FSub: // This is not modelled properly. Or mark as forbidden.
      // #ifdef OLD_SETUP
      //   return 9.323;
      // #endif
      #ifdef SYS_AWARE
        return 5.3;
      #else
      return 0.92;
      #endif

    case Instruction::Mul:
    // #ifdef OLD_SETUP
    //   return 9.9;
    // #endif
      #ifdef SYS_AWARE
        return 8.5;
      #else
        return 1;
      #endif

    case Instruction::FMul:  // This is not modelled proerly. Or mark as forbidden.
      // #ifdef OLD_SETUP
      //   return 9.9;
      // #endif
      #ifdef SYS_AWARE
        return 8.5;
      #else
      return 1;
      #endif

    case Instruction::UDiv:
      // #ifdef OLD_SETUP
      //   return 59.535;
      // #endif
      #ifdef SYS_AWARE
        return 49.5;
      #else
      return 3.76;
      #endif
    
    case Instruction::SDiv:
      // #ifdef OLD_SETUP
      //   return 59.7;
      // #endif
      #ifdef SYS_AWARE
        return 53;
      #else
      return 3.76;
      #endif

    case Instruction::FDiv:
      // #ifdef OLD_SETUP
      //   return 59.7;
      // #endif
      #ifdef SYS_AWARE
        return 53;
      #else
      return 3.76;
      #endif

    case Instruction::URem:
      // #ifdef OLD_SETUP
      //   return 59.636;
      // #endif
      #ifdef SYS_AWARE
        return 52.6;
      #else
      return 4.04;
      #endif

    case Instruction::SRem:
      // #ifdef OLD_SETUP
      //   return 59.636;
      // #endif
      #ifdef SYS_AWARE
        return 55.4;
      #else
      return 4.04;
      #endif

    case Instruction::FRem:
      // #ifdef OLD_SETUP
      //   return 59.636;
      // #endif
      #ifdef SYS_AWARE
        return 55.4;
      #else
      return 4.04;
      #endif

    case Instruction::Shl:{

      Value *Operand_two = Inst->getOperand(1);

      if (Operand_two->getType()->isSingleValueType()) // Shift by a single value (e.g. integer)
        return 0;

      // #ifdef OLD_SETUP
      //   return 8.862;
      // #endif
      #ifdef SYS_AWARE
        return 5.5;
      #else
      return 0.71;
      #endif
    
    }
    
    case Instruction::LShr: {
      
      Value *Operand_two = Inst->getOperand(1);

      if (Operand_two->getType()->isSingleValueType()) // Shift by a single value (e.g. integer)
        return 0;
      
      // #ifdef OLD_SETUP
      //   return 8.862;
      // #endif
      #ifdef SYS_AWARE
        return 5.5;
      #else
      return 0.73;
      #endif
    }

    case Instruction::AShr:{
      
      Value *Operand_two = Inst->getOperand(1);

      if (Operand_two->getType()->isSingleValueType()) // Shift by a single value (e.g. integer)
        return 0;
      
      // #ifdef OLD_SETUP
      //   return 9.052;
      // #endif
      #ifdef SYS_AWARE
        return 6.6;
      #else
      return 0.65;
      #endif
    }

    case Instruction::And:
      // #ifdef OLD_SETUP
      //   return 8.862;
      // #endif
      #ifdef SYS_AWARE
        return 4.3;
      #else
      return 0.02;
      #endif

    case Instruction::Or:
      // #ifdef OLD_SETUP
      //   return 8.862;
      // #endif
      #ifdef SYS_AWARE
        return 4.3;
      #else
      return 0.03;
      #endif

    case Instruction::Xor:
      // #ifdef OLD_SETUP
      //   return 8.862;
      // #endif
      #ifdef SYS_AWARE
        return 4.3;
      #else
      return 0.03;
      #endif

    case Instruction::Select: // Same as Phi.
      // #ifdef OLD_SETUP
      //   return 5.299;
      // #endif
      #ifdef SYS_AWARE
        return 4.3;
      #else
      return 0.23;
      #endif

    case Instruction::ICmp: // Check type of ICmp. (Equality or Relational) 
    {
      ICmpInst *Icmp = dyn_cast<ICmpInst>(&*Inst);

      if (Icmp->isEquality())
        // #ifdef OLD_SETUP
        //   return 9.276;
        // #endif
        #ifdef SYS_AWARE
          return 5;
        #else
        return 0.15;
        #endif
      else
        // #ifdef OLD_SETUP
        //   return 9.389;
        // #endif
       //#ifdef SYS_AWARE
      //    return 4.8;
       return 0;
        //#else
        //  return 0.37;
        //#endif      
    }

    case Instruction::FCmp: // This is my estimation!
      // #ifdef OLD_SETUP
      //   return 9.276;
      // #endif
      #ifdef SYS_AWARE
        return 5;
      #else
      return 0.15;
      #endif

    case Instruction::ZExt:
      return 0;

    case Instruction::SExt:
      return 0;

    case Instruction::FPToUI:
      return 0;

    case Instruction::FPToSI:
      return 0;

    case Instruction::FPExt:
      return 0;

    case Instruction::PtrToInt:
      return 0;

    case Instruction::IntToPtr:
      return 0;

    case Instruction::SIToFP:
      return 0;

    case Instruction::UIToFP:
      return 0;

    case Instruction::Trunc:
      return 0;

    case Instruction::FPTrunc:
      return 0;

    case Instruction::BitCast:
      return 0;

    default:  // Assume default as 0 LUTs in Area estimation.
      return 0;

    }// end of switch.
  }

  //Area Estimation for each DFG Node/Istruction in LUTs
// RegionSeeker  
unsigned int getAreaEstim(Instruction *Inst)
  {

    switch (Inst->getOpcode()) {

    case Instruction::GetElementPtr: 
      return 0;

    case Instruction::Br:
      return 0;

    case Instruction::Alloca:
      return 0;

    case Instruction::PHI:
      #ifdef SYS_AWARE
        return 16;
      #else
        return 33;
      #endif

    case Instruction::Store:
      return 0;

    case Instruction::Load:
      return 0;

    case Instruction::Call:
      return 0;

    case Instruction::Fence:
      return 0;

    case Instruction::LandingPad:
      return 0;

    case Instruction::AtomicCmpXchg:
      return 0;

    case Instruction::AtomicRMW: 
      return 0;

    case Instruction::ExtractValue: 
      return 0;

    case Instruction::InsertValue:
      return 0;

    case Instruction::Switch: // 33 * N (Number of Cases)
    {

      SwitchInst *Switch = dyn_cast<SwitchInst>(&*Inst);
      //errs() << " Number of switches is  " << Switch->getNumCases() << "\t";
      unsigned int NumCases = Switch->getNumCases();
      #ifdef SYS_AWARE
        return 16 * NumCases;
      #else

        return 33 * NumCases;
      #endif
    }
      
    case Instruction::IndirectBr: 
      return 0;

    case Instruction::Invoke: 
      return 0;

    case Instruction::Resume: 
      return 0;

    case Instruction::Unreachable: 
      return 0;

    case Instruction::Ret:
      return 0;

    case Instruction::ShuffleVector:
      return 0;

    case Instruction::ExtractElement:
      return 0;

    case Instruction::InsertElement:
      return 0;

    case Instruction::Add:
      #ifdef SYS_AWARE
        return 32;
      #else
        return 33;
      #endif

    case Instruction::FAdd:
      #ifdef SYS_AWARE
        return 32;
      #else
        return 33;
      #endif

    case Instruction::Sub:
      #ifdef SYS_AWARE
        return 32;
      #else
        return 33;
      #endif

    case Instruction::FSub:
      #ifdef SYS_AWARE
        return 32;
      #else
        return 33;
      #endif

    case Instruction::Mul:
      #ifdef SYS_AWARE
        //return 267;
     errs() << "Mul" << "\n";
        return 0;
      #else
        return 618;
      #endif
      
    case Instruction::FMul:
      #ifdef SYS_AWARE
        //return 267;
     errs() << "FMul" << "\n";
        return 0;
      #else
        return 618;
      #endif

    case Instruction::UDiv:
      #ifdef SYS_AWARE
       // return 1055;
    errs() << "UDiv" << "\n";
    return 320;
      #else
        return 1056;
      #endif

    case Instruction::SDiv:
      #ifdef SYS_AWARE
         // return 1214;
    errs() << "SDiv" << "\n";
    return 320;
      #else
        return 1185;
      #endif

    case Instruction::FDiv:
      #ifdef SYS_AWARE
        // return 1214;
    errs() << "FDiv" << "\n";
    return 320;
      #else
        return 1185;
      #endif

    case Instruction::URem:
      #ifdef SYS_AWARE
        // return 1122;
     errs() << "URem" << "\n";
     return 320;
      #else
        return 1312;
      #endif

    case Instruction::SRem:
      #ifdef SYS_AWARE
       // return 1299;
     errs() << "SRem" << "\n";
     return 320;
      #else
        return 1312;
      #endif

    case Instruction::FRem:
      #ifdef SYS_AWARE
        //return 1299;
     errs() << "FRem" << "\n";
        return 320;
      #else
        return 1312;
      #endif

    case Instruction::Shl:{

      Value *Operand_two = Inst->getOperand(1);

      if (Operand_two->getType()->isSingleValueType()) // Shift by a single value (e.g. integer)
        return 0;

      #ifdef SYS_AWARE
        return 79;
      #else
        return 103;
      #endif
    
    }
    
    case Instruction::LShr: {
      
      Value *Operand_two = Inst->getOperand(1);

      if (Operand_two->getType()->isSingleValueType()) // Shift by a single value (e.g. integer)
        return 0;
      
         #ifdef SYS_AWARE
        return 79;
      #else
        return 101;
      #endif
    }

    case Instruction::AShr:{
      
      Value *Operand_two = Inst->getOperand(1);

      if (Operand_two->getType()->isSingleValueType()) // Shift by a single value (e.g. integer)
        return 0;
      
      #ifdef SYS_AWARE
        return 99;
      #else
        return 145;
      #endif
    }

    case Instruction::And:
      #ifdef SYS_AWARE
        return 32;
      #else
        return 33;
      #endif

    case Instruction::Or:
      #ifdef SYS_AWARE
        return 32;
      #else
        return 33;
      #endif

    case Instruction::Xor:
      #ifdef SYS_AWARE
        return 32;
      #else
        return 33;
      #endif

    case Instruction::Select: // This is my estimation!
      #ifdef SYS_AWARE
        return 16;
      #else
        return 33;
      #endif

    case Instruction::ICmp: // Check type of ICmp. (Equality or Relational) 
    {
      ICmpInst *Icmp = dyn_cast<ICmpInst>(&*Inst);

      if (Icmp->isEquality())
        #ifdef SYS_AWARE
          return 11;
        #else
          return 12;
        #endif
      else
        #ifdef SYS_AWARE
          return 16;
        #else
          return 17;
        #endif      
    }

    case Instruction::FCmp: // This is my estimation!
      #ifdef SYS_AWARE
        return 16;
      #else
        return 17;
      #endif 

    case Instruction::ZExt:
      return 0;

    case Instruction::SExt:
      return 0;

    case Instruction::FPToUI:
      return 0;

    case Instruction::FPToSI:
      return 0;

    case Instruction::FPExt:
      return 0;

    case Instruction::PtrToInt:
      return 0;

    case Instruction::IntToPtr:
      return 0;

    case Instruction::SIToFP:
      return 0;

    case Instruction::UIToFP:
      return 0;

    case Instruction::Trunc:
      return 0;

    case Instruction::FPTrunc:
      return 0;

    case Instruction::BitCast:
      return 0;

    default:  // Assume default as 0 LUTs in Area estimation.
      return 0;

    }// end of switch.
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

    // SW Cost in Cycles estimation for a single BB. (Without Frequency of each BB)
  //
  long int getSWCostOfBB(BasicBlock *BB) {

    long int SWCostBB = 0;

    for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI)
      SWCostBB += getCycleSWDelayEstim(&*BI);


    return SWCostBB;
  }


 // Compute the Critical Path of HW for Delay inside the BB of a Region or Function in nSecs.
  //
  //
  float getDelayOfBB(BasicBlock *BB) {

    float DelayOfBB = 0;
    std::vector<Instruction *> worklist, send_node, receive_node;
    std::vector<float> DelayPaths, DelayNodes;

    worklist.clear();
    send_node.clear();
    receive_node.clear();
    DelayPaths.clear();
    DelayNodes.clear();

    // Iterate inside the basic block and gather all DFG Nodes.
    for(BasicBlock::iterator BI = BB->begin(), BE = BB->end(); BI != BE; ++BI) {
      worklist.push_back(&*BI);
      DelayPaths.push_back(getDelayEstim(&*BI)); //Initialize the Delay Estimation for each DFG Node.
      DelayNodes.push_back(getDelayEstim(&*BI)); //Initialize the Delay Estimation for each DFG Node.
    }
    
    // Find Relation among DFG Nodes.
    //
    // Send_Node --> Receive_Node
    //
    for (std::vector<Instruction *>::iterator iter = worklist.begin(); iter != worklist.end(); ++iter) {

      if(Instruction *Inst = *iter) {

        // Iterate over each operand of each Instruction.
        for (unsigned int i=0; i<Inst->getNumOperands(); i++) {

          Value *Operand = Inst->getOperand(i);

          if (PHINode *phi = dyn_cast<PHINode>(&*Inst) )
            if (phi->getIncomingBlock(i) != BB)
              continue;

          int count =0;  
          // Iterate over all the instructions of the Region and compare the operand to them.
          for (std::vector<Instruction *>::iterator instruction_iterator = worklist.begin(); instruction_iterator != worklist.end(); ++instruction_iterator, count++) {

            if(count <= find_inst(worklist, Inst) ) { // Might need to remove "=""

              if(Instruction * Inst_source = *instruction_iterator) {
                if (Operand == Inst_source) {

                  send_node.push_back(Inst_source); // Populate send_node vector
                  receive_node.push_back(Inst); // Populate receive_node vector
                }
              }
            }
          }
        }
      }
    }


      // for (int i=0; i< send_node.size(); i++)
      //   errs() << " sending Nodes " << *send_node[i] << "\n"; // My debugging Info!
           
      //      for (int i=0; i< receive_node.size(); i++)
      //         errs() << " REceiving Nodes " << *receive_node[i] << "\n"; // My debugging Info!

      // errs() << "\n\n"  ;

      // for (int i=0; i< send_node.size(); i++) {

      //   errs() << " Edges " << *send_node[i] << "  --->     " <<  *receive_node[i] << "\n"; // My debugging Info!


      // }


    if (send_node.size() > 0 && receive_node.size() >0) {

      // Critical Path Estimation. 
      //       
      // 
      for (std::vector<Instruction *>::iterator instr_iter = worklist.begin(); instr_iter != worklist.end(); ++instr_iter) {

        Instruction *instr_temp = *instr_iter;

        // Find the end Nodes (Bottom-most Nodes in the DFG Graph)
        if (find_inst(send_node, instr_temp) == -1) {

          Instruction *EndNode = instr_temp;
          std::vector<Instruction *> BottomNodes;
          BottomNodes.clear();
          BottomNodes.push_back(EndNode);

          Instruction *CurrentNode;
          int position =0;

          //float delay_path_estimation;

          while(BottomNodes.size() >0) {  

            CurrentNode = BottomNodes[0];

            while (find_inst(receive_node, CurrentNode) >=0) {

              position = find_inst(receive_node, CurrentNode); 

              DelayPaths[find_inst(worklist, send_node[position])] = std::max( DelayPaths[find_inst(worklist, send_node[position])] , DelayNodes[find_inst(worklist, send_node[position])] +  DelayPaths[find_inst(worklist, CurrentNode)]  );
              Instruction *Predecessor = send_node[position];

              receive_node.erase(receive_node.begin() + position);   // deleting the last edge
              send_node.erase(send_node.begin() + position);        //  deleting the last edge

              if (find_inst(send_node, Predecessor) == -1)
                BottomNodes.push_back(Predecessor);

            }

            BottomNodes.erase(BottomNodes.begin()); // Delete the First element of the BottomNodes list.

            
          }
        }
      }

      DelayOfBB = get_max(DelayPaths); // Get the maximum Value of Delay within a BB.
    }

    else
      for (unsigned i = 0; i < worklist.size(); i++)
        DelayOfBB += getDelayEstim(worklist[i]);

   #ifdef LOAD_AND_STORE_IN_DELAY_Of_BB   
     // Get Loads and Stores in the BB.   
     int LoadsAndStores   = getNumberofLoadsandStores(BB);
     float LoadStoreDelay = LoadsAndStores * 10;

     //errs() << " Loads/Stores :  " << LoadsAndStores << "\n";
     
     // Compare Memory to Computation Delay
     if (LoadStoreDelay > DelayOfBB)
      DelayOfBB = LoadStoreDelay;
  #endif

    //errs() << " Delay Estimation for BB is : " << format("%.8f", DelayOfBB) << "\n";
      
    // for (int i=0; i< DelayPaths.size(); i++)
    //   errs() << " Delays " << format("%.8f", DelayPaths[i]) << "\n"; // My debugging Info!

    return DelayOfBB;
  }

}
