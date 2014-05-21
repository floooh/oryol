#-------------------------------------------------------------------------------
#   oryol_private.cmake
#   Private cmake macros (all the public stuff is in oryol.cmake)
#   (C) 2013 A.Weissflog
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#   oryol_reset(target)
#   Reset the global tracker variables.
#
macro(oryol_reset target)
    set(CurSources)
    set(CurXmlFiles)
    set(CurDependencies)
    set(CurLinkLibs)
    set(CurFrameworks)
    set(CurModuleName)
    set(CurLibraryName)
    set(CurAppName)
    set(CurAppType)
endmacro()

#-------------------------------------------------------------------------------
#   oryol_apply_target_group(target)
#   Apply IDE group name to target.
#
macro(oryol_apply_target_group target)
    if (NOT ${ORYOL_TARGET_GROUP} STREQUAL "")
        set_target_properties(${target} PROPERTIES FOLDER ${ORYOL_TARGET_GROUP})
    endif()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_setup_link_directories()
#   Setup the link library search paths.
#
macro(oryol_setup_link_directories)
    link_directories(${ORYOL_ROOT_DIR}/lib/${ORYOL_PLATFORM_NAME})
endmacro()

#-------------------------------------------------------------------------------
#   oryol_recurse_deps(input output)
#
macro(oryol_recurse_deps input output)
    list(APPEND ${output} ${input})
    get_property(sub_input GLOBAL PROPERTY ${input}_deps)
    foreach(dep ${sub_input})
        oryol_recurse_deps(${dep} ${output})
    endforeach()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_resolve_dependencies(target)
#   Recursively resolve dependencies of a target.
#
macro(oryol_resolve_dependencies target)
    set(resolvedDeps)
    get_property(input GLOBAL PROPERTY ${target}_deps)
    foreach(dep ${input})
        oryol_recurse_deps(${dep} resolvedDeps)
    endforeach()

# NOTE: we do NOT remove dups, this simplifies the tricky linker order
# requirements on GCC
#    if (resolvedDeps)
#       list(REMOVE_DUPLICATES resolvedDeps)
#    endif()
    message("${target} Dependencies: ${resolvedDeps}")
    target_link_libraries(${target} ${resolvedDeps})
endmacro()

#-------------------------------------------------------------------------------
#   oryol_recurse_libs(input output)
#
macro(oryol_recurse_libs input output)
    list(APPEND ${output} ${input})
    get_property(sub_input GLOBAL PROPERTY ${input}_libs)
    foreach(lib ${sub_input})
        oryol_recurse_libs(${lib} ${output})
    endforeach()
endmacro()

#-------------------------------------------------------------------------------
#   oryol_resolve_dependencies(target)
#   Recursively resolve link-libs of a target.
#
macro(oryol_resolve_linklibs target)
    set(resolvedLibs)
    get_property(input GLOBAL PROPERTY ${target}_libs)
    foreach(lib ${input})
        oryol_recurse_libs(${lib} resolvedLibs)
    endforeach()
    if (resolvedLibs)
       list(REMOVE_DUPLICATES resolvedLibs)
    endif()
    message("${target} Libs: ${resolvedLibs}")
    target_link_libraries(${target} ${resolvedLibs})
endmacro()

#-------------------------------------------------------------------------------
#   Internal macro to set the configuration postfixes
#
macro(oryol_config_postfixes_for_exe target)
    if (NOT ORYOL_ANDROID)
        if (ORYOL_EMSCRIPTEN)
            set_target_properties(${target} PROPERTIES RELEASE_POSTFIX .html)
            set_target_properties(${target} PROPERTIES DEBUG_POSTFIX _debug.html)
        elseif (ORYOL_PNACL)
            set_target_properties(${target} PROPERTIES RELEASE_POSTFIX .pexe)
            set_target_properties(${target} PROPERTIES DEBUG_POSTFIX _debug.pexe)
        else()
            # set_target_properties(${target} PROPERTIES RELEASE_POSTFIX "")
            set_target_properties(${target} PROPERTIES DEBUG_POSTFIX _debug)
        endif()
    endif()
endmacro(oryol_config_postfixes_for_exe)

#-------------------------------------------------------------------------------
#   Internal macro to set the output directory
#
function(oryol_exe_output_directory target)
    if (NOT (ORYOL_IOS OR ORYOL_ANDROID))
        set(dir ${ORYOL_PROJECT_DIR}/bin/${ORYOL_PLATFORM_NAME})

        # exes
        set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${dir})
        set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${dir})
        set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${dir})

        # ...and DLLs
        set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${dir})
        set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${dir})
        set_target_properties(${target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${dir})
    endif()
endfunction()

#-------------------------------------------------------------------------------
#   Clear and start websamples description file.
#
macro(oryol_begin_web_samples)
    file(WRITE ${CMAKE_SOURCE_DIR}/build/websamples.json "{\n  \"samples\": [\n")
endmacro()

#-------------------------------------------------------------------------------
#   Add a sample file to the web samples description file
#
macro(oryol_add_web_sample name desc type)
    file(APPEND ${CMAKE_SOURCE_DIR}/build/websamples.json "    { \"name\": \"${name}\", \"desc\": \"${desc}\", \"type\": \"${type}\" },\n")
endmacro()

#-------------------------------------------------------------------------------
#   Finish the web samples description file
#
macro(oryol_end_web_samples)
    file(APPEND ${CMAKE_SOURCE_DIR}/build/websamples.json "    { \"name\": \"__end__\" }\n  ]\n}\n")
endmacro()
