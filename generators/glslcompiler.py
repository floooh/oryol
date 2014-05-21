'''
Simple python wrapper for the GLSL reference compiler.
'''

import subprocess
import tempfile
import platform
import os
import util

def getToolsBinPath() :
    path = os.path.dirname(os.path.abspath(__file__))
    if platform.system() == 'Windows' :
        path += '/../tools/win32/'
    elif platform.system() == 'Darwin' :
        path += '/../tools/osx/'
    elif platform.system() == 'Linux' :
        path +=  '/../tools/linux/'
    else :
        error("Unknown host system {}".format(platform.system()))
    return path;

def writeFile(f, lines) :
    '''
    Write an array of lines to a file.
    '''
    for line in lines :
        f.write(line.content + '\n')

def callValidator(cmd) :
    ''' 
    call the GLSL reference compiler and return its output
    '''
    child = subprocess.Popen(cmd, stdout=subprocess.PIPE)
    out = ''
    while True :
        out += child.stdout.read()
        if child.poll() != None :
            break
    return out

def parseOutput(output, lines) :
    '''
    Parse error output lines from the GLSL reference compiler,
    map them to the original source code location and output
    an error message compatible with Xcode or VStudio
    '''
    outLines = output.splitlines()
    for outLine in outLines :
        if outLine.startswith('ERROR: ') :
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
            util.fmtError(msg)

def validate(lines, type, glslVersion) :
    '''
    Validate a vertex-/fragment-shader pair for a given glsl version.
    '''
    ext = {
        'vs': '.vert',
        'fs': '.frag'
    }
    f = tempfile.NamedTemporaryFile(suffix=ext[type], delete=False)
    print '=> compiling {}'.format(f.name)
    writeFile(f, lines)
    f.close()

    toolPath = getToolsBinPath() + 'glslangValidator'
    cmd = [toolPath, f.name]
    output = callValidator(cmd)
    os.unlink(f.name)
    parseOutput(output, lines)


