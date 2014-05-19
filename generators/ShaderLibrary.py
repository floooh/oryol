'''
Code generator for shader libraries.
'''

Version = 1

import os
import sys
import glob
from pprint import pprint
import util

glslVersions = [ 100, 140 ]

macroKeywords = {
    '$position': '_POSITION',
    '$color': '_COLOR',
    '$texture2D': '_TEXTURE2D',
    '$texture2DProj': '_TEXTURE2DPROJ',
    '$texture2DLod': '_TEXTURE2DLOD',
    '$texture2DProjLod': '_TEXTURE2DPROJLOD',
    '$textureCube': '_TEXTURECUBE',
    '$textureCubeLod': '_TEXTURECUBELOD'
}

glsl100Macros = {
    '_POSITION': 'gl_Position',
    '_COLOR': 'gl_FragColor',
    '_TEXTURE2D': 'texture2D',
    '_TEXTURE2DPROJ': 'texture2DProj',
    '_TEXTURE2DLOD': 'texture2DLod',
    '_TEXTURE2DPROJLOD': 'texture2DProjLod',
    '_TEXTURECUBE': 'textureCube',
    '_TEXTURECUBELOD': 'textureCubeLod'
}

glsl130Macros = {
    '_POSITION': 'gl_Position',
    '_COLOR': '_FragColor', 
    '_TEXTURE2D': 'texture',
    '_TEXTURE2DPROJ': 'textureProj',
    '_TEXTURE2DLOD': 'texture',
    '_TEXTURE2DPROJLOD': 'textureProj',
    '_TEXTURECUBE': 'texture',
    '_TEXTURECUBELOD': 'texture'
}

#-------------------------------------------------------------------------------
def dumpObj(obj) :
    pprint(vars(obj))

#---------------------------------------------------------------------------
def checkListDup(name, objList) :
    for obj in objList :
        if name == obj.name :
            return True
    return False

#-------------------------------------------------------------------------------
def findByName(name, objList) :
    for obj in objList :
        if name == obj.name :
            return obj
    return None

#-------------------------------------------------------------------------------
def getMacroValue(macro, glslVersion) :
    if glslVersion < 130 :
        return glsl100Macros[macro]
    else :
        return glsl130Macros[macro]

#-------------------------------------------------------------------------------
class Snippet :
    '''
    A snippet from a shader file, can be a block, vertex/fragment shader,
    etc...
    '''
    def __init__(self) :
        self.name = None
        self.path = None
        self.lines = []
        self.dependencies = []
        self.macros = []

    def dump(self) :
        dumpObj(self)

#-------------------------------------------------------------------------------
class Reference :
    '''
    A reference to another block, with information where the 
    ref is located (source, linenumber)
    '''
    def __init__(self, name, path, lineNumber) :
        self.name = name
        self.path = path
        self.lineNumber = lineNumber
        
#-------------------------------------------------------------------------------
class Block(Snippet) :
    '''
    A code block snippet.
    '''
    def __init__(self, name, path) :
        Snippet.__init__(self)
        self.name = name
        self.path = path
        self.uniforms = []

    def getTag(self) :
        return 'block'

    def dump(self) :
        Snippet.dump(self)
        print 'Uniforms:'
        for uniform in self.uniforms :
            uniform.dump()

#-------------------------------------------------------------------------------
class Uniform :
    '''
    A shader uniform definition.
    '''
    def __init__(self, type, name, bind, filePath, lineNumber) :
        self.type = type
        self.name = name
        self.bind = bind
        self.filePath = filePath
        self.lineNumber = lineNumber

    def dump(self) :
        dumpObj(self)

#-------------------------------------------------------------------------------
class Attr :
    '''
    A shader input or output attribute.
    '''         
    def __init__(self, type, name, filePath, lineNumber) :
        self.type = type
        self.name = name
        self.filePath = filePath
        self.lineNumber = lineNumber

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
        self.generatedSource = {}

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
        self.generatedSource = {}

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
    def onBlock(self, args) :
        if len(args) != 1 :
            util.fmtError("@block must have 1 arg (name)")
        if self.current is not None :
            util.fmtError("cannot nest @block (missing @end in '{}'?)".format(self.current.name))
        name = args[0]
        if name in self.shaderLib.blocks :
            util.fmtError("@block '{}' already defined".format(name))
        block = Block(name, self.fileName)
        self.shaderLib.blocks[name] = block
        self.current = block

    #---------------------------------------------------------------------------
    def onVertexShader(self, args) :
        if len(args) != 1:
            util.fmtError("@vs must have 1 arg (name)")
        if self.current is not None :
            util.fmtError("cannot nest @vs (missing @end in '{}'?)".format(self.current.name))
        name = args[0]
        if name in self.shaderLib.vertexShaders :
            util.fmtError("@vs '{}' already defined".format(name))
        vs = VertexShader(name, self.fileName)
        self.shaderLib.vertexShaders[name] = vs
        self.current = vs        

    #---------------------------------------------------------------------------
    def onFragmentShader(self, args) :
        if len(args) != 1:
            util.fmtError("@fs must have 1 arg (name)")
        if self.current is not None :
            util.fmtError("cannot nest @fs (missing @end in '{}'?)".format(self.current.name))
        name = args[0]
        if name in self.shaderLib.fragmentShaders :
            util.fmtError("@fs '{}' already defined!".format(name))
        fs = FragmentShader(name, self.fileName)
        self.shaderLib.fragmentShaders[name] = fs
        self.current = fs

    #---------------------------------------------------------------------------
    def onBundle(self, args) :
        if len(args) != 1:
            util.fmtError("@bundle must have 1 arg (name)")
        if self.current is not None :
            util.fmtError("cannot nest @bundle (missing @end tag in '{}'?)".format(self.current.name))
        name = args[0]
        if name in self.shaderLib.bundles :
            util.fmtError("@bundle '{}' already defined!".format(name))
        bundle = Bundle(name, self.fileName)
        self.shaderLib.bundles[name] = bundle
        self.current = bundle            

    #---------------------------------------------------------------------------
    def onIn(self, args) :
        if not self.current or not self.current.getTag() in ['vs', 'fs'] :
            util.fmtError("@in must come after @vs or @fs!")
        if len(args) != 2:
            util.fmtError("@in must have 2 args (type name)")
        type = args[0]
        name = args[1]
        if checkListDup(name, self.current.inputs) :
            util.fmtError("@in '{}' already defined in '{}'!".format(name, self.current.name))
        self.current.inputs.append(Attr(type, name, self.fileName, self.lineNumber))

    #---------------------------------------------------------------------------
    def onOut(self, args) :
        if not self.current or not self.current.getTag() in ['vs'] :
            util.fmtError("@out must come after @vs!")
        if len(args) != 2:
            util.fmtError("@out must have 2 args (type name)")
        type = args[0]
        name = args[1]
        if checkListDup(name, self.current.outputs) :
            util.fmtError("@out '{}' already defined in '{}'!".format(name, self.current.name))
        self.current.outputs.append(Attr(type, name, self.fileName, self.lineNumber))

    #---------------------------------------------------------------------------
    def onUniform(self, args) :
        if not self.current or not self.current.getTag() in ['block', 'vs', 'fs'] :
            util.fmtError("@uniform must come after @block, @vs or @fs tag!")
        if len(args) != 3:
            util.fmtError("@uniform must have 3 args (type name binding)")
        type = args[0]
        name = args[1]
        bind = args[2]
        if checkListDup(name, self.current.uniforms) :
            util.fmtError("@uniform '{}' already defined in '{}'!".format(name, self.current.name))
        self.current.uniforms.append(Uniform(type, name, bind, self.fileName, self.lineNumber))

    #---------------------------------------------------------------------------
    def onProgram(self, args) :
        if not self.current or self.current.getTag() != 'bundle' :
            util.fmtError("@program must come after @bundle!")
        if len(args) != 2:
            util.fmtError("@program must have 2 args (vs fs)")
        vs = args[0]
        fs = args[1]
        self.current.programs.append(Program(vs, fs))

    #---------------------------------------------------------------------------
    def onUse(self, args) :
        if not self.current or not self.current.getTag() in ['block', 'vs', 'fs'] :
            util.fmtError("@use must come after @block, @vs or @fs!")
        if len(args) < 1:
            util.fmtError("@use must have at least one arg!")
        for arg in args :
            self.current.dependencies.append(Reference(arg, self.fileName, self.lineNumber))

    #---------------------------------------------------------------------------
    def onEnd(self, args) :
        if not self.current or not self.current.getTag() in ['block', 'vs', 'fs', 'bundle'] :
            util.fmtError("@end must come after @block, @vs, @fs or @bundle!")
        if len(args) != 0:
            util.fmtError("@end must not have arguments")
        self.current = None

    #---------------------------------------------------------------------------
    def parseTags(self, line) :
        # first check if the line contains a tag, the tag must be
        # alone on the line
        tagStartIndex = line.find('@')
        if tagStartIndex != -1 :
            if tagStartIndex > 0 :
                util.fmtError("only whitespace allowed in front of tag")

            tagAndArgs = line[tagStartIndex+1 :].split()
            tag = tagAndArgs[0]
            args = tagAndArgs[1:]
            if tag == 'block':
                self.onBlock(args)
            elif tag == 'vs':
                self.onVertexShader(args)
            elif tag == 'fs':
                self.onFragmentShader(args)
            elif tag == 'bundle':
                self.onBundle(args)
            elif tag == 'use':
                self.onUse(args)
            elif tag == 'in':
                self.onIn(args)
            elif tag == 'out':
                self.onOut(args)
            elif tag == 'uniform':
                self.onUniform(args)
            elif tag == 'program':
                self.onProgram(args)
            elif tag == 'end':
                self.onEnd(args)
            else :
                util.fmtError("unrecognized @ tag '{}'".format(tag))
            return ''

        # handle any $ macros
        for macro in macroKeywords :
            startIndex = line.find(macro)
            if startIndex != -1 :
                if self.current is not None:
                    line = line.replace(macro, macroKeywords[macro])
                    if macro not in self.current.macros :
                        self.current.macros.append(macroKeywords[macro])
                    else :
                        util.fmtError('$ tag must come after @block, @vs, @fs and before @end')

        # if there are still $ characters in the line, it must be an 
        # unrecognized macro keyword (typo?)
        if '$' in line :
            util.fmtError('unrecognized $ keyword!')

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
                if self.current is not None:
                    self.current.lines.append((self.lineNumber, line))

    #---------------------------------------------------------------------------
    def parseSource(self, fileName) :
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

        # all blocks must be closed
        if self.current is not None :
            util.fmtError('missing @end at end of file')

#-------------------------------------------------------------------------------
class Generator :
    '''
    Generate vertex and fragment shader source code.
    '''
    def __init__(self, shaderLib) :
        self.shaderLib = shaderLib

    #---------------------------------------------------------------------------
    def genLines(self, dstLines, srcLines) :
        for line in srcLines :
            dstLines.append(line[1])
        return dstLines

    #---------------------------------------------------------------------------
    def genVertexShaderSource(self, vs, glslVersion) :
        lines = []

        # version tag
        if glslVersion > 100 :
            lines.append('#version {}'.format(glslVersion))

        # write macros
        for macro in vs.macros :
            lines.append('#define {} {}'.format(macro, getMacroValue(macro, glslVersion)))

        # write uniforms
        for uniform in vs.uniforms :
            lines.append('uniform {} {};'.format(uniform.type, uniform.name, uniform.bind))

        # write vertex shader inputs
        for input in vs.inputs :
            if glslVersion < 130 :
                lines.append('attribute {} {};'.format(input.type, input.name))
            else :
                lines.append('in {} {};'.format(input.type, input.name))
        # write vertex shader outputs
        for output in vs.outputs :
            if glslVersion < 130 :
                lines.append('varying {} {};'.format(output.type, output.name))
            else :
                lines.append('out {} {};'.format(output.type, output.name))

        # write blocks the vs depends on
        for dep in vs.resolvedDeps :
            lines = self.genLines(lines, self.shaderLib.blocks[dep].lines)

        # write vertex shader function
        lines = self.genLines(lines, vs.lines)
        vs.generatedSource[glslVersion] = lines

    #---------------------------------------------------------------------------
    def genFragmentShaderSource(self, fs, glslVersion) :
        lines = []

        # version tag
        if glslVersion > 100 :
            lines.append('#version {}'.format(glslVersion))

        # precision modifiers
        if glslVersion == 100 :
            lines.append('precision mediump float;')

        # write macros
        for macro in fs.macros :
            lines.append('#define {} {}'.format(macro, getMacroValue(macro, glslVersion)))

        # write uniforms
        for uniform in fs.uniforms :
            lines.append('uniform {} {};'.format(uniform.type, uniform.name, uniform.bind))

        # write fragment shader inputs
        for input in fs.inputs :
            if glslVersion < 130 :
                lines.append('varying {} {};'.format(input.type, input.name))
            else :
                lines.append('in {} {};'.format(input.type, input.name))

        # write the fragcolor output
        if glslVersion >= 130 :
            lines.append('out vec4 _FragColor;')

        # write blocks the fs depends on
        for dep in fs.resolvedDeps :
            lines = self.genLines(lines, self.shaderLib.blocks[dep].lines)

        # write fragment shader function
        lines = self.genLines(lines, fs.lines)
        fs.generatedSource[glslVersion] = lines

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
        self.blocks = {}
        self.vertexShaders = {}
        self.fragmentShaders = {}
        self.bundles = {}
        self.current = None

    def dump(self) :
        dumpObj(self)
        print 'Blocks:'
        for block in self.blocks.values() :
            block.dump()
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
            parser.parseSource(source)

    def resolveDeps(self, shd, dep) :
        '''
        Recursively resolve dependencies for a shader.
        '''
        # just add new dependencies at the end of resolvedDeps,
        # and remove dups in a second pass after recursion
        if not dep.name in self.blocks :
            util.setErrorLocation(dep.path, dep.lineNumber)
            util.fmtError("unknown block dependency '{}'".format(dep.name))
        shd.resolvedDeps.append(dep.name)
        for depdep in self.blocks[dep.name].dependencies :
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

    def resolveUniforms(self, shd) :
        '''
        This adds and verifies uniforms from block 
        dependencies to the vertex or fragment shader.
        '''
        for dep in shd.resolvedDeps :            
            for uniform in self.blocks[dep].uniforms :
                # if the uniform already exists, check
                # whether type and binding are matching
                shdUniform = findByName(uniform.name, shd.uniforms)
                if shdUniform is not None:
                    # redundant uniform, check if type and binding name match
                    if shdUniform.type != uniform.type :
                        util.setErrorLocation(uniform.filePath, uniform.lineNumber)
                        util.fmtError("uniform type mismatch '{}' vs '{}'".format(uniform.type, shdUniform.type), False)
                        util.setErrorLocation(shdUniform.filePath, shdUniform.lineNumber)
                        util.fmtError("uniform type mismatch '{}' vs '{}'".format(shdUniform.type, uniform.type))
                    if shdUniform.bind != uniform.bind :
                        util.setErrorLocation(uniform.filePath, uniform.lineNumber)
                        util.fmtError("uniform bind name mismatch '{}' vs '{}'".format(uniform.bind, shdUniform.bind), False)
                        util.setErrorLocation(shdUniform.filePath, shdUniform.lineNumber)
                        util.fmtError("uniform bind name mismatch '{}' vs '{}'".format(shdUniform.bind, uniform.bind))
                else :
                    # new uniform from block, add to shader uniforms
                    shd.uniforms.append(uniform)

    def resolveMacros(self, shd) :
        '''
        Adds any macros used by dependent blocks to the shader.
        '''
        for dep in shd.resolvedDeps :
            for macro in self.blocks[dep].macros :
                if macro not in shd.macros :
                    shd.macros.append(macro)

    def resolveAllDependencies(self) :
        '''
        Resolve block and uniform dependencies for vertex- and fragment shaders.
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
        for vs in self.vertexShaders.values() :
            self.resolveUniforms(vs)
        for fs in self.fragmentShaders.values() :
            self.resolveUniforms(fs)

    def generateShaderSources(self) :
        '''
        This generates the vertex- and fragment-shader source 
        for all GLSL versions.
        '''
        gen = Generator(self)
        for glslVersion in glslVersions :
            for vs in self.vertexShaders.values() :
                gen.genVertexShaderSource(vs, glslVersion)
            for fs in self.fragmentShaders.values() :
                gen.genFragmentShaderSource(fs, glslVersion)

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
    for glslVersion in glslVersions :
        for vs in shdLib.vertexShaders.values() :
            f.write('    static const char* {}_{}_src;\n'.format(vs.name, glslVersion))
        for fs in shdLib.fragmentShaders.values() :
            f.write('    static const char* {}_{}_src;\n'.format(fs.name, glslVersion))

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
def writeVertexShaderSource(f, shdLib, vs, glslVersion) :
    f.write('const char* {}::{}_{}_src = \n'.format(shdLib.name, vs.name, glslVersion))
    for line in vs.generatedSource[glslVersion] :
        f.write('"{}\\n"\n'.format(line))
    f.write(';\n')

#-------------------------------------------------------------------------------
def writeFragmentShaderSource(f, shdLib, fs, glslVersion) :
    f.write('const char* {}::{}_{}_src = \n'.format(shdLib.name, fs.name, glslVersion))
    for line in fs.generatedSource[glslVersion] :
        f.write('"{}\\n"\n'.format(line))
    f.write(';\n')

#-------------------------------------------------------------------------------
def generateSource(absSourcePath, shaderLibrary) :
    f = open(absSourcePath, 'w')  
    writeSourceTop(f, absSourcePath, shaderLibrary)
    for glslVersion in glslVersions :
        for vs in shaderLibrary.vertexShaders.values() :
            writeVertexShaderSource(f, shaderLibrary, vs, glslVersion)
        for fs in shaderLibrary.fragmentShaders.values() :
            writeFragmentShaderSource(f, shaderLibrary, fs, glslVersion)
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
    shaderLibrary.generateShaderSources()

    generateHeader(absHeaderPath, shaderLibrary)
    generateSource(absSourcePath, shaderLibrary)
