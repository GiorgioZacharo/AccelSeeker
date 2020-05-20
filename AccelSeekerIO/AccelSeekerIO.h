//===------------------------- AccelSeekerIO.h -------------------------===//
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


using namespace llvm;

std::ofstream IO_file; // File that I/O info is written.
std::ofstream myfile; // File that I/O info is written.


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

  //
  //
  bool structNameIsValid(llvm::Type *type) {

    if (type->getStructName() == "struct._IO_marker")
      return 0;
    if (type->getStructName() == "struct._IO_FILE")
      return 0;


    return 1;
  }


  // Get the data of the Array type.
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

  // Get the data coming from a Ptr type.
  //
  long int getTypeDataPtr(llvm::Type *type){

    long int arg_data =0;

    // Struct Case
    if ( type->isStructTy()) {

      long int struct_data=0;
      unsigned int NumberOfElements = type->getStructNumElements();

      for (unsigned int i=0; i<NumberOfElements; i++){

        llvm::Type *element_type = type->getStructElementType(i);
        // errs() << "\n\t Struct -- Arg: " << i << " " << *element_type << " "
        //     << type->getStructName() << " \n ";

        if (structNameIsValid(type))
          struct_data +=  getTypeDataPtr(element_type);
  
      }
      arg_data = struct_data;
      //return arg_data;    
    }

    // Scalar Case
    else if ( type->getPrimitiveSizeInBits()) {
      errs() << "\n\t Primitive Size  " <<  type->getPrimitiveSizeInBits()  << " \n ";
      arg_data = type->getPrimitiveSizeInBits();
      //return arg_data;

    }
 
    // Vector Case
    else if ( type->isVectorTy()) {
      errs() << "\n\t Vector  " <<  type->getPrimitiveSizeInBits()  << " \n ";
      arg_data = type->getPrimitiveSizeInBits();
      //return arg_data;
    }


    // Array Case
    else if(type->isArrayTy()) {
      arg_data = getTypeArrayData(type);
      errs() << "\n\t Array Data " << arg_data << " \n ";
      //return arg_data;
    }

    return arg_data;
  }

  // Get the data of teh type.
  //
  long int getTypeData(llvm::Type *type){

    long int arg_data =0;

    if ( type->isPointerTy()) {
     errs() << "\n\t Pointer Type!  " << " \n --------\n";
     unsigned int ptr_instances = 0;
     // bool flag = false;
     //errs() << "\n\t Pointer Type 2!  " << " \n --------\n";	
      llvm::Type *Pointer_Type = type->getPointerElementType();

     //errs() << "\n\t Pointer Type 3!  " << " \n --------\n";
      while (Pointer_Type->isPointerTy()) {
	//flag = true;
	ptr_instances++;
     errs() << "\n\t Pointer Type 4!  " << " \n --------\n";
	Pointer_Type = Pointer_Type->getPointerElementType();

	// errs() << "\n\t Pointer Type!  " << ptr_instances << " \n --------\n";

	// Safe break - may need t obe reviewed/removed
	if (ptr_instances > 8)
		return  arg_data;	
      }
     	//errs() << "\n\t Pointer Type 5!  " << " \n --------\n";
     //if (Pointer_Type->isPointerTy())
     	arg_data+=getTypeDataPtr(Pointer_Type);

    }

    // Struct Case
    else if ( type->isStructTy()) {

      long int struct_data=0;
      unsigned int NumberOfElements = type->getStructNumElements();

      for (unsigned int i=0; i<NumberOfElements; i++){

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
      errs() << "\n\t Primitive Size  " <<  type->getPrimitiveSizeInBits()  << " \n ";
      arg_data = type->getPrimitiveSizeInBits();
      //return arg_data;

    }
 
    // Vector Case
    else if ( type->isVectorTy()) {
      errs() << "\n\t Vector  " <<  type->getPrimitiveSizeInBits()  << " \n ";
      arg_data = type->getPrimitiveSizeInBits();
      //return arg_data;
    }


    // Array Case
    else if(type->isArrayTy()) {
      arg_data = getTypeArrayData(type);
      errs() << "\n\t Array Data " << arg_data << " \n ";
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

      return false;
  }
    