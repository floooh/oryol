#-------------------------------------------------------------------------------
#	oryol_private.cmake
#	Private cmake macros (all the public stuff is in oryol.cmake)
#	(C) 2013 A.Weissflog
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#	oryol_reset(target)
#	Reset the global tracker variables.
#
macro(oryol_reset target)
	set(CurSources)
	set(CurNaclFiles)
	set(CurDependencies)
	set(CurLinkLibs)
	set(CurFrameworks)
	set(CurModuleName)
	set(CurAppName)
	set(CurAppType)
	set(${target}_deps)
	set(${target}_libs)
	set(${target}_frameworks)
endmacro()

#-------------------------------------------------------------------------------
#	oryol_apply_target_group(target)
#	Apply IDE group name to target.
#
macro(oryol_apply_target_group target)
	if (NOT ${ORYOL_TARGET_GROUP} STREQUAL "")
    	set_target_properties(${target} PROPERTIES FOLDER ${ORYOL_TARGET_GROUP})
    endif()
endmacro()

#-------------------------------------------------------------------------------
#	oryol_setup_link_directories()
#	Setup the link library search paths.
#
macro(oryol_setup_link_directories)
	include_directories(${ORYOL_ROOT_DIR}/lib/${ORYOL_PLATFORM_NAME})
endmacro()

#-------------------------------------------------------------------------------
#	oryol_recurse_deps(input output)
#
macro(oryol_recurse_deps input output)
	foreach(dep ${input})
		list(APPEND output ${dep})
		oryol_recurse_deps(${dep}_deps output)
	endforeach()
endmacro()

#-------------------------------------------------------------------------------
#	oryol_resolve_dependencies(target)
#	Recursively resolve dependencies of a target.
#
macro(oryol_resolve_dependencies target)
	set(deps)
    if (${target}_deps)
	   oryol_recurse_deps(${target}_deps deps)
    endif()
    if (deps)
	   list(REMOVE_DUPLICATES deps)
    endif()
	message("${target} Dependencies: ${deps}")
	target_link_libraries(${target} ${deps})
endmacro()

#-------------------------------------------------------------------------------
#	oryol_recurse_libs(input output)
#
macro(oryol_recurse_libs input output)
	foreach(lib ${input})
		list(APPEND output ${lib})
		oryol_recurse_deps(${lib}_libs output)
	endforeach()
endmacro()

#-------------------------------------------------------------------------------
#	oryol_resolve_dependencies(target)
#	Recursively resolve dependencies of a target.
#
macro(oryol_resolve_linklibs target)
	set(libs)
    if (${target}_libs)
	   oryol_recurse_libs(${target}_libs libs)
    endif()
    if (libs)
	   list(REMOVE_DUPLICATES libs)
    endif()
	message("${target} Libs: ${libs}")
	target_link_libraries(${target} ${libs})
endmacro()

#-------------------------------------------------------------------------------
#	oryol_recurse_frameworks(input output)
#
macro(oryol_recurse_frameworks input output)
	foreach(fw ${input})
		list(APPEND output ${fw})
		oryol_recurse_frameworks(${fw}_frameworks output)
	endforeach()
endmacro()

#-------------------------------------------------------------------------------
#	oryol_resolve_frameworks(target)
#	Recursively resolve dependencies of a target.
#
macro(oryol_resolve_frameworks target)
	set(frameworks)
    if (${target}_frameworks)
	   oryol_recurse_frameworks(${target}_frameworks frameworks)
    endif()
    if (frameworks)
	   list(REMOVE_DUPLICATES frameworks)
    endif()
	message("${target} frameworks: ${frameworks}")
	foreach (fw ${frameworks})
		FIND_LIBRARY(found_framework ${fw})
		target_link_libraries(${target} ${found_framework})
	endforeach()
endmacro()

#-------------------------------------------------------------------------------
#	oryol_copy_osx_dylib_files(target isbundle)
#	Copy OSX dynamic link libs to the executables directory.
#
macro(oryol_copy_osx_dylib_files target isbundle)
    if (ORYOL_IOS OR ORYOL_OSX)
        if (${isbundle})
            set(XCODE_OUTPUT_DIR \${TARGET_BUILD_DIR}/\${FULL_PRODUCT_NAME}/Contents/MacOS)
        else()
            set(XCODE_OUTPUT_DIR \${TARGET_BUILD_DIR})
        endif()
        foreach (dylib ${CurDylibFiles})
            set(fullDylibName lib${dylib}.dylib)
            message("Add post build step to copy dylib: ${ORYOL_ROOT_DIR}/lib/osx/${fullDylibName}")
            add_custom_command(TARGET ${target} POST_BUILD COMMAND pwd\; cp ${ORYOL_ROOT_DIR}/lib/osx/${fullDylibName} ${XCODE_OUTPUT_DIR})
        endforeach()
    endif()
endmacro()

#-------------------------------------------------------------------------------
#	oryol_osx_add_target_properties(target)
#   Setup setup special target properties for OSX/iOS.
#
macro(oryol_osx_add_target_properties target)
	if (ORYOL_IOS OR ORYOL_OSX)
		if (ORYOL_IOS)
	        set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")    
	        set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "'1,2'")
	    endif()
	    if (ORYOL_IOS)
	        file(GLOB plist *.ios.plist)
	    else()
	        file(GLOB plist *.osx.plist)
	    endif()
	    if (NOT ${plist} STREQUAL "")
	        message("iOS/OSX plist file:" ${plist} " GUI identifier: " ${ORYOL_OSX_GUI_IDENTIFIER})
	        set_target_properties(${target} PROPERTIES MACOSX_BUNDLE_EXECUTABLE_NAME \${EXECUTABLE_NAME})
	        set_target_properties(${target} PROPERTIES MACOSX_BUNDLE_PRODUCT_NAME \${PRODUCT_NAME})
	        set_target_properties(${target} PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER ${ORYOL_OSX_GUI_IDENTIFIER})
	        set_target_properties(${target} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME \${PRODUCT_NAME})
	        set_target_properties(${target} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${plist})
	        set_target_properties(${target} PROPERTIES MACOSX_BUNDLE_ICON_FILE "Icon.png")
	    endif()
	endif()
endmacro()

#-------------------------------------------------------------------------------
#   Internal macro to set the configuration postfixes
#
macro(oryol_config_postfixes_for_exe target)

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
endmacro(oryol_config_postfixes_for_exe)

#-------------------------------------------------------------------------------
#   Internal macro to set the output directory
#
function(oryol_exe_output_directory target)
    if (NOT ORYOL_IOS)
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
#   NACL: setup post-build step to run the finalizer.
#
macro(oryol_pnacl_post_buildsteps target)
    if (ORYOL_PNACL)
        add_custom_command(TARGET ${target} POST_BUILD COMMAND ${NACL_TOOLCHAIN_ROOT}/bin/pnacl-finalize $<TARGET_FILE:${target}>)
        foreach(curFile ${CurNaclFiles})
            message("Copy NaCl file: ${curFile}")
            add_custom_command(TARGET ${target} POST_BUILD COMMAND cp ${curFile} $<TARGET_FILE_DIR:${target}>)
        endforeach()
    endif()
endmacro()
