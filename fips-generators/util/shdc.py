'''
wrapper-script for the oryol-shdc tool (wrapper around SPIRV-Cross)
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
    return path + 'oryol-shdc'

#-------------------------------------------------------------------------------
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

#-------------------------------------------------------------------------------
def compile(input, base_path, args):
    util.setErrorLocation(input, 0)
    src_path = base_path + '.spv'
    tool = getToolPath()
    cmd = [tool, '-spirv', src_path]
    run(cmd)