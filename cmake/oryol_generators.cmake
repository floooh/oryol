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
macro(oryol_handle_xml_files_pretarget xmlFiles)
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
macro(oryol_handle_xml_files_posttarget target xmlFiles)
    if (PYTHON)
        # ...and add a custom target to build the sources
        add_custom_target(${target}_xml 
                          COMMAND ${PYTHON} ${ORYOL_ROOT_DIR}/generators/xmlgen.py ${xmlFiles} 
                          WORKING_DIRECTORY ${ORYOL_ROOT_DIR}/generators
                          COMMENT "Generating sources...")
        set_target_properties(${target}_xml PROPERTIES FOLDER "Generators")
        add_dependencies(${target} ${target}_xml)
    else()  
        message("WARNING: Python not found, skipping XML generators!")
    endif()
endmacro()

#-------------------------------------------------------------------------------
#   Adds a .cc and .h file for each .py generator file.
#
macro(oryol_handle_py_files_pretarget pyFiles)
    foreach(pyFile ${pyFiles})
        string(REPLACE .py .cc src ${pyFile})
        string(REPLACE .py .h hdr ${pyFile})
        if (NOT EXISTS ${src})
            file(WRITE ${src} " ")
        endif()
        if (NOT EXISTS ${hdr})
            file(WRITE ${hdr} " ")
        endif()
    endforeach()    
endmacro()

#-------------------------------------------------------------------------------
#   Create custom target for .py generator files.
#
macro(oryol_handle_py_files_posttarget target pyFiles)
    if (PYTHON)
        # ...and add a custom target to build the sources
        add_custom_target(${target}_py 
                          COMMAND ${PYTHON} ${ORYOL_ROOT_DIR}/generators/pygen.py ${pyFiles} 
                          WORKING_DIRECTORY ${ORYOL_ROOT_DIR}/generators
                          COMMENT "Generating sources...")
        set_target_properties(${target}_py PROPERTIES FOLDER "Generators")
        add_dependencies(${target} ${target}_py)
    else()  
        message("WARNING: Python not found, skipping python generators!")
    endif()
endmacro()
