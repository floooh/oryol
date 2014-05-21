#-------------------------------------------------------------------------------
#   oryol_generators.cmake
#   Helper cmake functions for integrating python generator scripts into 
#   the build process.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#   Handle XML generator files before the dependent target exists. This
#   checks if the target source/header files already exist, if not
#   dummy files will be generated (the generator script will not be run).
#
macro(oryol_handle_generator_files_pretarget xmlFiles)
    foreach(xmlFile ${xmlFiles})
        string(REPLACE .xml .cc src ${xmlFile})
        string(REPLACE .xml .h hdr ${xmlFile})
        if (NOT EXISTS ${src})
            file(WRITE ${src} " ")
        endif()
        if (NOT EXISTS ${hdr})
            file(WRITE ${hdr} " ")
        endif()
    endforeach()    
endmacro()

#-------------------------------------------------------------------------------
#   Handle XML generator files after the target has been created. This
#   adds a custom target to update the generated C++ sources when the
#   XML file changes and adds the custom target as dependency to the target
#
macro(oryol_handle_generator_files_posttarget target xmlFiles)
    if (PYTHON)
        # ...and add a custom target to build the sources
        add_custom_target(${target}_gen COMMAND ${PYTHON} ${ORYOL_ROOT_DIR}/generators/generator.py ${xmlFiles} COMMENT "Generating sources...")
        set_target_properties(${target}_gen PROPERTIES FOLDER "Generators")
        add_dependencies(${target} ${target}_gen)
    else()  
        message("WARNING: Python not found, skipping XML generators!")
    endif()
endmacro()

