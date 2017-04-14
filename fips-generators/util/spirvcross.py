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
    dst_path = base_path + '.es100.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--es', '--version', '100' ]
    subprocess.call(cmd)    

    # GLSL ES 300
    dst_path = base_path + '.es300.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--es', '--version', '300']
    subprocess.call(cmd)

    # GLSL 120
    dst_path = base_path + '.gl120.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--version', '120']
    subprocess.call(cmd)

    # GLSL 330
    dst_path = base_path + '.gl330.glsl'
    cmd = [ tool, '--output', dst_path, src_path, '--version', '330']
    subprocess.call(cmd)

    # MetalSL
    dst_path = base_path + '.msl'
    cmd = [ tool, '--output', dst_path, src_path, '--msl']
    subprocess.call(cmd)

    # HLSL5
    dst_path = base_path + '.hlsl'
    cmd = [ tool, '--output', dst_path, src_path, '--hlsl', '--shader-model', '50']
    subprocess.call(cmd)
