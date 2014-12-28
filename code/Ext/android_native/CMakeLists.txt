#
# helper code from the Android NDK
#
if (FIPS_ANDROID)
    fips_begin_lib(android_native)
        fips_sources(.)
    fips_end_lib(android_native)
endif()
