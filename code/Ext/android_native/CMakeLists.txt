#
# helper code from the Android NDK
#
if (FIPS_ANDROID)
    fips_begin_lib(android_native)
        fips_files(android_native_app_glue.c android_native_app_glue.h)
    fips_end_lib(android_native)
endif()
