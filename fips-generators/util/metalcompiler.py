'''
Python wrapper for metal shader compiler.
'''
import subprocess
import tempfile
import platform
import os
import sys
import binascii
import genutil as util

platform_roots = {
    'osx': [
        '/Applications/Xcode-beta.app/Contents/Developer/Platforms/MacOSX.platform/',
        '/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/'
    ],
    'ios': [
        '/Applications/Xcode-beta.app/Contents/Developer/Platforms/iPhoneOS.platform/',
        '/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/'
    ]
}
rel_sys_root = {
    'osx': [
        'Developer/SDKs/MacOSX10.11.sdk'
    ],
    'ios': [
        'Developer/SDKs/iPhoneOS9.2.sdk',
        'Developer/SDKs/iPhoneOS9.1.sdk',
        'Developer/SDKs/iPhoneOS9.0.sdk'
    ]
}

#-------------------------------------------------------------------------------
def get_sys_root(platform) :
    for platform_root in platform_roots[platform] :
        for sys_root in rel_sys_root[platform] :
            sys_root_path = platform_root + sys_root
            if os.path.isdir(sys_root_path) :
                return sys_root_path
    return None

#-------------------------------------------------------------------------------
def get_tool(platform, tool_name) :
    for platform_root in platform_roots[platform] :
        tool_path = platform_root + 'usr/bin/' + tool_name
        if os.path.isfile(tool_path) :
            return tool_path
    return None

#-------------------------------------------------------------------------------
def writeFile(f, lines) :
    '''
    Write an array of lines to a file.
    '''
    for line in lines :
        f.write(str.encode(line.content + '\n'))

#-------------------------------------------------------------------------------
def run(cmd) :
    # run a generic command an capture stdout
    print('cmd: {}'.format(cmd))
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
    tool = get_tool(platform, 'metal')
    if platform == 'ios':
        min_version_arg = '-miphoneos-version-min=9.0'
        std_arg = '-std=ios-metal1.0'
    else :
        min_version_arg = '-mmacosx-version-min=10.11'
        std_arg = '-std=osx-metal1.1'
    cmd = [tool, '-arch', 'air64', '-emit-llvm', '-ffast-math', '-c', 
           '-isysroot', get_sys_root(platform),
           '-serialize-diagnostics', out_dia,
           '-o', out_air,
           min_version_arg, std_arg,
           in_src]
    return run(cmd)

#-------------------------------------------------------------------------------
def ar(platform, in_air, out_lib) :
    # run the metal librarian
    cmd = [get_tool(platform, 'metal-ar'), 'r', out_lib, in_air]
    return run(cmd)

#-------------------------------------------------------------------------------
def link(platform, in_lib, out_bin) :
    # run the metal linker
    cmd = [get_tool(platform, 'metallib'), '-o', out_bin, in_lib]
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
def validate(platform, lines, outPath, c_name) :
   
    if platform != 'ios' and platform != 'osx' :
        return

    # test if tools exists
    if not get_tool(platform, 'metal') :
        util.fmtWarning('metal compiler not found\n')
        return
    if not get_tool(platform, 'metal-ar') :
        util.fmtWarning('metal librarian not found\n')
        return
    if not get_tool(platform, 'metallib') :
        util.fmtWarning('metal linker not found\n')
        return

    # filenames
    rootPath = os.path.splitext(outPath)[0]
    metal_src_path = rootPath + '.metal'
    metal_dia_path = rootPath + '.dia'
    metal_air_path = rootPath + '.air'
    metal_lib_path = rootPath + '.metal-ar'
    metal_bin_path = rootPath + '.metallib'
    c_header_path  = rootPath + '.metallib.h'

    # write metal source file
    with open(metal_src_path, 'w') as f :
        writeFile(f, lines)

    # compile .metal source file
    output = cc(platform, metal_src_path, metal_dia_path, metal_air_path)
    parseOutput(output, lines)
    output += ar(platform, metal_air_path, metal_lib_path)
    output += link(platform, metal_lib_path, metal_bin_path)
    writeBinHeader(metal_bin_path, c_header_path, c_name)

