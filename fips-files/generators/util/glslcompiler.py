'''
Simple python wrapper for the GLSL reference compiler.
'''

import subprocess, platform, os, sys
import genutil as util

#-------------------------------------------------------------------------------
class Line :
    def __init__(self, content, path='', lineNumber=0) :
        self.content = content
        self.path = path
        self.lineNumber = lineNumber

#-------------------------------------------------------------------------------
def getToolPath() :
    path = os.path.dirname(os.path.abspath(__file__))
    if platform.system() == 'Windows' :
        path += '/../../../tools/win32/'
    elif platform.system() == 'Darwin' :
        path += '/../../../tools/osx/'
    elif platform.system() == 'Linux' :
        if os.uname()[1] == 'raspberrypi' :
            path += '/../../../tools/raspi/'
        else :
            path +=  '/../../../tools/linux/'
    else :
        error("Unknown host system {}".format(platform.system()))
    return path + 'glslangValidator'

#-------------------------------------------------------------------------------
def writeFile(f, lines) :
    for line in lines :
        f.write(line.content + '\n')

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
            lineStartIndex = outLine.find(':', 9) + 1
            if lineStartIndex == 0:
                continue
            lineEndIndex = outLine.find(':', lineStartIndex)
            if lineEndIndex == -1 :
                continue
            msgStartIndex = lineEndIndex + 1
            try:
                lineNr = int(outLine[lineStartIndex:lineEndIndex])
            except ValueError:
                continue
            msg = outLine[msgStartIndex:]

            # map to original location 
            lineIndex = lineNr - 1
            if lineIndex >= len(lines) :
                lineIndex = len(lines) - 1
            srcPath = lines[lineIndex].path
            srcLineNr = lines[lineIndex].lineNumber
            util.setErrorLocation(srcPath, srcLineNr)
            util.fmtError(msg, False)
            
    if hasError :
        for line in lines :
            print(line.content)
        sys.exit(10) 

#-------------------------------------------------------------------------------
def compile(lines, type, base_path, slang, args) :
    # compile GLSL source file to SPIR-V
    ext = {
        'vs': 'vert',
        'fs': 'frag'
    }
    # GLSL can have multiple versions, force to generic 'glsl'
    if 'glsl' in slang:
        slang = 'glsl'
    src_path = '{}.{}.{}'.format(base_path, slang, ext[type])
    dst_path = '{}.{}.spv'.format(base_path, slang)
    tgt_lines = []
    tgt_lines.append(Line('#version 330'))
    tgt_lines.append(Line('#define ORYOL_GLSL ({})'.format('1' if slang=='glsl' else '0')))
    tgt_lines.append(Line('#define ORYOL_MSL ({})'.format('1' if slang=='metal' else '0')))
    tgt_lines.append(Line('#define ORYOL_HLSL ({})'.format('1' if slang=='hlsl' else '0')))
    tgt_lines.extend(lines)
    with open(src_path, 'w') as f:
        writeFile(f, tgt_lines)
    cmd = [getToolPath(), '-G', '-o', dst_path, src_path]
    output = call(cmd)
    parseOutput(output, tgt_lines)

#-------------------------------------------------------------------------------
'''
FIXME: this isn't currently called
def validate(sl_version, type, base_path, args) :
    # run validation over a generated GLSL source
    src_path = "{}.{}".format(base_path, sl_version)
    # since we can't map errors back to the original source file across
    # SPIRV, show errors in the generated source instead
    lines = []
    with open(src_path, 'r') as f:
        src_lines = f.readlines()
        line_nr = 0
        for line in src_lines:
            lines.append(Line(line, src_path, line_nr))
            line_nr += 1
    stage = 'vert' if type == 'vs' else 'frag'
    cmd = [getToolPath(), '-S', stage, src_path]
    output = call(cmd)
    parseOutput(output, lines)
'''


