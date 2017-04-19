'''
Wrapper script for spirv-cross tool
'''
import subprocess, platform, os, sys
import genutil as util

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
    return path + 'spirv-cross'

def run(cmd):
    child = subprocess.Popen(cmd, stderr=subprocess.PIPE)
    out = ''
    while True :
        out += bytes.decode(child.stderr.read())
        if child.poll() != None :
            break
    for line in out.splitlines():
        util.fmtError(line, False)
    if child.returncode != 0:
        exit(child.returncode)

def compile(input, base_path, args):
    util.setErrorLocation(input, 0)
    src_path = base_path + '.spv'
    tool = getToolPath()

    # GLSL ES 100
    dst_path = base_path + '.glsl100.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--es', '--version', '100' ]
    run(cmd)

    # GLSL ES 300
    dst_path = base_path + '.glsles3.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--es', '--version', '300' ]
    run(cmd)

    # GLSL 120
    dst_path = base_path + '.glsl120.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--version', '120' ]
    run(cmd)

    # GLSL 330
    dst_path = base_path + '.glsl330.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--version', '330' ]
    run(cmd)

    # MetalSL
    dst_path = base_path + '.metal'
    cmd = [ tool, '--output', dst_path, src_path, '--msl']
    run(cmd)

    # HLSL5
    dst_path = base_path + '.hlsl'
    cmd = [ tool, '--output', dst_path, src_path, '--hlsl', '--shader-model', '50']
    run(cmd)
