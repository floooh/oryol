#-------------------------------------------------------------------------------
#   oryol_generators.cmake
#   Helper cmake functions for integrating python generator scripts into 
#   the build process.
#-------------------------------------------------------------------------------

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
