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
import zipfile
from string import Template

hostPlatform = platform.system()

ProjectDirectory = os.path.dirname(os.path.abspath(__file__)) + '/..'
EmscGitUrl = 'https://github.com/kripken/emscripten.git'
EmscFastCompGitUrl = 'https://github.com/kripken/emscripten-fastcomp.git'
EmscFastCompClangGitUrl = 'https://github.com/kripken/emscripten-fastcomp-clang.git'

EmsdkUrls = {
    'Windows' : 'https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-1.22.0-portable-64bit.zip',
    'Darwin' : 'https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz',
    'Linux' : 'https://s3.amazonaws.com/mozilla-games/emscripten/releases/emsdk-portable.tar.gz'
}

EmsdkArchivePaths = {
    'Windows' : 'emsdk-1.22.0-portable-64bit.zip',
    'Darwin' : 'emsdk-portable.tar.gz',
    'Linux' : 'emsdk-portable.tar.gz'
}

EmsdkPaths = {
    'Windows' : 'emsdk_portable',
    'Darwin' : 'emsdk_portable',
    'Linux' : 'emsdk_portable'
}

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
    if hostPlatform == 'Darwin' :        
        return ProjectDirectory + '/sdks/osx'
    elif hostPlatform == 'Windows' :
        return ProjectDirectory + '/sdks/windows'
    else :
        return ProjectDirectory + '/sdks/linux'

#-------------------------------------------------------------------------------
def ensureSdkDirectory() :
    if not os.path.exists(getSdkDir()) :
        os.makedirs(getSdkDir())

#-------------------------------------------------------------------------------
def getEmscConfigFilePath() :
    return getSdkDir() + '/.emscripten'

#-------------------------------------------------------------------------------
def getEmscriptenPath() :
    return getSdkDir() + '/emscripten'

#-------------------------------------------------------------------------------
def ensureEmscriptenPath() :
    if not os.path.exists(getEmscriptenPath()) :
        os.makedirs(getEmscriptenPath())

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
    emscRoot = os.path.abspath(getEmscriptenPath())
    llvmRoot = os.path.abspath(getEmscFastcompPath() + '/build/Release/bin')

    # load the template file
    f = open(ProjectDirectory + '/tools/dot_emscripten.template')
    templ = Template(f.read())
    f.close

    # parse template 
    content = templ.safe_substitute(emscripten_root=emscRoot, llvm_root=llvmRoot)

    # write emscripten config file
    f = open(getEmscConfigFilePath(), 'w')
    f.write(content)
    f.close()

#-------------------------------------------------------------------------------
def testEmcc() :
    ret = subprocess.call(['python', 'emcc', '--version', '--em-config', getEmscConfigFilePath() ], cwd=getEmscriptenPath())
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
    if os.path.isfile(getEmscConfigFilePath()) :
        if not queryYesNo("This will overwrite your existing %s, continue?" % getEmscConfigFilePath()) :
            print "Nothing done."
            return

    if hostPlatform == 'Windows' :
        error('Not supported on Windows, please use "oryol setup emsdk"')
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
    print "=> write .emscripten file"
    writeEmscConfigFile()
    print "=> testing emcc execution"
    testEmcc()

#-------------------------------------------------------------------------------
def urlDownloadHook(count, blockSize, totalSize) :
    percent = int(count * blockSize * 100 / totalSize)
    sys.stdout.write('\r{}%'.format(percent))

#-------------------------------------------------------------------------------
def getEmSdkUrl() :
    if hostPlatform in EmsdkUrls :
        return EmsdkUrls[hostPlatform]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def getEmSdkArchivePath() :
    if hostPlatform in EmsdkArchivePaths :
        return getSdkDir() + '/' + EmsdkArchivePaths[hostPlatform]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def getEmsdkPath() :
    if hostPlatform in EmsdkPaths :
        return getSdkDir() + '/' + EmsdkPaths[hostPlatform]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def ensureEmsdkPath() :
    if not os.path.exists(getEmsdkPath()) :
        os.makedirs(getEmsdkPath())

#-------------------------------------------------------------------------------
def uncompress(srcPath, dstPath, zipDirName) :
    if '.zip' in srcPath :
        with zipfile.ZipFile(srcPath, 'r') as archive:
            archive.extractall(dstPath + '/' + zipDirName)
    elif '.tgz' or '.bz2' in path :
        subprocess.call(args=['tar', '-xvf', srcPath], cwd=dstPath)

#-------------------------------------------------------------------------------
def prepare(path) :
    print "prepare {}".format(path)
    if hostPlatform == 'Windows' :
        subprocess.call(args=['emsdk.bat', 'update'], cwd=path, shell=True)
        subprocess.call(args=['emsdk.bat', 'install', 'sdk-incoming-64bit'], cwd=path, shell=True)
        subprocess.call(args=['emsdk.bat', 'activate', 'sdk-incoming-64bit'], cwd=path, shell=True)
    else :
        # fixme: 
        subprocess.call(args=['./emsdk update'], cwd=path, shell=True)
        subprocess.call(args=['./emsdk install sdk-incoming-64bit'], cwd=path, shell=True)
        subprocess.call(args=['./emsdk activate sdk-incoming-64bit'], cwd=path, shell=True)

#-------------------------------------------------------------------------------
def setupEmSdk() :
    '''
    Setup the official emscripten SDK.
    '''

    if not queryYesNo("This will overwrite the global ~/.emscripten config file, continue?") :
        print "Nothing done."
        return

    ensureSdkDirectory()
    ensureEmsdkPath()

    print "=> downloading emscripten SDK..."
    sdkUrl = getEmSdkUrl()
    urllib.urlretrieve(sdkUrl, getEmSdkArchivePath(), urlDownloadHook)
    print '\n => unpacking emscripten SDK...'
    uncompress(getEmSdkArchivePath(), getSdkDir(), EmsdkPaths[hostPlatform])
    print '\n => preparing emscripten SDK...'
    prepare(getEmsdkPath())


