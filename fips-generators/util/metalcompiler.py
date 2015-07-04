'''
Python wrapper for metal shader compiler.
'''
import subprocess
import tempfile
import platform
import os
import sys
import genutil as util

# FIXME: different platform-root for OSX and iOS!
platform_root = '/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/' 
sys_root = platform_root + 'Developer/SDKs/MacOSX10.11.sdk'
bin_path = platform_root + 'usr/bin/'
metal_path = bin_path + 'metal'
metal_ar_path = bin_path + 'metal-ar'
metal_ln_path = bin_path + 'metallib'

#-------------------------------------------------------------------------------
def writeFile(f, lines) :
    '''
    Write an array of lines to a file.
    '''
    for line in lines :
        f.write(line.content + '\n')

#-------------------------------------------------------------------------------
def run(cmd) :
    # run a generic command an capture stdout
    child = subprocess.Popen(cmd, stderr=subprocess.PIPE)
    out = ''
    while True :
        out += child.stderr.read()
        if child.poll() != None :
            break
    return out

#-------------------------------------------------------------------------------
def cc(in_src, out_dia, out_air) :
    # run the metal compiler
    cmd = [metal_path, '-arch', 'air64', '-emit-llvm', '-ffast-math', '-c', 
           '-isysroot', sys_root,
           '-serialize-diagnostics', out_dia,
           '-o', out_air,
           '-mmacosx-version-min=10.11',
           '-std=osx-metal1.1',
           in_src]
    return run(cmd)

#-------------------------------------------------------------------------------
def ar(in_air, out_lib) :
    # run the metal librarian
    cmd = [metal_ar_path, 'r', out_lib, in_air]
    return run(cmd)

#-------------------------------------------------------------------------------
def link(in_lib, out_bin) :
    # run the metal linker
    cmd = [metal_ln_path, '-o', out_bin, in_lib]
    return run(cmd)

#-------------------------------------------------------------------------------
def parseOutput(output, lines) :
    hasError = False
    hasWarnings = False
    outLines = output.splitlines()

    for outLine in outLines :
        if 'error:' in outLine or 'warning:' in outLine or 'note:' in outLine:
            tokens = outLine.split(':')
            metalSrcPath = tokens[0]
            lineNr = int(tokens[1])
            colNr = int(tokens[2])
            msgType = tokens[3]
            msg = tokens[4]

            if msgType == ' error':
                hasError = True
            if msgType == ' warning':
                hasWarning = True

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
        for outLine in outLines :
            print(outLine)
        for line in lines :
            print(line.content)
        sys.exit(10) 

#-------------------------------------------------------------------------------
def validate(lines, outPath) :
    
    # test if tools exists
    if not os.path.isfile(metal_path) :
        util.fmtWarning('metal compiler not found (expected path: {})\n'.format(metal_path))
        return
    if not os.path.isfile(metal_ar_path) :
        util.fmtWarning('metal librarian not found (expected path: {})\n'.format(metal_ar_path))
        return
    if not os.path.isfile(metal_ln_path) :
        util.fmtWarning('metal linker not found (expected path: {})\n'.format(metal_ln_path))
        return

    # filenames
    rootPath = os.path.splitext(outPath)[0]
    metal_src_path = rootPath + '.metal'
    metal_dia_path = rootPath + '.dia'
    metal_air_path = rootPath + '.air'
    metal_lib_path = rootPath + '.metal-ar'
    metal_bin_path = rootPath + '.metallib'

    # write metal source file
    with open(metal_src_path, 'w') as f :
        writeFile(f, lines)

    # compile .metal source file
    output = cc(metal_src_path, metal_dia_path, metal_air_path)
    output += ar(metal_air_path, metal_lib_path)
    output += link(metal_lib_path, metal_bin_path)

    # parse errors and warnings
    parseOutput(output, lines)
    



