# If we don't need RTTI or EH, there's no reason to export anything
# from the hello plugin.

if(WIN32 OR CYGWIN)
  set(LLVM_LINK_COMPONENTS Core Support)
endif()

#add_llvm_library( IdentifyRegionsSA MODULE
add_llvm_library( AccelSeeker MODULE
  AccelSeeker.cpp

  DEPENDS
  intrinsics_gen
  #PLUGIN_TOOL
  #opt
  )
