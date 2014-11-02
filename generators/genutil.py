'''
    Common utilities for generator scripts.
'''
import sys
import os
import platform

FilePath = ''
LineNumber = 0

#-------------------------------------------------------------------------------
def error(msg) :
    '''
    Just print a simple error message and return with error code 10.
    '''
    print "ERROR: {}".format(msg)
    sys.exit(10)

#-------------------------------------------------------------------------------
def setErrorLocation(filePath, lineNumber) :
    global FilePath
    global LineNumber
    FilePath = filePath
    LineNumber = lineNumber

#-------------------------------------------------------------------------------
def fmtError(msg, terminate=True) :
    '''
    Print an error message formatted so that IDEs can parse them,
    and return with error code 10.
    '''
    if platform.system() == 'Windows' :
        print '{}({}): error: {}'.format(FilePath, LineNumber + 1, msg)
    else :
        print '{}:{}: error: {}\n'.format(FilePath, LineNumber + 1, msg)
    if terminate:
        sys.exit(10)

#-------------------------------------------------------------------------------
def fileVersionDirty(filePath, version) :
    '''
    Reads the first 4 lines of a file, checks whether there's an 
    $$version:X statemenet in it, returns False if the version
    number in the file is equal to the arg version.
    '''
    f = open(filePath, 'r')
    for i in range(0,4) :
        line = f.readline()
        startIndex = line.find('#version:')
        if startIndex != -1 :
            endIndex = line.find('#', startIndex + 9)
            if endIndex != -1 :
                versionNumber = line[startIndex + 9 : endIndex]
                if int(versionNumber) == version :
                    return False

    # fallthrough: no version or version doesn't match
    return True

#-------------------------------------------------------------------------------
def isDirty(srcFiles, version, dstSrcPath, dstHdrPath) :
    '''
    Check if code generation needs to run by comparing version stamp
    and time stamps of a number of source files, and generated
    source and header files.
    '''
    if fileVersionDirty(dstSrcPath, version) or fileVersionDirty(dstHdrPath, version) :
        return True

    dstSrcTime = os.path.getmtime(dstSrcPath)
    dstHdrTime = os.path.getmtime(dstHdrPath)
    for srcFile in srcFiles :
        srcTime = os.path.getmtime(srcFile)
        if srcTime > dstSrcTime or srcTime > dstHdrTime :
            return True
    return False
