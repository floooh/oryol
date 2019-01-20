#-------------------------------------------------------------------------------
#   oryol core module
#-------------------------------------------------------------------------------
fips_begin_module(Core)
    fips_vs_warning_level(3)
    if (FIPS_MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    endif()
    fips_files(
        Main.h
        App.cc App.h
        AppState.h
        Args.cc Args.h
        Assertion.h
        Class.h
        Config.h
        Core.cc Core.h
        Creator.h
        Log.cc Log.h
        Logger.cc Logger.h
        Ptr.h
        RefCounted.h
        RunLoop.cc RunLoop.h
        Types.h
        StackTrace.cc StackTrace.h
        precompiled.h
    )
    fips_dir(Containers)
    fips_files(
        Array.h
        ArrayMap.h
        Slice.h
        Buffer.h
        HashSet.h
        KeyValuePair.h
        Map.h
        Queue.h
        Set.h
        StaticArray.h
        elementBuffer.h
        InlineArray.h
    )
    fips_dir(Memory)
    fips_files(Memory.cc Memory.h)
    fips_dir(String)
    fips_files(
        String.cc String.h
        StringAtom.cc StringAtom.h
        StringBuilder.cc StringBuilder.h
        StringConverter.cc StringConverter.h
        WideString.cc WideString.h
        stringAtomBuffer.cc stringAtomBuffer.h
        stringAtomTable.cc stringAtomTable.h
        ConvertUTF.c ConvertUTF.h
    )
    fips_dir(Threading)
    fips_files(
        ThreadLocalData.cc ThreadLocalData.h
        ThreadLocalPtr.h
    )
    fips_dir(Time)
    fips_files(
        Clock.cc Clock.h Duration.h TimePoint.h
    )
    if (FIPS_POSIX)
        fips_dir(private/posix)
        fips_files(precompiled.h)
    endif()
    if (FIPS_LINUX)
        fips_libs(dl)
    endif()
    if (FIPS_WINDOWS)
        fips_dir(private/windows)
        fips_files(StackWalker.cc StackWalker.h)
        fips_files(precompiled.h)
    endif()
    if (FIPS_ANDROID)
        fips_dir(private/android)
        fips_files(android_native_app_glue.c android_native_app_glue.h)
        fips_files(androidBridge.cc androidBridge.h)
        fips_libs(android log)
    endif()
    if (FIPS_IOS)
        fips_dir(private/ios)
        fips_files(
            iosBridge.mm iosBridge.h
        )
        if (ORYOL_METAL)
            fips_frameworks_osx(Metal MetalKit)
        endif()
        if (ORYOL_OPENGL)
            fips_frameworks_osx(OpenGLES GLKit)
        endif()
    endif()
    if (FIPS_MACOS AND ORYOL_METAL)
        fips_dir(private/osx)
        fips_files(osxBridge.h osxBridge.mm osxInputDefs.h)
        fips_frameworks_osx(Cocoa Metal MetalKit QuartzCore)
    endif()
    fips_dir(.)
    fips_files(Trace.h Trace.cc)
    if (FIPS_PROFILING AND (FIPS_LINUX OR FIPS_MACOS OR FIPS_WINDOWS))
        fips_deps(Remotery)
    endif()
    if (FIPS_USE_VLD)
        fips_libs(vld)
    endif()
fips_end_module()
if (FIPS_USE_VLD)
    add_dependencies(Core vld_copy_dlls)
endif()

oryol_begin_unittest(Core)
    fips_vs_warning_level(3)
    fips_dir(UnitTests)
    fips_files(
        PtrTest.cc
        SliceTest.cc
        InlineArrayTest.cc
        StackTraceTest.cc
        BufferTest.cc
        ArgsTest.cc
        ArrayTest.cc
        StaticArrayTest.cc
        ArrayMapTest.cc
        CreationTest.cc
        CreatorTest.cc
        HashSetTest.cc
        MapTest.cc
        MemoryTest.cc
        QueueTest.cc
        RttiTest.cc
        RunLoopTest.cc
        SetTest.cc
        StringAtomTest.cc
        StringBuilderTest.cc
        StringConverterTest.cc
        StringTest.cc
        WideStringTest.cc
        elementBufferTest.cc
        ClockTest.cc
        DurationTest.cc
        TimePointTest.cc
        LogTest.cc
    )
    fips_deps(Core)
oryol_end_unittest()

