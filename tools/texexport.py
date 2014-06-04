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

# NOTE: PVRTexTools supports a lot more formats!
PVRFormats = ['PVRTC1_2', 'PVRTC1_4', 'PVRTC1_2_RGB', 'PVRTC1_4_RGB', 'PVRTC2_2', 'PVRTC2_4']

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
    elif platform.system() == 'Linux' :
        path +=  '/linux/'
    else :
        error("Unknown host system {}".format(platform.system()))
    return path;

#-------------------------------------------------------------------------------
def ensureDstDirectory() :
    if not os.path.exists(TexDstDirectory) :
        os.makedirs(TexDstDirectory)

#-------------------------------------------------------------------------------
def toDDS(srcFilename, dstFilename, linearGamma, fmt, rgbFmt=None) :
    '''
    Convert a file to DDS format
    '''
    ensureDstDirectory()
    ddsTool = getToolsBinPath() + 'nvcompress'
    srcPath = TexSrcDirectory + '/' + srcFilename
    dstPath = TexDstDirectory + '/' + dstFilename
    print '=== toDDS: {} => {}:'.format(srcPath, dstPath)
    cmdLine = [ddsTool, '-'+fmt]
    if rgbFmt != None :
        cmdLine.append('-rgbfmt')
        cmdLine.append(rgbFmt)
    if linearGamma :
        cmdLine.append('-tolineargamma')
    cmdLine.append(srcPath)
    cmdLine.append(dstPath)
    subprocess.call(args=cmdLine)

#-------------------------------------------------------------------------------
def toCubeDDS(srcDir, srcExt, dstFilename, linearGamma, fmt, rgbFmt=None) :
    '''
    Generate a cube map and convert to dds.
    '''
    ensureDstDirectory()
    nvassemble = getToolsBinPath() + 'nvassemble'
    ddsTool = getToolsBinPath() + 'nvcompress'
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
    cmdLine = [ddsTool, '-'+fmt]
    if rgbFmt != None :
        cmdLine.append('-rgbfmt')
        cmdLine.append(rgbFmt)
    if linearGamma :
        cmdLine.append('-tolineargamma')        
    cmdLine.append(dstPath)
    cmdLine.append(dstPath)
    subprocess.call(args=cmdLine)

#-------------------------------------------------------------------------------
def toPVR(srcFilename, dstFilename, format) :
    '''
    Convert a file to PVR format
    '''
    if format not in PVRFormats :
        error('invalid PVR texture format {}!'.format(format))

    ensureDstDirectory()
    pvrTool = getToolsBinPath() + 'PVRTexToolCLI'
    srcPath = TexSrcDirectory + '/' + srcFilename
    dstPath = TexDstDirectory + '/' + dstFilename
    print '=== toPVR: {} => {}:'.format(srcPath, dstPath)
    cmdLine = [pvrTool, '-i', srcPath, '-o', dstPath, '-square', '+', '-pot', '+', '-m', '-mfilter', 'cubic', '-f', format ]
    subprocess.call(args=cmdLine)

#-------------------------------------------------------------------------------
def toCubePVR(srcDir, srcExt, dstFilename, format) :
    '''
    Generate a cube map and convert to PVR
    '''
    if format not in PVRFormats :
        error('invalid PVR texture format {}!'.format(format))

    ensureDstDirectory()
    pvrTool = getToolsBinPath() + 'PVRTexToolCLI'
    srcFiles = ['posx', 'negx', 'posy', 'negy', 'posz', 'negz']
    dstPath  = TexDstDirectory + '/' + dstFilename

    print '=== toCubePVR: {}/{}/[posx,negx,posy,negy,posz,negz].{} => {}'.format(TexSrcDirectory, srcDir, srcExt, dstPath)

    cmdLine = [pvrTool, '-i']
    inputFiles = ''
    for src in srcFiles :
        inputFiles += TexSrcDirectory + '/' + srcDir + '/' + src + '.' + srcExt + ','
    inputFiles = inputFiles[:-1]
    cmdLine.append(inputFiles)
    cmdLine.append('-o')
    cmdLine.append(dstPath)
    cmdLine.append('-cube')
    cmdLine.append('-m')
    cmdLine.append('-mfilter')
    cmdLine.append('cubic')
    cmdLine.append('-f')
    cmdLine.append(format)
    subprocess.call(args=cmdLine)

#-------------------------------------------------------------------------------
def exportSampleTextures() :
    # default gamma 2.2
    toDDS('lok256.jpg', 'lok_dxt1.dds', False, 'bc1')
    toDDS('lok256.jpg', 'lok_dxt3.dds', False, 'bc2')
    toDDS('lok256.jpg', 'lok_dxt5.dds', False, 'bc3')
    toDDS('lok256.jpg', 'lok_bgra8.dds', False, 'rgb', 'bgra8')
    toDDS('lok256.jpg', 'lok_rgba8.dds', False, 'rgb', 'rgba8')
    toDDS('lok256.jpg', 'lok_bgr8.dds', False, 'rgb', 'bgr8')
    toDDS('lok256.jpg', 'lok_rgb8.dds', False, 'rgb', 'rgb8')
    toDDS('lok256.jpg', 'lok_argb4.dds', False, 'rgb', 'argb4')
    toDDS('lok256.jpg', 'lok_abgr4.dds', False, 'rgb', 'abgr4')
    toDDS('lok256.jpg', 'lok_rgb565.dds', False, 'rgb', 'rgb565')
    toDDS('lok256.jpg', 'lok_bgr565.dds', False, 'rgb', 'bgr565')
    toDDS('lok256.jpg', 'lok_argb1555.dds', False, 'rgb', 'argb1555')
    toDDS('lok256.jpg', 'lok_abgr1555.dds', False, 'rgb', 'abgr1555')
    toCubeDDS('RomeChurch', 'jpg', 'romechurch_dxt1.dds', False, 'bc1')

    # linear gamma
    toDDS('lok256.jpg', 'lok_linear_dxt1.dds', True, 'bc1')
    toDDS('lok256.jpg', 'lok_linear_dxt3.dds', True, 'bc2')
    toDDS('lok256.jpg', 'lok_linear_dxt5.dds', True, 'bc3')
    toDDS('lok256.jpg', 'lok_linear_bgra8.dds', True, 'rgb', 'bgra8')
    toDDS('lok256.jpg', 'lok_linear_rgba8.dds', True, 'rgb', 'rgba8')
    toDDS('lok256.jpg', 'lok_linear_bgr8.dds', True, 'rgb', 'bgr8')
    toDDS('lok256.jpg', 'lok_linear_rgb8.dds', True, 'rgb', 'rgb8')
    toDDS('lok256.jpg', 'lok_linear_argb4.dds', True, 'rgb', 'argb4')
    toDDS('lok256.jpg', 'lok_linear_abgr4.dds', True, 'rgb', 'abgr4')
    toDDS('lok256.jpg', 'lok_linear_rgb565.dds', True, 'rgb', 'rgb565')
    toDDS('lok256.jpg', 'lok_linear_bgr565.dds', True, 'rgb', 'bgr565')
    toDDS('lok256.jpg', 'lok_linear_argb1555.dds', True, 'rgb', 'argb1555')
    toDDS('lok256.jpg', 'lok_linear_abgr1555.dds', True, 'rgb', 'abgr1555')
    toCubeDDS('RomeChurch', 'jpg', 'romechurch_linear_dxt1.dds', True, 'bc1')

    # PVRTC
    toPVR('lok256.jpg', 'lok_bpp2.pvr', 'PVRTC1_2')
    toPVR('lok256.jpg', 'lok_bpp4.pvr', 'PVRTC1_4')
    toCubePVR('RomeChurch', 'jpg', 'romechurch_bpp2.pvr', 'PVRTC1_2')

#-------------------------------------------------------------------------------
if __name__ == '__main__' :
    print "{}".format(__file__)
    exportSampleTextures()
