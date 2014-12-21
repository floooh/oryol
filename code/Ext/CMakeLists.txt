#
#   external libraries
#
oryol_add_subdirectory(UnitTest++)
oryol_add_subdirectory(ConvertUTF)
oryol_add_subdirectory(zlib)
if (ORYOL_ANDROID)
  oryol_add_subdirectory(android_native)
endif()
if (ORYOL_LINUX OR ORYOL_WINDOWS OR ORYOL_MACOS)
  oryol_add_subdirectory(flextgl)
endif()
