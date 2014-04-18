#!/usr/bin/env python
'''
Oryol texture export functions
'''
import sys
import os
import platform
import subprocess

ProjectDirectory = os.path.dirname(os.path.abspath(__file__)) + '/..'
TexSrcDirectory = ProjectDirectory + '/data'
TexDstDirectory = ProjectDirectory + '/build/webpage'

#-------------------------------------------------------------------------------
def error(msg) :
    print "ERROR: {}".format(msg)
    sys.exit(10)

#-------------------------------------------------------------------------------
def configure(projDir, texSrcDir, texDstDir) :
    '''
    Configure the directories of the texture export module
    '''
    ProjectDirectory = projDir
    TexSrcDirectory = projDir + '/' + texSrcDir
    TexDstDirectory = projDir + '/' + texDstDir

#-------------------------------------------------------------------------------
def getToolsBinPath() :
    path = os.path.dirname(os.path.abspath(__file__))
    if platform.system() == 'Windows' :
        path += '/win32/'
    elif platform.system() == 'Darwin' :
        path += '/osx/'
    elif paltform.system() == 'Linux' :
        path +=  '/linux'
    else :
        error("Unknown host system {}".format(platform.system()))
    return path;

#-------------------------------------------------------------------------------
def ensureDstDirectory() :
    if not os.path.exists(TexDstDirectory) :
        os.makedirs(TexDstDirectory)

#-------------------------------------------------------------------------------
def toDDS(srcFilename, dstFilename, fmt, rgbFmt=None) :
    '''
    Convert a file to DDS format
    '''
    ensureDstDirectory()
    nvcompress = getToolsBinPath() + 'nvcompress'
    srcPath = TexSrcDirectory + '/' + srcFilename
    dstPath = TexDstDirectory + '/' + dstFilename
    print '=== toDDS: {} => {}:'.format(srcPath, dstPath)
    cmdLine = [nvcompress, '-'+fmt]
    if rgbFmt != None :
        cmdLine.append('-rgbfmt')
        cmdLine.append(rgbFmt)
    cmdLine.append(srcPath)
    cmdLine.append(dstPath)
    subprocess.call(args=cmdLine)

#-------------------------------------------------------------------------------
def toCubeDDS(srcDir, srcExt, dstFilename, fmt, rgbFmt=None) :
    '''
    Generate a cube map and convert to dds.
    '''
    ensureDstDirectory()
    nvassemble = getToolsBinPath() + 'nvassemble'
    nvcompress = getToolsBinPath() + 'nvcompress'
    srcFiles = ['posx', 'negx', 'posy', 'negy', 'posz', 'negz']
    dstPath  = TexDstDirectory + '/' + dstFilename

    print '=== toCubeDDS: {}/{}/[posx,negx,posy,negy,posz,negz].{} => {}'.format(TexSrcDirectory, srcDir, srcExt, dstPath)

    # call nvassemble to generate an uncompressed cube map...
    cmdLine = [nvassemble, '-cube']
    for src in srcFiles :
        cmdLine.append(TexSrcDirectory + '/' + srcDir + '/' + src + '.' + srcExt)
    cmdLine.append('-o')
    cmdLine.append(dstPath)
    subprocess.call(args=cmdLine)

    # ...and compress/convert to the desired format
    cmdLine = [nvcompress, '-'+fmt]
    if rgbFmt != None :
        cmdLine.append('-rgbfmt')
        cmdLine.append(rgbFmt)
    cmdLine.append(dstPath)
    cmdLine.append(dstPath)
    print cmdLine
    subprocess.call(args=cmdLine)

#-------------------------------------------------------------------------------
def exportSampleTextures() :
    toDDS('lok256.jpg', 'lok_dxt1.dds', 'bc1')
    toDDS('lok256.jpg', 'lok_dxt3.dds', 'bc2')
    toDDS('lok256.jpg', 'lok_dxt5.dds', 'bc3')
    toDDS('lok256.jpg', 'lok_bgra8.dds', 'rgb', 'bgra8')
    toDDS('lok256.jpg', 'lok_rgba8.dds', 'rgb', 'rgba8')
    toDDS('lok256.jpg', 'lok_bgr8.dds', 'rgb', 'bgr8')
    toDDS('lok256.jpg', 'lok_rgb8.dds', 'rgb', 'rgb8')
    toDDS('lok256.jpg', 'lok_argb4.dds', 'rgb', 'argb4')
    toDDS('lok256.jpg', 'lok_abgr4.dds', 'rgb', 'abgr4')
    toDDS('lok256.jpg', 'lok_rgb565.dds', 'rgb', 'rgb565')
    toDDS('lok256.jpg', 'lok_bgr565.dds', 'rgb', 'bgr565')
    toDDS('lok256.jpg', 'lok_argb1555.dds', 'rgb', 'argb1555')
    toDDS('lok256.jpg', 'lok_abgr1555.dds', 'rgb', 'abgr1555')
    toCubeDDS('RomeChurch', 'jpg', 'romechurch_dxt1.dds', 'bc1')

#-------------------------------------------------------------------------------
if __name__ == '__main__' :
    print "{}".format(__file__)
    exportSampleTextures()