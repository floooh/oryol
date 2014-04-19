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

    # override AndroidManifest.xml 
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/android/AndroidManifest.xml
        "<manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"\n"
        "  package=\"com.oryol.${target}\"\n"
        "  android:versionCode=\"1\"\n"
        "  android:versionName=\"1.0\">\n"
        "  <uses-sdk android:minSdkVersion=\"9\"/>\n"
        "  <application android:label=\"@string/app_name\" android:hasCode=\"false\">\n"
        "    <activity android:name=\"android.app.NativeActivity\"\n"
        "      android:label=\"@string/app_name\"\n"
        "      android:configChanges=\"orientation|keyboardHidden\">\n"
        "      <meta-data android:name=\"android.app.${target}\"\n"
        "        android:value=\"native-activity\"/>\n"
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