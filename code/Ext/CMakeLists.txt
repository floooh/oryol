#
#   external libraries
#
oryol_add_subdirectory(UnitTest++)
oryol_add_subdirectory(ConvertUTF)
oryol_add_subdirectory(zlib)
oryol_add_subdirectory(glew)
if (ORYOL_ANDROID)
  oryol_add_subdirectory(android_native)
endif()
