#-------------------------------------------------------------------------------
#   oryol_unittests.cmake
#
#   Macros for generating unit tests.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#   oryol_begin_unittest(name)
#   Begin defining a unit test.
#
macro(oryol_begin_unittest name)
    oryol_reset(${name})
    set(CurAppName ${name}_test)
    set(CurAppType "cmdline")
    message("Oryol Unit Test: name=" ${CurAppName})
endmacro()

#-------------------------------------------------------------------------------
#   oryol_end_unittest()
#   End defining a unittest named 'name' from sources in 'dir'
#
macro(oryol_end_unittest)
    
    # add unittestpp lib dependency
    oryol_deps(unittestpp)

    # generate a scratch main-source-file
    set(main_path ${CMAKE_CURRENT_BINARY_DIR}/${CurAppName}_main.cc)
    file(WRITE ${main_path}
        "// machine generated, do not edit\n"
        "#include \"pre.h\"\n"
        "#include \"core/core.h\"\n"
        "#include \"UnitTest++/src/unittest++.h\"\n"
        "int main(void) {\n"
        "    oryol::core::setup();\n"
        "    int res = UnitTest::RunAllTests();\n"
        "    oryol::core::discard();\n"
        "    return res;\n"
        "}\n"
    )

    # generate a command line app
    list(APPEND CurSources ${main_path})
    oryol_end_app()
    set_target_properties(${CurAppName} PROPERTIES FOLDER "unittests")

    # add as cmake unit test
    add_test(NAME ${CurAppName} COMMAND ${CurAppName})

    # if configured, start the app as post-build-step
    if (ORYOL_UNITTESTS_RUN_AFTER_BUILD)
        add_custom_command (TARGET ${CurAppName} POST_BUILD COMMAND ${CurAppName})
    endif()
endmacro()
