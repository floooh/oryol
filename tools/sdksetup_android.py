'''
Helper functions for setting up the Android SDK. This is called from the
main oryol script.
'''

import sys
import os
import platform
import subprocess
import urllib

ProjectDirectory = os.path.dirname(os.path.abspath(__file__)) + '/..'

androidSdkUrls = {
    'Windows' : 'http://dl.google.com/android/android-sdk_r22.6.2-windows.zip',
    'Darwin' : 'http://dl.google.com/android/android-sdk_r22.6.2-macosx.zip',
    'Linux' : 'http://dl.google.com/android/android-sdk_r22.6.2-linux.tgz'
}

androidSdkArchivePaths = {
    'Windows' : 'android-sdk_r22.6.2-windows.zip',
    'Darwin' : 'android-sdk_r22.6.2-macosx.zip',
    'Linux' : 'android-sdk_r22.6.2-linux.tgz'
}

androidSdkPaths = { 
    'Windows' : 'android-sdk-windows',
    'Darwin' : 'android-sdk-macosx',
    'Linux' : 'android-sdk-linux'    
}

androidNdkUrls = {
    'Windows' : 'http://dl.google.com/android/ndk/android-ndk-r9d-windows-x86.zip',
    'Darwin' : 'http://dl.google.com/android/ndk/android-ndk-r9d-darwin-x86_64.tar.bz2',
    'Linux' : 'http://dl.google.com/android/ndk/android-ndk-r9d-linux-x86_64.tar.bz2'
}

androidNdkArchivePaths = {
    'Windows' : 'android-ndk-r9d-windows-x86.zip',
    'Darwin' : 'android-ndk-r9d-darwin-x86_64.tar.bz2',
    'Linux' : 'android-ndk-r9d-linux-x86_64.tar.bz2'
}

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
def getAndroidSdkPath() :
    if platform.system() in androidSdkPaths :
        return getSdkDir() + '/' + androidSdkPaths[platform.system()]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def getAndroidNdkPath() :
    return getSdkDir() + '/android-ndk-r9d'

#-------------------------------------------------------------------------------
def getAndroidToolchainPath() :
    return getSdkDir() + '/android-toolchain'  

#-------------------------------------------------------------------------------
def getAndroidSdkUrl() :
    if platform.system() in androidSdkUrls :
        return androidSdkUrls[platform.system()]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def getAndroidSdkArchivePath() :
    if platform.system() in androidSdkArchivePaths :
        return getSdkDir() + '/' + androidSdkArchivePaths[platform.system()]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def getAndroidNdkUrl() :
    if platform.system() in androidSdkUrls :
        return androidNdkUrls[platform.system()]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def getAndroidNdkArchivePath() :
    if platform.system() in androidNdkArchivePaths :
        return getSdkDir() + '/' + androidNdkArchivePaths[platform.system()]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def getAdbPath() :
    return getAndroidSdkPath() + '/platform-tools/adb'        

#-------------------------------------------------------------------------------
def checkAndroidSdk() :
    if not os.path.isdir(getAndroidSdkPath()) :
        return False
    if not os.path.isdir(getAndroidNdkPath()) :
        return False
    if not os.path.isdir(getAndroidToolchainPath()) :
        return False
    return True

#-------------------------------------------------------------------------------
def uncompress(path) :
    if '.zip' in path :
        subprocess.call(args=['unzip', path], cwd=getSdkDir())
    elif '.tgz' or '.bz2' in path :
        subprocess.call(args=['tar', '-xvf', path], cwd=getSdkDir())

#-------------------------------------------------------------------------------
def updateAndroidSdk() :
    cmd = ['sh', '{}/tools/android'.format(getAndroidSdkPath()),
           'update','sdk',
           '-f', '-u', 
           '-t', 'platform-tools,build-tools-19.0.3,android-19']
    print cmd
    subprocess.call(args=cmd, cwd=ProjectDirectory)

#-------------------------------------------------------------------------------
def createAndroidToolchain() :

    if platform.system() == 'Darwin' :
        toolchainSystem = 'darwin-x86_64'
    elif platform.system() == 'Linux' :
        toolchainSystem = 'linux-x86_64'
    else :
        error('FIXME: additional toolchain system names')

    cmd = ['sh', '{}/build/tools/make-standalone-toolchain.sh'.format(getAndroidNdkPath()),
           '--platform=android-19',
           '--ndk-dir={}'.format(getAndroidNdkPath()),
           '--install-dir={}'.format(getAndroidToolchainPath()),
           '--toolchain=arm-linux-androideabi-4.8',
           '--system={}'.format(toolchainSystem)]
    print cmd
    subprocess.call(args=cmd, cwd=ProjectDirectory)

#-------------------------------------------------------------------------------
def setupAndroid() :
    '''
    Setup everything needed for Oryol Android development
    '''
    if checkAndroidSdk() :
        return

    if platform.system() == 'Windows' :
        error('Not yet supported on Windows')

    ensureSdkDirectory()

    # the Android SDK...
    sdkUrl = getAndroidSdkUrl()
    print '\n=> downloading Android SDK: {}...'.format(sdkUrl)
    urllib.urlretrieve(sdkUrl, getAndroidSdkArchivePath(), urlDownloadHook)
    print '\n => unpacking SDK...'
    uncompress(getAndroidSdkArchivePath())
    print '\n => downloading additional SDK files...'
    updateAndroidSdk()

    # ... and the NDK plus standalone toolchain
    ndkUrl = getAndroidNdkUrl()
    print '\n=> download Android NDK: {}...'.format(ndkUrl)
    urllib.urlretrieve(ndkUrl, getAndroidNdkArchivePath(), urlDownloadHook)
    print '\n => unpacking NDK...'
    uncompress(getAndroidNdkArchivePath())
    print '\n => creating standalone toolchain...'
    createAndroidToolchain()
