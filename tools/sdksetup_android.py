'''
Helper functions for setting up the Android SDK. This is called from the
main oryol script.
'''

import sys
import os
import platform
import subprocess
import urllib
import zipfile

ProjectDirectory = os.path.dirname(os.path.abspath(__file__)) + '/..'
hostPlatform = platform.system()

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
def getAndroidSdkPath() :
    if hostPlatform in androidSdkPaths :
        return getSdkDir() + '/' + androidSdkPaths[hostPlatform]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def getAndroidNdkPath() :
    return getSdkDir() + '/android-ndk-r9d'

#-------------------------------------------------------------------------------
def getAndroidSdkUrl() :
    if hostPlatform in androidSdkUrls :
        return androidSdkUrls[hostPlatform]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def getAndroidSdkArchivePath() :
    if hostPlatform in androidSdkArchivePaths :
        return getSdkDir() + '/' + androidSdkArchivePaths[hostPlatform]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def getAndroidNdkUrl() :
    if hostPlatform in androidSdkUrls :
        return androidNdkUrls[hostPlatform]
    else :
        error('unknown host platform')

#-------------------------------------------------------------------------------
def getAndroidNdkArchivePath() :
    if hostPlatform in androidNdkArchivePaths :
        return getSdkDir() + '/' + androidNdkArchivePaths[hostPlatform]
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
    return True

#-------------------------------------------------------------------------------
def uncompress(path) :
    if '.zip' in path :
        with zipfile.ZipFile(path, 'r') as archive:
            archive.extractall(getSdkDir())
    elif '.bz2' or '.tgz' in path :
        # note: for some reason python's tarfile
        # module cannot completely unpack the 
        # Android NDK tar.gz2 file (tested on OSX with python 2.7),
        # so fall back to command line tar
        subprocess.call(args=['tar', '-xvf', path], cwd=getSdkDir())

#-------------------------------------------------------------------------------
def updateAndroidSdk() :
    if hostPlatform == 'Windows' :
        cmd = ['{}/tools/android.bat'.format(getAndroidSdkPath()),
               'update','sdk',
               '-f', '-u', '--all',
               '--filter', 'tools,platform-tools,build-tools-19.1.0,android-19']
    else :
        cmd = ['sh', '{}/tools/android'.format(getAndroidSdkPath()),
               'update','sdk',
               '-f', '-u', '--all',
               '--filter', 'tools,platform-tools,build-tools-19.1.0,android-19']
    print cmd
    subprocess.call(args=cmd, cwd=ProjectDirectory)

#-------------------------------------------------------------------------------
def setupAndroid() :
    '''
    Setup everything needed for Oryol Android development
    '''
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
