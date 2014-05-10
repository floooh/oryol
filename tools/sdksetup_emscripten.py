'''
Helper functions to setup the emscripten SDK with fastcomp llvm backend.
See https://github.com/kripken/emscripten/wiki/LLVM-Backend for details.
NOTE: this will setup the emscripten SDK in the unstable incoming branch!
'''

import sys
import os
import platform
import subprocess
import urllib
from string import Template

ProjectDirectory = os.path.dirname(os.path.abspath(__file__)) + '/..'
EmscGitUrl = 'https://github.com/kripken/emscripten.git'
EmscFastCompGitUrl = 'https://github.com/kripken/emscripten-fastcomp.git'
EmscFastCompClangGitUrl = 'https://github.com/kripken/emscripten-fastcomp-clang.git'

#-------------------------------------------------------------------------------
def queryYesNo(question) :
    validAnswers={'': False, 'yes': True, 'ye': True, 'y': True, 'no': False, 'n': False }
    while True :
        sys.stdout.write(question + ' [y/N]: ')
        choice = raw_input().lower()
        if choice in validAnswers :
            return validAnswers[choice]
        else :
            sys.stdout.write("please response with 'yes' or 'no' (or 'y' / 'n')\n")

#-------------------------------------------------------------------------------
def urlDownloadHook(count, blockSize, totalSize) :
    percent = int(count * blockSize * 100 / totalSize)
    sys.stdout.write('\r{}%'.format(percent))

#-------------------------------------------------------------------------------
def error(msg) :
    print "ERROR: {}".format(msg)
    sys.exit(10)

#-------------------------------------------------------------------------------
def getSdkDir() :
    if platform.system() == 'Darwin' :        
        return ProjectDirectory + '/sdks/osx'
    elif platform.system() == 'Windows' :
        return ProjectDirectory + '/sdks/windows'
    else :
        return ProjectDirectory + '/sdks/linux'

#-------------------------------------------------------------------------------
def ensureSdkDirectory() :
    if not os.path.exists(getSdkDir()) :
        os.makedirs(getSdkDir())

#-------------------------------------------------------------------------------
def getEmscSdkPath() :
    return getSdkDir() + '/emscripten'

#-------------------------------------------------------------------------------
def getEmscFastcompPath() :
    return getSdkDir() + '/emscripten-fastcomp'

#-------------------------------------------------------------------------------
def getEmscFastcompToolsPath() :
    return getEmscFastcompPath() + '/tools'

#-------------------------------------------------------------------------------
def gitCloneOrUpdate(url, dirName, branch, cwd) :
    pathToRepo = cwd + '/' + dirName
    if not os.path.exists(pathToRepo) :
        cmd = ['git', 'clone', url, dirName ]
        subprocess.call(args=cmd, cwd=cwd)
    else :
        cmd = ['git', 'pull']
        subprocess.call(args=cmd, cwd=pathToRepo)
    cmd = ['git', 'checkout', branch]
    subprocess.call(args=cmd, cwd=pathToRepo)

#-------------------------------------------------------------------------------
def buildClang() :
    buildDir = getEmscFastcompPath() + '/build'
    if not os.path.exists(buildDir) :
        os.makedirs(buildDir)
    cmd = ['sh', '../configure', '--enable-optimized', '--disable-assertions', '--enable-targets=host,js']
    subprocess.call(args=cmd, cwd=buildDir)
    subprocess.call(args=['make', '-j3'], cwd=buildDir)

#-------------------------------------------------------------------------------
def writeEmscConfigFile() :

    # paths to emscripten dir and clang
    emscRoot = os.path.abspath(getEmscSdkPath())
    llvmRoot = os.path.abspath(getEmscFastcompPath() + '/build/Release/bin')

    # load the template file
    f = open(ProjectDirectory + '/tools/dot_emscripten.template')
    templ = Template(f.read())
    f.close

    # parse template 
    content = templ.safe_substitute(emscripten_root=emscRoot, llvm_root=llvmRoot)

    # write emscripten config file
    f = open(os.path.expanduser('~/.emscripten'), 'w')
    f.write(content)
    f.close()

#-------------------------------------------------------------------------------
def testEmcc() :
    ret = subprocess.call(['python', 'emcc', '--version'], cwd=getEmscSdkPath())
    if ret != 0 :
        error('emcc is not properly installed!')
    else :
        print('All ok!')

#-------------------------------------------------------------------------------
def setupEmscripten() :
    '''
    Setup everything needed for Oryol emscripten development.
    '''

    # first check if we would overwrite anything important
    if os.path.isfile(os.path.expanduser('~/.emscripten')) :
        if not queryYesNo("This will overwrite your existing ~/.emscripten file, continue?") :
            print "Nothing done."
            return

    if platform.system() == 'Windows' :
        error('Not yet supported on Windows')
    ensureSdkDirectory()

    # git clone everything
    print "=> getting emscripten SDK..."
    gitCloneOrUpdate(EmscGitUrl, 'emscripten', 'incoming', getSdkDir())
    print "=> getting fastcomp LLVM..."
    gitCloneOrUpdate(EmscFastCompGitUrl, 'emscripten-fastcomp', 'incoming', getSdkDir())
    print "=> getting fastcomp-clang..."
    gitCloneOrUpdate(EmscFastCompClangGitUrl, 'clang', 'incoming', getEmscFastcompToolsPath())
    print "=> configuring and building clang..."
    buildClang()
    print "=> write ~/.emscripten file"
    writeEmscConfigFile()
    print "=> testing emcc execution"
    testEmcc()