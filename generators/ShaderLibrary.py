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
def srcError(msg, fileName, line) :
    # FIXME: format so that IDE can parse the string
    print '{}:{}: error: {}\n'.format(fileName, line, msg)
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
        self.lines = []

#-------------------------------------------------------------------------------
class Func(Snippet) :
    '''
    A function snippet
    '''
    def __init__(self, name, path) :
        Snippet.__init__(self)
        self.name = name
        self.path = path
        
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
    def __init__(self, name, path) :
        Snippet.__init__(self)
        self.name = name
        self.path = path
        self.uniforms = []
        self.inAttrs = []
        self.outAttrs = []

#-------------------------------------------------------------------------------
class FragmentShader(Snippet) :
    '''
    A fragment shader function.
    '''
    def __init__(self, name, path) :
        Snippet.__init__(self)
        self.name = name
        self.path = path
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
class Bundle() :
    '''
    A program bundle (array of vertex/fragment shader tuples, and uniforms)
    '''
    def __init__(self, name) :
        self.name = name
        self.programs = []
        self.uniforms = []

#-------------------------------------------------------------------------------
class Parser :
    '''
    Populate a shader library from annotated shader source files.
    '''
    #---------------------------------------------------------------------------
    def __init__(self, shaderLib) :
        self.shaderLib = shaderLib
        self.fileName = None
        self.lineNumber = 0
        self.current = None
        self.inComment = False

    #---------------------------------------------------------------------------
    def stripComments(self, line) :
        '''
        Remove comments from a single line, can carry
        over to next or from previous line.
        '''
        done = False
        while not done :
            # if currently in comment, look for end-of-comment
            if self.inComment :
                endIndex = line.find('*/')
                if endIndex == -1 :
                    # entire line is comment
                    if '/*' in line or '//' in line :
                        srcError('comment in comment!', self.fileName, self.lineNumber)
                    else :
                        return ''
                else :
                    comment = line[:endIndex+2]
                    if '/*' in comment or '//' in comment :
                        srcError('comment in comment!', self.fileName, self.lineNumber)
                    else :
                        line = line[endIndex+2:]
                        self.inComment = False

            # clip off winged comment (if exists)
            wingedIndex = line.find('//')
            if wingedIndex != -1 :
                line = line[:wingedIndex]

            # look for start of comment
            startIndex = line.find('/*')
            if startIndex != -1 :
                # ...and for the matching end...
                endIndex = line.find('*/', startIndex)
                if endIndex != -1 :
                    line = line[:startIndex] + line[endIndex+2:]
                else :
                    # comment carries over to next line
                    self.inComment = True
                    line = line[:startIndex]
                    done = True
            else :
                # no comment until end of line, done
                done = True;
        line = line.strip(' \t\n\r')
        return line

    #---------------------------------------------------------------------------
    def removeTag(self, line, startIndex, endIndex) :
        return line[:startIndex] + line[endIndex+1:]

    #---------------------------------------------------------------------------
    def fillTag(self, line, startIndex, endIndex, char) :
        return line[:startIndex] + char * ((endIndex + 1) - startIndex) + line[endIndex+1:]

    #---------------------------------------------------------------------------
    def newFunc(self, line, kw, args, startIndex, endIndex) :
        print 'newFunc: kw={} args={}'.format(kw, args)
        if len(args) != 1 :
            srcError("func: exactly 1 arg expected (name), {} given".format(len(args)), self.fileName, self.lineNumber)
        name = args[0]
        if name in self.shaderLib.functions :
            srcError("func: function '{}'' already defined!".format(name), self.fileName, self.lineNumber)
        func = Func(name, self.fileName)
        self.shaderLib.functions[name] = func
        self.current = func
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def newVertexShader(self, line, kw, args, startIndex, endIndex) :
        print 'newVertexShader: kw={} args={}'.format(kw, args)
        if len(args) != 1:
            srcError("vs: exactly 1 arg expected (name), {} given".format(len(args)), self.fileName, self.lineNumber)
        name = args[0]
        if name in self.shaderLib.vertexShaders :
            srcError("vs: vertex shader '{}' already defined!".format(name), self.fileName, self.lineNumber)            
        vs = VertexShader(name, self.fileName)
        self.shaderLib.vertexShaders[name] = vs
        self.current = vs        
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def newFragmentShader(self, line, kw, args, startIndex, endIndex) :
        print 'newFragmentShader: kw={} args={}'.format(kw, args)
        if len(args) != 1:
            srcError("fs: exactly 1 arg expected (name), {} given".format(len(args)), self.fileName, self.lineNumber)
        name = args[0]
        if name in self.shaderLib.fragmentShaders :
            srcError("fs: fragment shader '{}' already defined!".format(name), self.fileName, self.lineNumber)
        fs = FragmentShader(name, self.fileName)
        self.shaderLib.fragmentShaders[name] = fs
        self.current = fs
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def newBundle(self, line, kw, args, startIndex, endIndex) :
        print 'newBundle: kw={} args={}'.format(kw, args)
        if len(args) != 1:
            srcError("bundle: exactly 1 arg expected (name), {} given".format(len(args)), self.fileName, self.lineNumber);
        name = args[0]
        if name in self.shaderLib.bundles :
            srcError("bundle: bundle '{}' already defined!".format(name), self.fileName, self.lineNumber)
        bundle = Bundle(name)
        self.shaderLib.bundles[name] = bundle
        self.current = bundle            
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addInTag(self, line, kw, args, startIndex, endIndex) :
        print 'addInTag: kw={} args={}'.format(kw, args)
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addOutTag(self, line, kw, args, startIndex, endIndex) :
        print 'addOutTag: kw={} args={}'.format(kw, args)
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addReturnTag(self, line, kw, args, startIndex, endIndex) :
        print 'addReturnTag: kw={} args={}'.format(kw, args)
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addUniformTag(self, line, kw, args, startIndex, endIndex) :
        print 'addUniformTag: kw={} args={}'.format(kw, args)
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addProgramTag(self, line, kw, args, startIndex, endIndex) :
        print 'addProgramTag: kw={} args={}'.format(kw, args)
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addReplaceTag(self, line, kw, args, startIndex, endIndex) :
        print 'addReplaceTag: kw={} args={}'.format(kw, args)
        return self.fillTag(line, startIndex, endIndex, 'X')

    #---------------------------------------------------------------------------
    def addDependencyTag(self, line, kw, args, startIndex, endIndex) :
        print 'addDependencyTag: kw={} args={}'.format(kw, args)
        return self.fillTag(line, startIndex, endIndex, 'D')

    #---------------------------------------------------------------------------
    def parseTags(self, line) :
        startIndex = 0
        endIndex = 0
        while startIndex != -1 :
            startIndex = line.find('${')
            if startIndex != -1 :
                endIndex = line.find('}', startIndex)
                if endIndex != -1 :
                    tag = line[startIndex+2 : endIndex]
                    tag = tag.strip(' \t')
                    colonIndex = tag.find(':')
                    if colonIndex == -1 :
                        kw = tag
                        args = []
                    else :
                        kw = tag[:colonIndex]
                        args = tag[colonIndex+1:].split()

                    if kw == 'func':
                        line = self.newFunc(line, kw, args, startIndex, endIndex)
                    elif kw == 'vs':
                        line = self.newVertexShader(line, kw, args, startIndex, endIndex)
                    elif kw == 'fs':
                        line = self.newFragmentShader(line, kw, args, startIndex, endIndex)
                    elif kw == 'bundle':
                        line = self.newBundle(line, kw, args, startIndex, endIndex)
                    elif kw == 'in':
                        line = self.addInTag(line, kw, args, startIndex, endIndex)
                    elif kw == 'out':
                        line = self.addOutTag(line, kw, args, startIndex, endIndex)
                    elif kw == 'return':
                        line = self.addReturnTag(line, kw, args, startIndex, endIndex)
                    elif kw == 'uniform':
                        line = self.addUniformTag(line, kw, args, startIndex, endIndex)
                    elif kw == 'program':
                        line = self.addProgramTag(line, kw, args, startIndex, endIndex)
                    elif kw in ['texture2D', 'texture2DProj', 'texture2DLod', 'texture2DProjLod',
                                'textureCube', 'textureCubeLod', 'position', 'color'] :
                        if args :
                            srcError("replacement tag '{}' can't have args!".format(kw), self.fileName, self.lineNumber)
                        line = self.addReplaceTag(line, kw, args, startIndex, endIndex)
                    else :
                        if args :
                            srcError("function call tag '{}' can't have args!".format(kw), self.fileName, self.lineNumber)
                        line = self.addDependencyTag(line, kw, args, startIndex, endIndex)
                else :
                    srcError('unterminated tag', self.fileName, self.lineNumber)
        line = line.strip(' \t\r\n')
        return line

    #---------------------------------------------------------------------------
    def parseLine(self, line) :
        '''
        Parse a single line.
        '''
        line = self.stripComments(line)
        if line != '':
            line = self.parseTags(line)
            if line != '':
                print '{}: {}'.format(self.lineNumber, line)
                if self.current != None:
                    self.current.lines.append((self.lineNumber, line))

    #---------------------------------------------------------------------------
    def parse(self, fileName) :
        '''
        Parse a single file and populate shader lib
        '''
        print '=> parsing {}'.format(fileName)
        f = open(fileName, 'r')
        self.fileName = fileName
        self.lineNumber = 0
        for line in f :
            self.parseLine(line)
            self.lineNumber += 1
        f.close()

#-------------------------------------------------------------------------------
class ShaderLibrary :
    '''
    This represents the entire shader lib.
    '''
    #---------------------------------------------------------------------------
    def __init__(self, xmlTree, absXmlPath) :
        self.xmlRoot = xmlTree.getroot()
        self.xmlPath = absXmlPath
        self.name = None
        self.dirs = []
        self.sources = []
        self.functions = {}
        self.vertexShaders = {}
        self.fragmentShaders = {}
        self.programs = {}
        self.bundles = {}
        self.current = None

    #---------------------------------------------------------------------------
    def parseXmlTree(self) :
        '''
        Parse the root xml file, this sets the name
        and the source directories members.
        '''
        rootDir = os.path.dirname(self.xmlPath)
        print 'ROOT DIR: {}'.format(rootDir)
        self.name = self.xmlRoot.get('name')
        for dir in self.xmlRoot.findall('AddDir') :
            self.dirs.append(rootDir + '/' + dir.get('path'))

    #---------------------------------------------------------------------------
    def gatherSources(self) :
        '''
        This gathers the shader source file names from
        all source directories.
        '''
        for dir in self.dirs :
            self.sources.extend(glob.glob(dir + '/*.shd'))

    #---------------------------------------------------------------------------
    def parseSources(self) :
        '''
        Parse one source file.
        '''
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
    dump(shaderLibrary)

    generateHeader(xmlTree, absHeaderPath)
    generateSource(xmlTree, absSourcePath)
