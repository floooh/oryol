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

def compile(base_path, args):
    src_path = base_path + '.spv'
    tool = getToolPath()

    # GLSL ES 100
    dst_path = base_path + '.glsl100.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--es', '--version', '100' ]
    subprocess.call(cmd)    

    # GLSL ES 300
    dst_path = base_path + '.glsles3.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--es', '--version', '300' ]
    subprocess.call(cmd)

    # GLSL 120
    dst_path = base_path + '.glsl120.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--version', '120' ]
    subprocess.call(cmd)

    # GLSL 330
    dst_path = base_path + '.glsl330.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--version', '330' ]
    subprocess.call(cmd)

    # MetalSL
    dst_path = base_path + '.metal'
    cmd = [ tool, '--output', dst_path, src_path, '--msl']
    subprocess.call(cmd)

    # HLSL5
    dst_path = base_path + '.hlsl'
    cmd = [ tool, '--output', dst_path, src_path, '--hlsl', '--shader-model', '50']
    subprocess.call(cmd)
