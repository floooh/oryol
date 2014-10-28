'''
Helper functions for setting up the NaCl SDK. This is called from the
main oryol script.
'''

import sys
import os
import platform
import subprocess
import urllib
import zipfile

ProjectDirectory = os.path.dirname(os.path.abspath(__file__)) + '/..'

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
def getNaclSdkPath() :
    return getSdkDir() + '/nacl_sdk'

#-------------------------------------------------------------------------------
def getNaclSdkUrl() :
    return 'http://storage.googleapis.com/nativeclient-mirror/nacl/nacl_sdk/nacl_sdk.zip'

#-------------------------------------------------------------------------------
def getNaclSdkArchivePath() :
    return getSdkDir() + '/nacl_sdk.zip'

#-------------------------------------------------------------------------------
def uncompress(path) :
    with zipfile.ZipFile(path, 'r') as archive:
        archive.extractall(getSdkDir())

#-------------------------------------------------------------------------------
def updateNaclSdk() :
    naclsdk = '{}/naclsdk'.format(getNaclSdkPath())
    os.chmod(naclsdk, 0744)
    cmd = [naclsdk, 'install', '--force', 'pepper_canary']
    print cmd
    subprocess.call(args=cmd, cwd=ProjectDirectory)

#-------------------------------------------------------------------------------
def setupNacl() :
    '''
    Setup everything needed for Oryol Nacl development
    '''
    if platform.system() == 'Windows' :
        error('Not yet supported on Windows')

    ensureSdkDirectory()

    # get Nacl SDK, uncompress and update
    sdkUrl = getNaclSdkUrl()
    print '\n=> downloading NaCl SDK: {}...'.format(sdkUrl)
    urllib.urlretrieve(sdkUrl, getNaclSdkArchivePath(), urlDownloadHook)
    print '\n => unpacking SDK...'
    uncompress(getNaclSdkArchivePath())
    print '\n => downloading additional SDK files...'
    updateNaclSdk()




