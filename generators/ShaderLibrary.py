'''
Code generator for shader libraries.
'''

Version = 1

import os
import sys
import glob
from pprint import pprint
import util

#-------------------------------------------------------------------------------
def dumpObj(obj) :
    pprint(vars(obj))

#-------------------------------------------------------------------------------
class Snippet :
    '''
    A snippet from a shader file, can be a function, vertex/fragment shader,
    etc...
    '''
    def __init__(self) :
        self.name = None
        self.path = None
        self.lines = []
        self.replace = []
        self.dependencies = []

    def dump(self) :
        dumpObj(self)

#-------------------------------------------------------------------------------
class Reference :
    '''
    A reference to another function, with information where the 
    ref is located (source, linenumber, start/end indices)
    '''
    def __init__(self, name, path, lineNumber, startIndex, endIndex) :
        self.name = name
        self.path = path
        self.lineNumber = lineNumber
        self.startIndex = startIndex
        self.endIndex = endIndex
        
#-------------------------------------------------------------------------------
class Func(Snippet) :
    '''
    A function snippet
    '''
    def __init__(self, name, path) :
        Snippet.__init__(self)
        self.name = name
        self.path = path
        self.uniforms = []
        self.inputs = []
        self.outputs = []
        self.returnType = None

    def getTag(self) :
        return 'func'

    def dump(self) :
        Snippet.dump(self)
        print 'Uniforms:'
        for uniform in self.uniforms :
            uniform.dump()
        print 'Inputs:'
        for input in self.inputs :
            input.dump()
        print 'Outputs:'
        for output in self.outputs :
            outputs.dump()

#-------------------------------------------------------------------------------
class Uniform :
    '''
    A shader uniform definition.
    '''
    def __init__(self, type, name, bind) :
        self.type = type
        self.name = name
        self.bind = bind

    def dump(self) :
        dumpObj(self)

#-------------------------------------------------------------------------------
class Attr :
    '''
    A shader input or output attribute.
    '''         
    def __init__(self, type, name) :
        self.type = type
        self.name = name

    def dump(self) :
        dumpObj(self)

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
        self.inputs = []
        self.outputs = []
        self.resolvedDeps = []

    def getTag(self) :
        return 'vs' 

    def dump(self) :
        Snippet.dump(self)
        print 'Uniforms:'
        for uniform in self.uniforms :
            uniform.dump()
        print 'Inputs:'
        for input in self.inputs :
            input.dump()
        print 'Outputs:'
        for output in self.outputs :
            output.dump()

#-------------------------------------------------------------------------------
class FragmentShader(Snippet) :
    '''
    A fragment shader function.
    '''
    def __init__(self, name, path) :
        Snippet.__init__(self)
        self.name = name
        self.path = path
        self.uniforms = []
        self.inputs = []
        self.resolvedDeps = []        

    def getTag(self) :
        return 'fs'

    def dump(self) :
        Snippet.dump(self)
        print 'Uniforms:'
        for uniform in self.uniforms :
            uniform.dump()
        print 'Inputs:'
        for input in self.inputs :
            input.dump()

#-------------------------------------------------------------------------------
class Program() :
    '''
    A shader program, made of vertex/fragment shader and uniforms
    '''
    def __init__(self, vs, fs) :
        self.vs = vs
        self.fs = fs
        self.uniforms = []

    def getTag(self) :
        return 'program'

    def dump(self) :
        dumpObj(self)

#-------------------------------------------------------------------------------
class Bundle() :
    '''
    A program bundle (array of vertex/fragment shader tuples, and uniforms)
    '''
    def __init__(self, name, path) :
        self.name = name
        self.path = path
        self.programs = []
        self.uniforms = []

    def getTag(self) :
        return 'bundle'

    def dump(self) :
        dumpObj(self)

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
                        util.fmtError('comment in comment!')
                    else :
                        return ''
                else :
                    comment = line[:endIndex+2]
                    if '/*' in comment or '//' in comment :
                        util.fmtError('comment in comment!')
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
    def replaceTag(self, line, startIndex, endIndex, replace) :
        return line[:startIndex] + replace + line[endIndex+1:]      

    #---------------------------------------------------------------------------
    def checkListDup(self, name, objList) :
        for obj in objList :
            if name == obj.name :
                return True
        return False

    #---------------------------------------------------------------------------
    def newFunc(self, line, kw, args, startIndex, endIndex) :
        print 'newFunc: kw={} args={}'.format(kw, args)
        if len(args) != 1 :
            util.fmtError("func: must have 1 arg (name)")
        name = args[0]
        if name in self.shaderLib.functions :
            util.fmtError("func: function '{}'' already defined in '{}'!".format(name, self.current.name))
        func = Func(name, self.fileName)
        self.shaderLib.functions[name] = func
        self.current = func
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def newVertexShader(self, line, kw, args, startIndex, endIndex) :
        print 'newVertexShader: kw={} args={}'.format(kw, args)
        if len(args) != 1:
            util.fmtError("vs: must have 1 arg (name)")
        name = args[0]
        if name in self.shaderLib.vertexShaders :
            util.fmtError("vs: vertex shader '{}' already defined in '{}'!".format(name, self.current.name))
        vs = VertexShader(name, self.fileName)
        self.shaderLib.vertexShaders[name] = vs
        self.current = vs        
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def newFragmentShader(self, line, kw, args, startIndex, endIndex) :
        print 'newFragmentShader: kw={} args={}'.format(kw, args)
        if len(args) != 1:
            util.fmtError("fs: must have 1 arg (name)")
        name = args[0]
        if name in self.shaderLib.fragmentShaders :
            util.fmtError("fs: fragment shader '{}' already defined in '{}'!".format(name, self.current.name))
        fs = FragmentShader(name, self.fileName)
        self.shaderLib.fragmentShaders[name] = fs
        self.current = fs
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def newBundle(self, line, kw, args, startIndex, endIndex) :
        print 'newBundle: kw={} args={}'.format(kw, args)
        if len(args) != 1:
            util.fmtError("bundle: must have 1 arg (name)")
        name = args[0]
        if name in self.shaderLib.bundles :
            util.fmtError("bundle: bundle '{}' already defined in '{}'!".format(name, self.current.name))
        bundle = Bundle(name, self.fileName)
        self.shaderLib.bundles[name] = bundle
        self.current = bundle            
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addInTag(self, line, kw, args, startIndex, endIndex) :
        print 'addInTag: kw={} args={}'.format(kw, args)
        if not self.current or not self.current.getTag() in ['func', 'vs', 'fs'] :
            util.fmtError("in: must come after a 'func', 'vs' or 'fs' tag!")
        if len(args) != 2:
            util.fmtError("in: must have 2 args (type name)")
        type = args[0]
        name = args[1]
        if self.checkListDup(name, self.current.inputs) :
            util.fmtError("in: input name '{}' already defined in '{}'!".format(name, self.current.name))
        self.current.inputs.append(Attr(type, name))
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addOutTag(self, line, kw, args, startIndex, endIndex) :
        print 'addOutTag: kw={} args={}'.format(kw, args)
        if not self.current or not self.current.getTag() in ['func', 'vs'] :
            util.fmtError("out: must come after a 'func' or 'vs' tag!")
        if len(args) != 2:
            util.fmtError("out: must have 2 args (type name)")
        type = args[0]
        name = args[1]
        if self.checkListDup(name, self.current.outputs) :
            util.fmtError("out: output name '{}' already defined in '{}'!".format(name, self.current.name))
        self.current.outputs.append(Attr(type, name))
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addReturnTag(self, line, kw, args, startIndex, endIndex) :
        print 'addReturnTag: kw={} args={}'.format(kw, args)
        if not self.current or self.current.getTag() != 'func' :
            util.fmtError("return: must come after a 'func' tag!")
        if len(args) != 1:
            util.fmtError("return: must have 1 arg (type)")
        if self.current.returnType != None :
            util.fmtError("return: only one return tag allowed")
        type = args[0]
        self.current.returnType = type            
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addUniformTag(self, line, kw, args, startIndex, endIndex) :
        print 'addUniformTag: kw={} args={}'.format(kw, args)
        if not self.current or not self.current.getTag() in ['func', 'vs', 'fs'] :
            util.fmtError("uniform: must come after a 'func', 'vs' or 'fs' tag!")
        if len(args) != 3:
            util.fmtError("uniform: must have 3 args (type name binding)")
        type = args[0]
        name = args[1]
        bind = args[2]
        if self.checkListDup(name, self.current.uniforms) :
            util.fmtError("uniform: uniform name '{}' already defined in '{}'!".format(name, self.current.name))
        self.current.uniforms.append(Uniform(type, name, bind))
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addProgramTag(self, line, kw, args, startIndex, endIndex) :
        print 'addProgramTag: kw={} args={}'.format(kw, args)
        if not self.current or self.current.getTag() != 'bundle' :
            util.fmtError("program: must come after a 'bundle' tag!")
        if len(args) != 2:
            util.fmtError("program: must have 2 args (vs fs)")
        vs = args[0]
        fs = args[1]
        self.current.programs.append(Program(vs, fs))
        return self.removeTag(line, startIndex, endIndex)

    #---------------------------------------------------------------------------
    def addReplaceTag(self, line, kw, args, startIndex, endIndex) :
        print 'addReplaceTag: kw={} args={}'.format(kw, args)
        if not self.current or not self.current.getTag() in ['func', 'vs', 'fs'] :
            util.fmtError("replacement tag {}: only valid after 'func', 'vs' or 'fs' tag!".format(kw))
        if len(args) != 0:
            util.fmtError("replacement tag {}: can't have args!".format(kw))
        self.current.replace.append(Reference(kw, self.fileName, self.lineNumber, startIndex, endIndex))
        return self.fillTag(line, startIndex, endIndex, 'R')

    #---------------------------------------------------------------------------
    def addDependencyTag(self, line, kw, args, startIndex, endIndex) :
        print 'addDependencyTag: kw={} args={}'.format(kw, args)
        if not self.current or not self.current.getTag() in ['func', 'vs', 'fs'] :
            util.fmtError("dependency tag {}: only valid after 'func', 'vs' or 'fs' tag!".format(kw))
        if len(args) != 0:
            util.fmtError("dependency tag {}: can't have args!".format(kw))
        self.current.dependencies.append(Reference(kw, self.fileName, self.lineNumber, startIndex, endIndex))
        return self.replaceTag(line, startIndex, endIndex, kw)

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
                        line = self.addReplaceTag(line, kw, args, startIndex, endIndex)
                    else :
                        if args :
                            util.fmtError("function call tag '{}' can't have args!".format(kw))
                        line = self.addDependencyTag(line, kw, args, startIndex, endIndex)
                else :
                    util.fmtError('unterminated tag')
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
            util.setErrorLocation(self.fileName, self.lineNumber)
            self.parseLine(line)
            self.lineNumber += 1
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
        self.functions = {}
        self.vertexShaders = {}
        self.fragmentShaders = {}
        self.bundles = {}
        self.current = None

    def dump(self) :
        dumpObj(self)
        print 'Functions:'
        for func in self.functions.values() :
            func.dump()
        print 'Vertex Shaders:'
        for vs in self.vertexShaders.values() :
            vs.dump()
        print 'Fragment Shaders:'
        for fs in self.fragmentShaders.values() :
            fs.dump()
        print 'Bundles:'
        for bundle in self.bundles.values() :
            bundle.dump()

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

    def gatherSources(self) :
        '''
        This gathers the shader source file names from
        all source directories.
        '''
        for dir in self.dirs :
            self.sources.extend(glob.glob(dir + '/*.shd'))

    def parseSources(self) :
        '''
        Parse one source file.
        '''
        parser = Parser(self)
        for source in self.sources :            
            parser.parse(source)

    def resolveDeps(self, shd, dep) :
        '''
        Recursively resolve dependencies for a shader.
        '''
        # just add new dependencies at the end of resolvedDeps,
        # and remove dups in a second pass after recursion
        if not dep.name in self.functions :
            util.setErrorLocation(dep.path, dep.lineNumber)
            util.fmtError("unknown function dependency '{}'".format(dep.name))
        shd.resolvedDeps.append(dep.name)
        for depdep in self.functions[dep.name].dependencies :
            self.resolveDeps(shd, depdep)

    def removeDuplicateDeps(self, shd) :
        '''
        Remove duplicates from the resolvedDeps from the front.
        While we're at it, reverse the order so that the
        lowest level dependency comes first.
        '''
        deps = []
        for dep in shd.resolvedDeps :
            if not dep in deps :
                deps.append(dep)
        deps.reverse()
        shd.resolvedDeps = deps

    def resolveAllDependencies(self) :
        '''
        Resolve function dependencies for vertex- and fragment shaders.
        This populates the resolvedDeps array, with duplicates
        removed, in the right order.
        '''
        for vs in self.vertexShaders.values() :
            for dep in vs.dependencies :
                self.resolveDeps(vs, dep)
        self.removeDuplicateDeps(vs)
        for fs in self.fragmentShaders.values() :
            for dep in fs.dependencies :
                self.resolveDeps(fs, dep)
        self.removeDuplicateDeps(fs)

#-------------------------------------------------------------------------------
def writeHeaderTop(f, shdLib) :
    f.write('#pragma once\n')
    f.write('//-----------------------------------------------------------------------------\n')
    f.write('/*  #version:{}#\n'.format(Version))
    f.write('    machine generated, do not edit!\n')
    f.write('*/\n')
    f.write('namespace Oryol {\n')
    f.write('class ' + shdLib.name + ' {\n')
    f.write('public:\n')

#-------------------------------------------------------------------------------
def writeHeaderBottom(f, shdLib) :
    f.write('};\n')
    f.write('}\n')
    f.write('\n')

#-------------------------------------------------------------------------------
def generateHeader(absHeaderPath, shdLib) :
    f = open(absHeaderPath, 'w')
    writeHeaderTop(f, shdLib)
    for vs in shdLib.vertexShaders.values() :
        f.write('    static const char* {}_src;\n'.format(vs.name))
    for fs in shdLib.fragmentShaders.values() :
        f.write('    static const char* {}_src;\n'.format(fs.name))

    writeHeaderBottom(f, shdLib)
    f.close()

#-------------------------------------------------------------------------------
def writeSourceTop(f, absSourcePath, shdLib) :
    path, hdrFileAndExt = os.path.split(absSourcePath)
    hdrFile, ext = os.path.splitext(hdrFileAndExt)

    f.write('//-----------------------------------------------------------------------------\n')
    f.write('// #version:{}# machine generated, do not edit!\n'.format(Version))
    f.write('//-----------------------------------------------------------------------------\n')
    f.write('#include "Pre.h"\n')
    f.write('#include "' + hdrFile + '.h"\n')
    f.write('\n')
    f.write('namespace Oryol {\n')

#-------------------------------------------------------------------------------
def writeSourceBottom(f, shdLib) :
    f.write('}\n')
    f.write('\n')

#-------------------------------------------------------------------------------
def writeFunctionBody(f, shdLib, lines, lastFunc, glslVersion) :
    for i in range(0, len(lines)) :
        f.write('"{}\\n"'.format(lines[i][1]))
        if lastFunc and i == len(lines)-1 :
            f.write(';\n')
        else :
            f.write('\n')

#-------------------------------------------------------------------------------
def writeFunctionSource(f, shdLib, func, glslVersion) :

    # construct and write function head
    head = '"'
    if func.returnType == None:
        head += 'void '
    else :
        head += '{} '.format(func.returnType)
    head += func.name + '('
    for arg in func.inputs :
        head += 'in {} {},'.format(arg.type, arg.name)
    for arg in func.outputs :
        head += 'out {} {},'.format(arg.type, arg.name)
    head = head[:-1]
    head += ')\\n"\n'
    f.write(head)

    # write function body
    writeFunctionBody(f, shdLib, func.lines, False, glslVersion)

#-------------------------------------------------------------------------------
def writeVertexShaderSource(f, shdLib, vs, glslVersion) :
    f.write('const char* {}::{}_src = \n'.format(shdLib.name, vs.name))

    # write uniforms
    for uniform in vs.uniforms :
        f.write('"uniform {} {}; // {}\\n"\n'.format(uniform.type, uniform.name, uniform.bind))

    # write vertex shader inputs
    for input in vs.inputs :
        if glslVersion < 140 :
            f.write('"attribute {} {};\\n"\n'.format(input.type, input.name))
        else :
            f.write('"in {} {};\\n"\n'.format(input.type, input.name))
    # write vertex shader outputs
    for output in vs.outputs :
        if glslVersion < 140 :
            f.write('"varying {} {};\\n"\n'.format(output.type, output.name))
        else :
            f.write('"out {} {};\\n"\n'.format(output.type, output.name))

    # write functions the vs depends on
    for dep in vs.resolvedDeps :
        writeFunctionSource(f, shdLib, shdLib.functions[dep], glslVersion)

    # write vertex shader function
    f.write('"void main()\\n"\n')
    writeFunctionBody(f, shdLib, vs.lines, True, glslVersion)

#-------------------------------------------------------------------------------
def writeFragmentShaderSource(f, shdLib, fs, glslVersion) :
    f.write('const char* {}::{}_src = \n'.format(shdLib.name, fs.name))

    # write uniforms
    for uniform in fs.uniforms :
        f.write('"uniform {} {};\\n" // {}\n'.format(uniform.type, uniform.name, uniform.bind))

    # write fragment shader inputs
    for input in fs.inputs :
        if glslVersion < 140 :
            f.write('"varying {} {};\\n"\n'.format(input.type, input.name))
        else :
            f.write('"in {} {};\\n"\n'.format(input.type, input.name))

    # write functions the fs depends on
    for dep in fs.resolvedDeps :
        writeFunctionSource(f, shdLib, shdLib.functions[dep], glslVersion)

    # write fragment shader function
    f.write('"void main()\\n"\n')
    writeFunctionBody(f, shdLib, fs.lines, True, glslVersion)

#-------------------------------------------------------------------------------
def generateSource(absSourcePath, shaderLibrary) :
    f = open(absSourcePath, 'w')  
    writeSourceTop(f, absSourcePath, shaderLibrary)
    for vs in shaderLibrary.vertexShaders.values() :
        writeVertexShaderSource(f, shaderLibrary, vs, 100)
    for fs in shaderLibrary.fragmentShaders.values() :
        writeFragmentShaderSource(f, shaderLibrary, fs, 100)
    writeSourceBottom(f, shaderLibrary)  
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
    shaderLibrary.parseSources()
    shaderLibrary.resolveAllDependencies()
    shaderLibrary.dump()

    generateHeader(absHeaderPath, shaderLibrary)
    generateSource(absSourcePath, shaderLibrary)
