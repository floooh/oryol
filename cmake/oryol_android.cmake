#-------------------------------------------------------------------------------
#   oryol_android.cmake
#   Helper functions for building and deploying Android apps.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#   oryol_android_create_project
#   Create Android project wrapper for a given target.
#
macro(oryol_android_create_project target)
    
    # call the android SDK tool to create a new project
    execute_process(COMMAND ${ANDROID_SDK_TOOL} create project
                    --path ${CMAKE_CURRENT_BINARY_DIR}/android
                    --target ${ANDROID_PLATFORM}
                    --name ${target}
                    --package com.oryol.${target}
                    --activity DummyActivity
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    # set the output directory for the .so files to point to the android project's 'lib/[cpuarch] directory
    set(ANDROID_SO_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/android/libs/${ANDROID_NDK_CPU})
    set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${ANDROID_SO_OUTDIR})
    set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${ANDROID_SO_OUTDIR})
    set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${ANDROID_SO_OUTDIR})

    # copy shared libraries over from the Android toolchain directory
    # FIXME: this should be automated as post-build-step by invoking the ndk-depends command
    # to find out the .so's, and copy them over
    file(COPY ${ANDROID_SYSROOT_LIB}/libm.so DESTINATION ${ANDROID_SO_OUTDIR})
    file(COPY ${ANDROID_SYSROOT_LIB}/liblog.so DESTINATION ${ANDROID_SO_OUTDIR})
    file(COPY ${ANDROID_SYSROOT_LIB}/libdl.so DESTINATION ${ANDROID_SO_OUTDIR})
    file(COPY ${ANDROID_SYSROOT_LIB}/libc.so DESTINATION ${ANDROID_SO_OUTDIR})
    file(COPY ${ANDROID_SYSROOT_LIB}/libandroid.so DESTINATION ${ANDROID_SO_OUTDIR})
    file(COPY ${ANDROID_SYSROOT_LIB}/libGLESv2.so DESTINATION ${ANDROID_SO_OUTDIR})
    file(COPY ${ANDROID_SYSROOT_LIB}/libEGL.so DESTINATION ${ANDROID_SO_OUTDIR})

    # override AndroidManifest.xml 
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/android/AndroidManifest.xml
        "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n"
        "  package=\"com.oryol.${target}\"\n"
        "  android:versionCode=\"1\"\n"
        "  android:versionName=\"1.0\">\n"
        "  <uses-sdk android:minSdkVersion=\"11\" android:targetSdkVersion=\"19\"/>\n"
        "  <uses-feature android:glEsVersion=\"0x00020000\"></uses-feature>"
        "  <application android:label=\"${target}\" android:hasCode=\"false\">\n"
        "    <activity android:name=\"android.app.NativeActivity\"\n"
        "      android:label=\"${target}\"\n"
        "      android:configChanges=\"orientation|keyboardHidden\">\n"
        "      <meta-data android:name=\"android.app.lib_name\" android:value=\"${target}\"/>\n"
        "      <intent-filter>\n"
        "        <action android:name=\"android.intent.action.MAIN\"/>\n"
        "        <category android:name=\"android.intent.category.LAUNCHER\"/>\n"
        "      </intent-filter>\n"
        "    </activity>\n"
        "  </application>\n"
        "</manifest>\n")

endmacro()

#-------------------------------------------------------------------------------
#   oryol_android_postbuildstep
#   Setup a post-build-step which creates the actual APK file.
#
macro(oryol_android_postbuildstep target)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(ANT_BUILD_TYPE "debug")
    else()
        set(ANT_BUILD_TYPE "release")
    endif()
    add_custom_command(TARGET ${target} POST_BUILD COMMAND ${ANDROID_ANT} ${ANT_BUILD_TYPE} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/android)
endmacro()