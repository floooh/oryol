'''
Code generator for shader libraries.
'''

import os
import sys
import glob
from pprint import pprint

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

UniformTypes = [ 'bool', 'int', 'uint', 'float', 
                 'bvec2', 'bvec3', 'bvec4',
                 'ivec2', 'ivec3', 'ivec4',
                 'mat2', 'mat3', 'mat4',
                 'sampler1D', 'sampler2D', 'sampler2DRect', 
                 'sampler3D', 'sampler3DRect', 'samplerCube' ]
AttrTypes = [ 'float', 'vec2', 'vec3', 'vec4', 'mat2', 'mat3', 'mat4' ]

#-------------------------------------------------------------------------------
def error(msg) :
    print "ERROR: {}".format(msg)
    sys.exit(10)

#-------------------------------------------------------------------------------
def dump(obj) :
    pprint(vars(obj))

#-------------------------------------------------------------------------------
class Snippet :
    '''
    A snippet from a shader file, can be a function, vertex/fragment shader,
    etc...
    '''
    def __init__(self) :
        self.name = None
        self.deps = []
        self.path = None
        self.lineIndex = 0
        self.numLindex = 0

#-------------------------------------------------------------------------------
class Func(Snippet) :
    '''
    A function snippet
    '''
    def __init__(self) :
        super(Func, self).__init__()
        
#-------------------------------------------------------------------------------
class Uniform :
    '''
    A shader uniform definition.
    '''
    def __init__(self) :
        self.type = None
        self.name = None
        self.bind = None

#-------------------------------------------------------------------------------
class Attr :
    '''
    A shader input or output attribute.
    '''         
    def __init__(self) :
        self.type = None
        self.name = None

#-------------------------------------------------------------------------------
class VertexShader(Snippet) :
    '''
    A vertex shader function.
    '''
    def __init__(self) :
        super(VertexShader, self).__init__()
        self.uniforms = []
        self.inAttrs = []
        self.outAttrs = []

#-------------------------------------------------------------------------------
class FragmentShader(Snippet) :
    '''
    A fragment shader function.
    '''
    def __init__(self) :
        super(FragmentShader, self).__init__()
        self.inAttrs = []
        self.uniforms = []

#-------------------------------------------------------------------------------
class Program() :
    '''
    A shader program, made of vertex/fragment shader and uniforms
    '''
    def __init__(self) :
        self.name = None
        self.vs = None
        self.fs = None
        self.features = []
        self.uniforms = []

#-------------------------------------------------------------------------------
class ProgramBundle() :
    '''
    A program bundle (array of vertex/fragment shader tuples, and uniforms)
    '''
    def __init__(self) :
        self.name = None
        self.programs = []
        self.uniforms = []

#-------------------------------------------------------------------------------
class Parser :
    '''
    Populate a shader library from annotated shader source files.
    '''
    def __init__(self, shaderLib) :
        self.shaderLib = shaderLib
        self.current = None

    def parseLine(self, line, lineNumber) :
        '''
        Parse a single line.
        '''
        # check if the line contains a tag
        tagStartIndex = line.find('${')
        if tagStartIndex != -1 :
            tagEndIndex = line.index('}', tagStartIndex)
            tagString = line[tagStartIndex+2:tagEndIndex]
            print "found tag '{}'".format(tagString)

    def parse(self, path) :
        '''
        Parse a single file and populate shader lib
        '''
        print '=> parsing {}'.format(path)
        f = open(path, 'r')
        lineNumber = 0
        for line in f :
            self.parseLine(line, lineNumber)
            lineNumber += 1
        f.close()

#-------------------------------------------------------------------------------
class ShaderLibrary :
    '''
    This represents the entire shader lib.
    '''
    def __init__(self, xmlTree, absXmlPath) :
        self.xmlRoot = xmlTree.getroot()
        self.xmlPath = absXmlPath
        self.name = None
        self.dirs = []
        self.sources = []
        self.funcs = {}
        self.vertexShaders = {}
        self.fragmentShaders = {}
        self.programs = {}
        self.programBundles = {}
        self.current = None

    '''
    Parse the root xml file, this sets the name
    and the source directories members.
    '''
    def parseXmlTree(self) :
        rootDir = os.path.dirname(self.xmlPath)
        print 'ROOT DIR: {}'.format(rootDir)
        self.name = self.xmlRoot.get('name')
        for dir in self.xmlRoot.findall('AddDir') :
            self.dirs.append(rootDir + '/' + dir.get('path'))

    '''
    This gathers the shader source file names from
    all source directories.
    '''
    def gatherSources(self) :
        for dir in self.dirs :
            self.sources.extend(glob.glob(dir + '/*.shd'))

    '''
    Parse one source file.
    '''
    def parseSources(self) :
        parser = Parser(self)
        for source in self.sources :            
            parser.parse(source)

#-------------------------------------------------------------------------------
def generateHeader(xmlTree, absHeaderPath) :
    '''
    Generate the C++ header file 
    '''
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
def isDirty(xmlTree, absXmlPath, absSourcePath, absHeaderPath) :
    # FIXME: check time stamp of shader sources against 
    # source/header (if exists)
    return True

#-------------------------------------------------------------------------------
def generate(xmlTree, absXmlPath, absSourcePath, absHeaderPath) :

    shaderLibrary = ShaderLibrary(xmlTree, absXmlPath)
    shaderLibrary.parseXmlTree()
    shaderLibrary.gatherSources()
    dump(shaderLibrary)
    shaderLibrary.parseSources()

    generateHeader(xmlTree, absHeaderPath)
    generateSource(xmlTree, absSourcePath)
