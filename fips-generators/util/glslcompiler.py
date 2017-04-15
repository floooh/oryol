'''
Simple python wrapper for the GLSL reference compiler.
'''

import subprocess, platform, os, sys
import genutil as util

#-------------------------------------------------------------------------------
def getToolPath() :
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
    return path + 'glslangValidator'

#-------------------------------------------------------------------------------
def writeFile(f, lines) :
    for line in lines :
        f.write(str.encode(line.content + '\n'))

#-------------------------------------------------------------------------------
def call(cmd) :
    child = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    out = ''
    while True :
        out += bytes.decode(child.stdout.read())
        if child.poll() != None :
            break
    return out

#-------------------------------------------------------------------------------
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
            lineStartIndex = outLine.find(':', 6) + 1
            if lineStartIndex == 0:
                continue
            lineEndIndex = outLine.find(':', lineStartIndex)
            if lineEndIndex == -1 :
                continue
            msgStartIndex = lineEndIndex + 1
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

#-------------------------------------------------------------------------------
def compile(lines, type, base_path, args) :
    # compile GLSL source file to SPIR-V
    ext = {
        'vs': '.vert',
        'fs': '.frag'
    }
    src_path = base_path + ext[type]
    dst_path = base_path + '.spv'
    with open(src_path, 'w') as f:
        writeFile(f, lines)
    cmd = [getToolPath(), '-G', '-o', dst_path, src_path]
    output = call(cmd)
    parseOutput(output, lines)


