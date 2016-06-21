'''
Simple python wrapper for the GLSL reference compiler, also used
to generate SPIRV bytecode for Vulkan.
'''

import subprocess
import tempfile
import platform
import os
import sys
import binascii
import genutil as util

def getToolsBinPath() :
    path = os.path.dirname(os.path.abspath(__file__))
    if platform.system() == 'Windows' :
        path += '/../../tools/win32/'
    elif platform.system() == 'Darwin' :
        path += '/../../tools/osx/'
    elif platform.system() == 'Linux' :
        if os.uname()[1] == 'raspberrypi' :
            path += '/../../tools/raspi/'
        else :
            path +=  '/../../tools/linux/'
    else :
        error("Unknown host system {}".format(platform.system()))
    return path;

def writeFile(f, lines) :
    '''
    Write an array of lines to a file.
    '''
    for line in lines :
        f.write(str.encode(line.content + '\n'))

def callValidator(cmd) :
    ''' 
    call the GLSL reference compiler and return its output
    '''
    child = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    out = ''
    while True :
        out += bytes.decode(child.stdout.read())
        if child.poll() != None :
            break
    return out

def parseOutput(output, lines) :
    '''
    Parse error output lines from the GLSL reference compiler,
    map them to the original source code location and output
    an error message compatible with Xcode or VStudio
    '''
    hasError = False
    outLines = output.splitlines()
    for outLine in outLines :
        if outLine.startswith('ERROR: ') :
            hasError = True

            # extract generated shader source column, line and message
            colStartIndex = 7
            colEndIndex = outLine.find(':', colStartIndex)
            if colEndIndex == -1 :
                continue
            lineStartIndex = colEndIndex + 1
            lineEndIndex = outLine.find(':', lineStartIndex)
            if lineEndIndex == -1 :
                continue
            msgStartIndex = lineEndIndex + 1
            colNr = int(outLine[colStartIndex:colEndIndex])
            lineNr = int(outLine[lineStartIndex:lineEndIndex])
            msg = outLine[msgStartIndex:]

            # map to original location
            lineIndex = lineNr - 1
            if lineIndex >= len(lines) :
                lineIndex = len(lines) - 1
            srcPath = lines[lineIndex].path
            srcLineNr = lines[lineIndex].lineNumber
            
            # and output...
            util.setErrorLocation(srcPath, srcLineNr)
            util.fmtError(msg, False)
            
    if hasError :
        for line in lines :
            print(line.content)
        sys.exit(10) 

def writeBinHeader(in_bin, out_hdr, c_name) :
    '''
    Write the generated SPIRV bytecode file to C header
    '''
    with open(in_bin, 'rb') as in_file :
        data = in_file.read()
    hexdata = binascii.hexlify(data)
    with open(out_hdr, 'w') as out_file :
        out_file.write('#pragma once\n')
        out_file.write('static const unsigned char {}[] = {{\n'.format(c_name))
        for i in range(0, len(data)) :
            out_file.write('0x{}{},'.format(hexdata[i*2], hexdata[i*2+1]))
            if (i % 16) == 15 :
                out_file.write('\n')
        out_file.write('\n};\n')

def validate(lines, type, glslVersion) :
    '''
    Validate a vertex-/fragment-shader pair for a given glsl version.
    '''
    ext = {
        'vs': '.vert',
        'fs': '.frag'
    }
    f = tempfile.NamedTemporaryFile(suffix=ext[type], delete=False)
    writeFile(f, lines)
    f.close()

    toolPath = getToolsBinPath() + 'glslangValidator'
    cmd = [toolPath, f.name]
    output = callValidator(cmd)
    os.unlink(f.name)
    parseOutput(output, lines)

def validateSPIRV(lines, type, glslVersion, outPath, cName) :
    '''
    Validate shader for SPIRV and generate C header with byte code. 
    '''
    ext = {
        'vs': '.vert',
        'fs': '.frag'
    }

    rootPath = os.path.splitext(outPath)[0]
    glsl_src_path = rootPath + '{}'.format(ext[type])
    spirv_out_path = rootPath + '{}.spirv'.format(ext[type])
    with open(glsl_src_path, 'w') as f :
        writeFile(f, lines)

    toolPath = getToolsBinPath() + 'glslangValidator'
    cmd = [toolPath, f.name, '-V', '-o', spirv_out_path]
    output = callValidator(cmd)
    parseOutput(output, lines)
    writeBinHeader(spirv_out_path, outPath, cName)


