#-------------------------------------------------------------------------------
#   oryol_pnacl.cmake
#   PNaCl specific cmake functions.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#   Setup post-build step to run the finalizer.
#
macro(oryol_pnacl_post_buildsteps target)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(PNACL_POSTFIX "_debug")
    else()
        set(PNACL_POSTFIX "")
    endif()
    add_custom_command(TARGET ${target} POST_BUILD COMMAND ${NACL_TOOLCHAIN_ROOT}/bin/pnacl-finalize --compress $<TARGET_FILE:${target}>)
    add_custom_command(TARGET ${target} POST_BUILD COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/${target}${PNACL_POSTFIX}.nmf $<TARGET_FILE_DIR:${target}>)
    add_custom_command(TARGET ${target} POST_BUILD COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/${target}${PNACL_POSTFIX}_pnacl.html $<TARGET_FILE_DIR:${target}>)
endmacro()

#-------------------------------------------------------------------------------
#   Create wrapper files for NaCl projects (a shell HTML, and the manifest)
macro(oryol_pnacl_create_wrapper target)
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(PNACL_POSTFIX "_debug")
    else()
        set(PNACL_POSTFIX "")
    endif()
    
    # create the manifest file
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${target}${PNACL_POSTFIX}.nmf
        "{\n"
        "  \"program\": {\n"
        "    \"portable\": {\n"
        "      \"pnacl-translate\": {\n"
        "        \"url\": \"${target}${PNACL_POSTFIX}.pexe\"\n"
        "      }\n"
        "    }\n"
        "  }\n"
        "}\n")

    # create the HTML wrapper file
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${target}${PNACL_POSTFIX}_pnacl.html
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n"
        "<html>\n"
        "<head>\n"
        "  <meta charset=\"utf-8\"></meta>\n"
        "  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></meta>\n"
        "  <meta http-equiv=\"Pragma\" content=\"no-cache\">\n"
        "  <meta http-equiv=\"Expires\" content=\"-1\">\n"        
        "  <title>Oryol/PNaCl</title>\n"
        "</head>\n"
        "<body>\n"
        "<div class=\"page\">\n"
        "  <div class=\"pnacl_status\">\n"
        "    <code id=\"statusField\">NO-STATUS</code>\n"
        "  </div>\n"
        "  <div class=\"pnacl_border\">\n"
        "    <embed src=\"${target}${PNACL_POSTFIX}.nmf\" class=\"pnacl\" id=\"pnacl_module\" name=\"pnacl_module\" width=\"800\" height=\"452\" type=\"application/x-pnacl\"/>\n"
        "  </div>\n"
        "  <textarea class=\"pnacl\" id=\"output\" rows=\"20\" cols=\"80\"></textarea>\n"
        "  <script type=\"text/javascript\">\n"
        "    var progressCount = 0;\n"
        "    function updateStatus(text) {\n"
        "      document.getElementById('statusField').innerHTML = text;\n"
        "    }\n"
        "    function handleLoadStart() {\n"
        "      updateStatus('LOAD START...');\n"
        "    }\n"
        "    function handleProgress(event) {\n"
        "      var loadPercent = 0.0;\n"
        "      var loadPercentString;\n"
        "      if (event.lengthComputable && event.total > 0) {\n"
        "        loadPercent = (event.loaded / event.total) * 100.0;\n"
        "        loadPercentString = 'Downloading & Preparing (' + loadPercent.toFixed(2) + '%)';\n"
        "      } else {\n"
        "        loadPercent = -1.0;\n"
        "        progressCount++;\n"
        "        loadPercentString = 'Downloading... (count=' + progressCount + ')';\n"
        "      }\n"
        "      updateStatus(loadPercentString);\n"
        "    }\n"
        "    function handleLoad() {\n"
        "      updateStatus('RUNNING...');\n"
        "    }\n"
        "    function handleError() {\n"
        "      updateStatus('ERROR: ' + document.getElementById('pnacl_module').lastError);\n"
        "    }\n"
        "    function handleCrash() {\n"
        "      updateStatus('CRASHED!!!');\n"
        "    }\n"
        "    function handleMessage(msg) {\n"
        "      var element = document.getElementById('output');\n"
        "      element.value += msg.data;\n"
        "      element.scrollTop = 99999;\n"
        "    }\n"
        "    var naclModule = document.getElementById('pnacl_module');\n"
        "    naclModule.addEventListener('loadstart', handleLoadStart, true);\n"
        "    naclModule.addEventListener('progress', handleProgress, true);\n" 
        "    naclModule.addEventListener('load', handleLoad, true);\n"
        "    naclModule.addEventListener('error', handleError, true);\n"
        "    naclModule.addEventListener('crash', handleCrash, true);\n"
        "    naclModule.addEventListener('message', handleMessage, true);\n"
        "    updateStatus('LOADING...');\n"
        "  </script>\n"
        "</div>\n"
        "</body>\n"
        "</html>\n")

endmacro()


