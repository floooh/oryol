'''
Code generator for shader libraries.
'''

import os
import sys

'''
REMINDER:

shaderLibrary:
    name
    functions:
        func: name, functions

        ...
    vertex_shaders:
        vs: name
            uniform: type, name, bind
            in: type, name
            out: type, name
            functions
    
    fragment_shaders:
        fs: name
            uniform: type, name, bind
            in: type, name
            out: type, name
            functions

    programs:
        program: vs, fs

    shaders:
        shader: programs
'''

#-------------------------------------------------------------------------------
def error(msg) :
    print "ERROR: {}".format(msg)
    sys.exit(10)

#-------------------------------------------------------------------------------
def generateHeader(xmlTree, absHeaderPath) :
    '''
    Generate the C++ header file 
    '''
    xmlRoot = xmlTree.getroot()
    f = open(absHeaderPath, 'w')
    f.close()

#-------------------------------------------------------------------------------
def generateSource(xmlTree, absSourcePath) :
    '''
    Generate the C++ source file
    '''
    xmlRoot = xmlTree.getroot()
    f = open(absSourcePath, 'w')    
    f.close()

#-------------------------------------------------------------------------------
def generate(xmlTree, absXmlPath, absSourcePath, absHeaderPath) :
    generateHeader(xmlTree, absHeaderPath)
    generateSource(xmlTree, absSourcePath)
