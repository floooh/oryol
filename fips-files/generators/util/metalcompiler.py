'''
Python wrapper for metal shader compiler.
'''
import subprocess, os, sys, binascii
import genutil as util

#-------------------------------------------------------------------------------
def writeFile(f, lines) :
    '''
    Write an array of lines to a file.
    '''
    for line in lines :
        f.write(str.encode(line.content + '\n'))

#-------------------------------------------------------------------------------
def run(platform, run_cmd) :
    # run a generic command through xcrun an capture stdout
    if platform == 'ios':
        sdk = 'iphoneos'
    else:
        sdk = 'macosx'
    cmd = ['xcrun', '--sdk', sdk, '--run']
    cmd.extend(run_cmd)
    child = subprocess.Popen(cmd, stderr=subprocess.PIPE)
    out = ''
    while True :
        out += bytes.decode(child.stderr.read())
        if child.poll() != None :
            break
    return out

#-------------------------------------------------------------------------------
def cc(platform, in_src, out_dia, out_air) :
    # run the metal compiler
    if platform == 'ios':
        min_version_arg = '-miphoneos-version-min=9.0'
        std_arg = '-std=ios-metal1.0'
    else :
        min_version_arg = '-mmacosx-version-min=10.11'
        std_arg = '-std=osx-metal1.1'
    cmd = ['metal', '-arch', 'air64', '-emit-llvm', '-ffast-math', '-c', 
           '-serialize-diagnostics', out_dia,
           '-o', out_air,
           min_version_arg, std_arg,
           in_src]
    return run(platform, cmd)

#-------------------------------------------------------------------------------
def ar(platform, in_air, out_lib) :
    # run the metal librarian
    cmd = ['metal-ar', 'r', out_lib, in_air]
    return run(platform, cmd)

#-------------------------------------------------------------------------------
def link(platform, in_lib, out_bin) :
    # run the metal linker
    cmd = ['metallib', '-o', out_bin, in_lib]
    return run(platform, cmd)

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
def writeBinHeader(in_bin, out_hdr, c_name) :
    '''
    Write the metallib binary data into a C header file.
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

#-------------------------------------------------------------------------------
def compile(lines, base_path, c_name, args) :
   
    platform = util.getEnv('target_platform')
    if platform != 'ios' and platform != 'osx' :
        return

    # filenames
    metal_src_path = base_path + '.metal'
    metal_dia_path = base_path + '.dia'
    metal_air_path = base_path + '.air'
    metal_lib_path = base_path + '.metal-ar'
    metal_bin_path = base_path + '.metallib'
    c_header_path  = base_path + '.metallib.h'

    # compile .metal source file
    output = cc(platform, metal_src_path, metal_dia_path, metal_air_path)
    parseOutput(output, lines)
    output += ar(platform, metal_air_path, metal_lib_path)
    output += link(platform, metal_lib_path, metal_bin_path)
    writeBinHeader(metal_bin_path, c_header_path, c_name)

