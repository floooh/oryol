'''
Code generator for shader libraries.
'''

Version = 25

import os
import sys
import glob
import platform
from pprint import pprint
import genutil as util
from util import glslcompiler

if platform.system() == 'Windows' :
    from util import hlslcompiler

# SL versions for OpenGLES2.0, OpenGL2.1, OpenGL3.0, D3D11
slVersions = [ 'glsl100', 'glsl120', 'glsl150', 'hlsl5' ]

slSlangTypes = {
    'glsl100': 'ShaderLang::GLSL100',
    'glsl120': 'ShaderLang::GLSL120',
    'glsl150': 'ShaderLang::GLSL150',
    'hlsl5':   'ShaderLang::HLSL5'
}

isGLSL = {
    'glsl100': True,
    'glsl120': True,
    'glsl150': True,
    'hlsl5': False
}

isHLSL = {
    'glsl100': False,
    'glsl120': False,
    'glsl150': False,
    'hlsl5': True
}

glslVersionNumber = {
    'glsl100': 100,
    'glsl120': 120,
    'glsl150': 150,
    'hlsl5': None
}

# map HLSL functions to GLSL versions
slMacros = {
    'glsl100': {
        '_position': 'gl_Position',
        '_color': 'gl_FragColor',
        'static': '',
        'mul(v,m)': '(m * v)',
        'tex2D(s, t)': 'texture2D(s, t)',
        'texCUBE(s, t)': 'textureCube(s, t)',
        'tex2Dvs(s, t)': 'texture2D(s, t)'
    },
    'glsl120': {
        '_position': 'gl_Position',
        '_color': 'gl_FragColor',
        'static': '',
        'mul(v,m)': '(m * v)',
        'tex2D(s, t)': 'texture2D(s, t)',
        'texCUBE(s, t)': 'textureCube(s, t)',
        'tex2Dvs(s, t)': 'texture2D(s, t)'
    },
    'glsl150': {
        '_position': 'gl_Position',
        '_color': '_FragColor',
        'static': '',
        'mul(v,m)': '(m * v)',
        'tex2D(s, t)': 'texture(s, t)',
        'texCUBE(s, t)': 'texture(s, t)',
        'tex2Dvs(s, t)': 'texture(s, t)'
    },
    'hlsl5': {
        '_position': '_oPosition',
        '_color': '_oColor',
        'vec2': 'float2',
        'vec3': 'float3',
        'vec4': 'float4',
        'mat2': 'float2x2',
        'mat3': 'float3x3',
        'mat4': 'float4x4',
        'tex2D(s, t)': 's.Sample(s ## _sampler, t)',
        'texCUBE(s, t)': 's.Sample(s ## _sampler, t)',
        'tex2Dvs(s, t)': 's.Load(int3(t.x, t.y, 0))',
        'mix(a,b,c)': 'lerp(a,b,c)',
        'mod(x,y)': '(x-y*floor(x/y))',
        'fract(x)': 'frac(x)'
    }
}

validVsInNames = [
    'position', 'normal', 'texcoord0', 'texcoord1', 'texcoord2', 'texcoord3',
    'tangent', 'binormal', 'weights', 'indices', 'color0', 'color1',
    'instance0', 'instance1', 'instance2', 'instance3'
]
validInOutTypes = [
    'float', 'vec2', 'vec3', 'vec4'
]

# NOTE: once uniform blocks are fully implemented, only samplers will be valid
# standalone uniform types, all other types must be contained in uniform blocks
validStandaloneUniformTypes = [
    'bool', 'float', 'vec2', 'vec3', 'vec4', 'mat2', 'mat3', 'mat4', 'sampler2D', 'samplerCube'
]

validBlockUniformTypes = [
    'bool', 'float', 'vec2', 'vec3', 'vec4', 'mat2', 'mat3', 'mat4' 
]

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
class Line :
    '''
    A line object with mapping to a source file and line number.
    '''
    def __init__(self, content, path='', lineNumber=0) :
        self.content = content
        self.path = path
        self.lineNumber = lineNumber

#-------------------------------------------------------------------------------
class Snippet :
    '''
    A snippet from a shader file, can be a block, vertex/fragment shader,
    etc...
    '''
    def __init__(self) :
        self.name = None
        self.lines = []
        self.dependencies = []

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
    def __init__(self, name) :
        Snippet.__init__(self)
        self.name = name

    def getTag(self) :
        return 'block'

    def dump(self) :
        Snippet.dump(self)

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
class UniformBlock :
    '''
    A group of related uniforms, these are orthogonal to standalone-uniforms
    (standalone uniforms will very likely go away when uniform blocks are
    fully implemented).
    '''
    def __init__(self, name, bind, filePath, lineNumber) :
        self.name = name
        self.bind = bind
        self.filePath = filePath
        self.lineNumber = lineNumber
        self.uniforms = []
        self.uniformsByType = {}

    def getTag(self) :
        return 'uniform_block'
    
    def dump(self) :
        dumpObj(self)
        for uniform in self.uniforms :
            dumpObj(uniform)
        for type in self.uniformsByType :
            for uniform in self.uniformsByType[type] :
                dumpObj(uniform)

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
    def __init__(self, name) :
        Snippet.__init__(self)
        self.name = name
        self.highPrecision = []
        self.uniforms = []
        self.uniformBlocks = []
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
        print 'UniformBlocks:'
        for uniformBlock in self.uniformBlocks :
            uniformBlock.dump()
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
    def __init__(self, name) :
        Snippet.__init__(self)
        self.name = name
        self.highPrecision = []
        self.uniforms = []
        self.uniformBlocks = []
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
        print 'UniformBlocks:'
        for uniformBlock in self.uniformBlocks :
            uniformBlock.dump()
        print 'Inputs:'
        for input in self.inputs :
            input.dump()

#-------------------------------------------------------------------------------
class Program() :
    '''
    A shader program, made of vertex/fragment shader and uniforms
    '''
    def __init__(self, vs, fs, filePath, lineNumber) :
        self.vs = vs
        self.fs = fs
        self.filePath = filePath
        self.lineNumber = lineNumber        

    def getTag(self) :
        return 'program'

    def dump(self) :
        dumpObj(self)

#-------------------------------------------------------------------------------
class Bundle() :
    '''
    A program bundle (array of vertex/fragment shader tuples, and uniforms)
    '''
    def __init__(self, name) :
        self.name = name
        self.programs = []
        self.uniforms = []
        self.uniformBlocks = []

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
        self.stack = []
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
    def push(self, obj) :
        self.stack.append(self.current)
        self.current = obj

    #---------------------------------------------------------------------------
    def pop(self) :
        self.current = self.stack.pop();

    #---------------------------------------------------------------------------
    def onBlock(self, args) :
        if len(args) != 1 :
            util.fmtError("@block must have 1 arg (name)")
        if self.current is not None :
            util.fmtError("cannot nest @block (missing @end in '{}'?)".format(self.current.name))
        name = args[0]
        if name in self.shaderLib.blocks :
            util.fmtError("@block '{}' already defined".format(name))
        block = Block(name)
        self.shaderLib.blocks[name] = block
        self.push(block)

    #---------------------------------------------------------------------------
    def onVertexShader(self, args) :
        if len(args) != 1:
            util.fmtError("@vs must have 1 arg (name)")
        if self.current is not None :
            util.fmtError("cannot nest @vs (missing @end in '{}'?)".format(self.current.name))
        name = args[0]
        if name in self.shaderLib.vertexShaders :
            util.fmtError("@vs '{}' already defined".format(name))
        vs = VertexShader(name)
        self.shaderLib.vertexShaders[name] = vs
        self.push(vs)        

    #---------------------------------------------------------------------------
    def onFragmentShader(self, args) :
        if len(args) != 1:
            util.fmtError("@fs must have 1 arg (name)")
        if self.current is not None :
            util.fmtError("cannot nest @fs (missing @end in '{}'?)".format(self.current.name))
        name = args[0]
        if name in self.shaderLib.fragmentShaders :
            util.fmtError("@fs '{}' already defined!".format(name))
        fs = FragmentShader(name)
        self.shaderLib.fragmentShaders[name] = fs
        self.push(fs)

    #---------------------------------------------------------------------------
    def onBundle(self, args) :
        if len(args) != 1:
            util.fmtError("@bundle must have 1 arg (name)")
        if self.current is not None :
            util.fmtError("cannot nest @bundle (missing @end tag in '{}'?)".format(self.current.name))
        name = args[0]
        if name in self.shaderLib.bundles :
            util.fmtError("@bundle '{}' already defined!".format(name))
        bundle = Bundle(name)
        self.shaderLib.bundles[name] = bundle
        self.push(bundle)          

    #---------------------------------------------------------------------------
    def onIn(self, args) :
        if not self.current or not self.current.getTag() in ['vs', 'fs'] :
            util.fmtError("@in must come after @vs or @fs!")
        if len(args) != 2:
            util.fmtError("@in must have 2 args (type name)")
        type = args[0]
        name = args[1]
        if type not in validInOutTypes :
            util.fmtError("invalid 'in' type '{}', must be one of '{}'!".format(type, ','.join(validInOutTypes)))
        if self.current.getTag() == 'vs' :
            if name not in validVsInNames :
                util.fmtError("invalid input attribute name '{}', must be one of '{}'!".format(name, ','.join(validVsInNames)))
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
        if type not in validInOutTypes :
            util.fmtError("invalid 'out' type '{}', must be one of '{}'!".format(type, ','.join(validInOutTypes))) 
        if checkListDup(name, self.current.outputs) :
            util.fmtError("@out '{}' already defined in '{}'!".format(name, self.current.name))
        self.current.outputs.append(Attr(type, name, self.fileName, self.lineNumber))

    #---------------------------------------------------------------------------
    def onPrecision(self, args) :
        if not self.current or not self.current.getTag() in ['vs', 'fs'] :
            util.fmtError("@highp must come after @vs or @fs!")
        if len(args) != 1:
            util.fmtError("@highp must have 1 arg (type)")
        type = args[0]
        if checkListDup(type, self.current.highPrecision) :
            util.fmtError("@highp type '{}' already defined in '{}'!".format(type, self.current.name))
        self.current.highPrecision.append(type)

    #---------------------------------------------------------------------------
    def onUniform(self, args) :
        if not self.current or not self.current.getTag() in ['uniform_block', 'vs', 'fs'] :
            util.fmtError("@uniform must come after @uniform_block, @vs or @fs tag!")
        if len(args) != 3:
            util.fmtError("@uniform must have 3 args (type name binding)")
        type = args[0]
        name = args[1]
        bind = args[2]
        if self.current.getTag() == 'uniform_block' :
            if type not in validBlockUniformTypes :
                util.fmtError("invalid block uniform type '{}', must be one of '{}'!".format(type, ','.join(validBlockUniformTypes)))
        else :
            if type not in validStandaloneUniformTypes :
                util.fmtError("unvalid standalone uniform type '{}', must be one of '{}'!".format(type, ','.join(validStandaloneUniformTypes)))

        if checkListDup(name, self.current.uniforms) :
            util.fmtError("@uniform '{}' already defined in '{}'!".format(name, self.current.name))
        uniform = Uniform(type, name, bind, self.fileName, self.lineNumber)
        self.current.uniforms.append(uniform)
        if self.current.getTag() == 'uniform_block' :
            # uniform_blocks group contained uniforms by type
            if not type in self.current.uniformsByType :
                self.current.uniformsByType[type] = []
            self.current.uniformsByType[type].append(uniform)

    #---------------------------------------------------------------------------
    def onUniformBlock(self, args) :
        if not self.current or not self.current.getTag() in ['vs', 'fs'] :
            util.fmtError("@uniform_block must come after @vs or @fs tag!")
        if len(args) != 2:
            util.fmtError("@uniform_block must have 2 args (name bind)")
        name = args[0]
        bind = args[1]
        if checkListDup(name, self.current.uniformBlocks) :
            util.fmtError("@uniform_block '{}' already defined in '{}'!".format(name, self.current.name))
        uniformBlock = UniformBlock(name, bind, self.fileName, self.lineNumber)
        self.current.uniformBlocks.append(uniformBlock)
        self.push(uniformBlock)

    #---------------------------------------------------------------------------
    def onProgram(self, args) :
        if not self.current or self.current.getTag() != 'bundle' :
            util.fmtError("@program must come after @bundle!")
        if len(args) != 2:
            util.fmtError("@program must have 2 args (vs fs)")
        vs = args[0]
        fs = args[1]
        self.current.programs.append(Program(vs, fs, self.fileName, self.lineNumber))

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
        if not self.current or not self.current.getTag() in ['uniform_block', 'block', 'vs', 'fs', 'bundle'] :
            util.fmtError("@end must come after @uniform_block, @block, @vs, @fs or @bundle!")
        if len(args) != 0:
            util.fmtError("@end must not have arguments")
        if self.current.getTag() in ['block', 'vs', 'fs'] and len(self.current.lines) == 0 :
            util.fmtError("no source code lines in @block, @vs or @fs section")
        self.pop()

    #---------------------------------------------------------------------------
    def parseTags(self, line) :
        # first check if the line contains a tag, the tag must be
        # alone on the line
        tagStartIndex = line.find('@')
        if tagStartIndex != -1 :
            if tagStartIndex > 0 :
                util.fmtError("only whitespace allowed in front of tag")
            if line.find(';') != -1 :
                util.fmtError("no semicolons allowed in tag lines")

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
            elif tag == 'uniform_block':
                self.onUniformBlock(args)
            elif tag == 'highp' :
                self.onPrecision(args)
            elif tag == 'program':
                self.onProgram(args)
            elif tag == 'end':
                self.onEnd(args)
            else :
                util.fmtError("unrecognized @ tag '{}'".format(tag))
            return ''

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
                    self.current.lines.append(Line(line, self.fileName, self.lineNumber))

    #---------------------------------------------------------------------------
    def parseSource(self, fileName) :
        '''
        Parse a single file and populate shader lib
        '''
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
class GLSLGenerator :
    '''
    Generate vertex and fragment shader source code for GLSL dialects
    '''
    def __init__(self, shaderLib) :
        self.shaderLib = shaderLib

    #---------------------------------------------------------------------------
    def genLines(self, dstLines, srcLines) :
        for srcLine in srcLines :
            dstLines.append(srcLine)
        return dstLines

    #---------------------------------------------------------------------------
    def genUniforms(self, shd, lines) :
        # NOTE: in the future, only texture samplers will be standaline uniforms
        for uniform in shd.uniforms :
            lines.append(Line('uniform {} {};'.format(uniform.type, uniform.name), uniform.filePath, uniform.lineNumber))
        return lines

    #---------------------------------------------------------------------------
    def genUniformBlocks(self, shd, slVersion, lines) :
        for uBlock in shd.uniformBlocks :
            if glslVersionNumber[slVersion] >= 150 :
                # on GLSL 1.50 and above, write actual uniform blocks
                lines.append(Line('layout(std140) uniform {} {{'.format(uBlock.name), uBlock.filePath, uBlock.lineNumber))
                for type in uBlock.uniformsByType :
                    for uniform in uBlock.uniformsByType[type] :
                        lines.append(Line('  {} {};'.format(uniform.type, uniform.name), uniform.filePath, uniform.lineNumber))
                lines.append(Line('};', uBlock.filePath, uBlock.lineNumber))
            else :
                # on old-school GLSL, pack the uniform block members into arrays by type
                for type in uBlock.uniformsByType :
                    name = '{}_{}'.format(uBlock.name, type)
                    size = len(uBlock.uniformsByType[type])
                    lines.append(Line('uniform {} {}[{}];'.format(type, name, size), uBlock.filePath, uBlock.lineNumber)) 
        return lines 

    #---------------------------------------------------------------------------
    def genVertexShaderSource(self, vs, slVersion) :
        lines = []

        # version tag
        if glslVersionNumber[slVersion] > 100 :
            lines.append(Line('#version {}'.format(glslVersionNumber[slVersion])))

        # write compatibility macros
        for func in slMacros[slVersion] :
            lines.append(Line('#define {} {}'.format(func, slMacros[slVersion][func])))

        # precision modifiers 
        # (NOTE: GLSL spec says that GL_FRAGMENT_PRECISION_HIGH is also avl. in vertex language)
        if slVersion == 'glsl100' :
            if vs.highPrecision :
                lines.append(Line('#ifdef GL_FRAGMENT_PRECISION_HIGH'))
                for type in vs.highPrecision :
                    lines.append(Line('precision highp {};'.format(type)))
                lines.append(Line('#endif'))

        # write uniforms
        lines = self.genUniforms(vs, lines)

        # write uniform blocks 
        lines = self.genUniformBlocks(vs, slVersion, lines)

        # write vertex shader inputs
        for input in vs.inputs :
            if glslVersionNumber[slVersion] < 130 :
                lines.append(Line('attribute {} {};'.format(input.type, input.name), input.filePath, input.lineNumber))
            else :
                lines.append(Line('in {} {};'.format(input.type, input.name), input.filePath, input.lineNumber))

        # write vertex shader outputs
        for output in vs.outputs :
            if glslVersionNumber[slVersion] < 130 :
                lines.append(Line('varying {} {};'.format(output.type, output.name), output.filePath, output.lineNumber))
            else :
                lines.append(Line('out {} {};'.format(output.type, output.name), output.filePath, output.lineNumber))

        # write blocks the vs depends on
        for dep in vs.resolvedDeps :
            lines = self.genLines(lines, self.shaderLib.blocks[dep].lines)

        # write vertex shader function
        lines.append(Line('void main() {', vs.lines[0].path, vs.lines[0].lineNumber))
        lines = self.genLines(lines, vs.lines)
        lines.append(Line('}', vs.lines[-1].path, vs.lines[-1].lineNumber))
        vs.generatedSource[slVersion] = lines

    #---------------------------------------------------------------------------
    def genFragmentShaderSource(self, fs, slVersion) :
        lines = []

        # version tag
        if glslVersionNumber[slVersion] > 100 :
            lines.append(Line('#version {}'.format(glslVersionNumber[slVersion])))

        # precision modifiers
        if slVersion == 'glsl100' :
            lines.append(Line('precision mediump float;'))
            if fs.highPrecision :
                lines.append(Line('#ifdef GL_FRAGMENT_PRECISION_HIGH'))
                for type in fs.highPrecision :
                    lines.append(Line('precision highp {};'.format(type)))
                lines.append(Line('#endif'))

        # write compatibility macros
        for func in slMacros[slVersion] :
            lines.append(Line('#define {} {}'.format(func, slMacros[slVersion][func])))

        # write uniforms
        lines = self.genUniforms(fs, lines)

        # write uniform blocks (only in glsl150)
        lines = self.genUniformBlocks(fs, slVersion, lines)

        # write fragment shader inputs
        for input in fs.inputs :
            if glslVersionNumber[slVersion] < 130 :
                lines.append(Line('varying {} {};'.format(input.type, input.name), input.filePath, input.lineNumber))
            else :
                lines.append(Line('in {} {};'.format(input.type, input.name), input.filePath, input.lineNumber))

        # write the fragcolor output
        if glslVersionNumber[slVersion] >= 130 :
            lines.append(Line('out vec4 _FragColor;'))

        # write blocks the fs depends on
        for dep in fs.resolvedDeps :
            lines = self.genLines(lines, self.shaderLib.blocks[dep].lines)

        # write fragment shader function
        lines.append(Line('void main() {', fs.lines[0].path, fs.lines[0].lineNumber))
        lines = self.genLines(lines, fs.lines)
        lines.append(Line('}', fs.lines[-1].path, fs.lines[-1].lineNumber))
        fs.generatedSource[slVersion] = lines

#-------------------------------------------------------------------------------
class HLSLGenerator :
    '''
    Generate vertex and fragment shader source code for HLSL dialects
    '''
    def __init__(self, shaderLib) :
        self.shaderLib = shaderLib

    #---------------------------------------------------------------------------
    def genLines(self, dstLines, srcLines) :
        for srcLine in srcLines :
            dstLines.append(srcLine)
        return dstLines

    #---------------------------------------------------------------------------
    def genUniforms(self, shd, lines) :
        # only samplers and textures can be standalone uniforms in HLSL
        for uniform in shd.uniforms :
            if uniform.type == 'sampler2D' :
                lines.append(Line('Texture2D {};'.format(uniform.name), uniform.filePath, uniform.lineNumber))
                lines.append(Line('SamplerState {}_sampler;'.format(uniform.name), uniform.filePath, uniform.lineNumber))
            elif uniform.type == 'samplerCube' :
                lines.append(Line('TextureCube {};'.format(uniform.name), uniform.filePath, uniform.lineNumber))
                lines.append(Line('SamplerState {}_sampler;'.format(uniform.name), uniform.filePath, uniform.lineNumber))
            else :
                # FIXME: remove when uniform buffers are fully implemented
                lines.append(Line('{} {};'.format(uniform.type, uniform.name), uniform.filePath, uniform.lineNumber))
        return lines

    #---------------------------------------------------------------------------
    def genUniformBlocks(self, shd, lines) :
        for uBlock in shd.uniformBlocks :
            lines.append(Line('cbuffer {} {{'.format(uBlock.name), uBlock.filePath, uBlock.lineNumber))
            for type in uBlock.uniformsByType :
                for uniform in uBlock.uniformsByType[type] :
                    lines.append(Line('  {} {};'.format(uniform.type, uniform.name), uniform.filePath, uniform.lineNumber))
            lines.append(Line('};', uBlock.filePath, uBlock.lineNumber))
        return lines 
    
    #---------------------------------------------------------------------------
    def genVertexShaderSource(self, vs, slVersion) :
        lines = []
        
        # write compatibility macros
        for func in slMacros[slVersion] :
            lines.append(Line('#define {} {}'.format(func, slMacros[slVersion][func])))

        # write standalone uniforms (should only be texture samplers)
        lines = self.genUniforms(vs, lines)

        # write uniform blocks as cbuffers
        lines = self.genUniformBlocks(vs, lines)

        # write blocks the vs depends on
        for dep in vs.resolvedDeps :
            lines = self.genLines(lines, self.shaderLib.blocks[dep].lines)
    
        # write the main() function
        lines.append(Line('void main(', vs.lines[0].path, vs.lines[0].lineNumber))
        for input in vs.inputs :
            l = 'in {} {} : {},'.format(input.type, input.name, input.name)
            lines.append(Line(l, input.filePath, input.lineNumber))
        for output in vs.outputs :
            l = 'out {} {} : {},'.format(output.type, output.name, output.name)
            lines.append(Line(l, output.filePath, output.lineNumber))
        lines.append(Line('out vec4 _oPosition : SV_POSITION) {', vs.lines[0].path, vs.lines[0].lineNumber))
        lines = self.genLines(lines, vs.lines)
        lines.append(Line('}', vs.lines[-1].path, vs.lines[-1].lineNumber))
        vs.generatedSource[slVersion] = lines

    #---------------------------------------------------------------------------
    def genFragmentShaderSource(self, fs, slVersion) :
        lines = []

        # write compatibility macros
        for func in slMacros[slVersion] :
            lines.append(Line('#define {} {}'.format(func, slMacros[slVersion][func])))

        # write standalone uniforms (should only be texture samplers)
        lines = self.genUniforms(fs, lines)

        # write uniform blocks as cbuffers
        lines = self.genUniformBlocks(fs, lines)

        # write blocks the fs depends on
        for dep in fs.resolvedDeps :
            lines = self.genLines(lines, self.shaderLib.blocks[dep].lines)
        
        # write the main function
        lines.append(Line('void main(', fs.lines[0].path, fs.lines[0].lineNumber))
        for input in fs.inputs :
            l = 'in {} {} : {},'.format(input.type, input.name, input.name)
            lines.append(Line(l, input.filePath, input.lineNumber))
        lines.append(Line('out vec4 _oColor : SV_TARGET) {', fs.lines[0].path, fs.lines[0].lineNumber))
        lines = self.genLines(lines, fs.lines)
        lines.append(Line('}', fs.lines[-1].path, fs.lines[-1].lineNumber))
        fs.generatedSource[slVersion] = lines

#-------------------------------------------------------------------------------
class ShaderLibrary :
    '''
    This represents the entire shader lib.
    '''
    def __init__(self, inputs) :
        self.sources = inputs
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

    def checkAddUniform(self, uniform, list) :
        '''
        Check if uniform already exists in list, if yes
        check if type and binding matches, if not write error.
        If uniform doesn't exist yet in list, add it.
        '''
        listUniform = findByName(uniform.name, list)
        if listUniform is not None:
            # redundant uniform, check if type and binding name match
            if listUniform.type != uniform.type :
                util.setErrorLocation(uniform.filePath, uniform.lineNumber)
                util.fmtError("uniform type mismatch '{}' vs '{}'".format(uniform.type, listUniform.type), False)
                util.setErrorLocation(listUniform.filePath, listUniform.lineNumber)
                util.fmtError("uniform type mismatch '{}' vs '{}'".format(listUniform.type, uniform.type))
            if listUniform.bind != uniform.bind :
                util.setErrorLocation(uniform.filePath, uniform.lineNumber)
                util.fmtError("uniform bind name mismatch '{}' vs '{}'".format(uniform.bind, listUniform.bind), False)
                util.setErrorLocation(listUniform.filePath, listUniform.lineNumber)
                util.fmtError("uniform bind name mismatch '{}' vs '{}'".format(listUniform.bind, uniform.bind))
        else :
            # new uniform from block, add to list
            list.append(uniform)


    def resolveBundleUniforms(self, bundle) :
        '''
        Gathers all uniforms from all shaders in the bundle.
        '''
        for program in bundle.programs :
            if program.vs not in self.vertexShaders :
                util.setErrorLocation(program.filePath, program.lineNumber)
                util.fmtError("unknown vertex shader '{}'".format(program.vs))
            for uniform in self.vertexShaders[program.vs].uniforms :
                self.checkAddUniform(uniform, bundle.uniforms)

            if program.fs not in self.fragmentShaders :
                util.setErrorLocation(program.filePath, program.lineNumber)
                util.fmtError("unknown fragment shader '{}'".format(program.fs))
            for uniform in self.fragmentShaders[program.fs].uniforms :
                self.checkAddUniform(uniform, bundle.uniforms)

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
        for bundle in self.bundles.values() :
            self.resolveBundleUniforms(bundle)

    def generateShaderSourcesGLSL(self) :
        '''
        This generates the vertex- and fragment-shader source 
        for all GLSL versions.
        '''
        gen = GLSLGenerator(self)
        for slVersion in slVersions :
            if isGLSL[slVersion] :
                for vs in self.vertexShaders.values() :
                    gen.genVertexShaderSource(vs, slVersion)
                for fs in self.fragmentShaders.values() :
                    gen.genFragmentShaderSource(fs, slVersion)

    def generateShaderSourcesHLSL(self) :
        '''
        Generates vertex- and fragment-shader source for HLSL
        '''
        gen = HLSLGenerator(self)
        for slVersion in slVersions :
            if isHLSL[slVersion] :
                for vs in self.vertexShaders.values() :
                    gen.genVertexShaderSource(vs, slVersion)
                for fs in self.fragmentShaders.values() :
                    gen.genFragmentShaderSource(fs, slVersion)

    def validateShadersGLSL(self) :
        '''
        Run the shader sources through the GLSL reference compiler
        '''
        for slVersion in slVersions :
            if isGLSL[slVersion] :
                for vs in self.vertexShaders.values() :
                    srcLines = vs.generatedSource[slVersion]
                    glslcompiler.validate(srcLines, 'vs', slVersion)
                for fs in self.fragmentShaders.values() :
                    srcLines = fs.generatedSource[slVersion]
                    glslcompiler.validate(srcLines, 'fs', slVersion)

    def validateAndWriteShadersHLSL(self, absHdrPath) :
        '''
        Run the shader sources through the HLSL compiler,
        and let HLSL generate shader byte code in header file
        '''
        rootPath = os.path.splitext(absHdrPath)[0]
        for slVersion in slVersions :
            if isHLSL[slVersion] :
                for vs in self.vertexShaders.values() :
                    srcLines = vs.generatedSource[slVersion]
                    cName = vs.name + '_' + slVersion + '_src'
                    outPath = rootPath + '_' + cName + '.h'
                    hlslcompiler.validate(srcLines, 'vs', slVersion, outPath, cName)
                for fs in self.fragmentShaders.values() :
                    srcLines = fs.generatedSource[slVersion]
                    cName = fs.name + '_' + slVersion + '_src'
                    outPath = rootPath + '_' + cName + '.h'
                    hlslcompiler.validate(srcLines, 'fs', slVersion, outPath, cName)

#-------------------------------------------------------------------------------
def writeHeaderTop(f, shdLib) :
    f.write('#pragma once\n')
    f.write('//-----------------------------------------------------------------------------\n')
    f.write('/*  #version:{}#\n'.format(Version))
    f.write('    machine generated, do not edit!\n')
    f.write('*/\n')
    f.write('#include "Gfx/Setup/ProgramBundleSetup.h"\n')
    f.write('namespace Oryol {\n')
    f.write('namespace Shaders {\n')

#-------------------------------------------------------------------------------
def writeHeaderBottom(f, shdLib) :
    f.write('}\n')
    f.write('}\n')
    f.write('\n')

#-------------------------------------------------------------------------------
def writeBundleHeader(f, shdLib, bundle) :
    f.write('    class ' + bundle.name + ' {\n')
    f.write('    public:\n')
    for i in range(0, len(bundle.uniforms)) :
        f.write('        static const int32 {} = {};\n'.format(bundle.uniforms[i].bind, i))
    f.write('        static ProgramBundleSetup CreateSetup();\n')
    f.write('    };\n')

#-------------------------------------------------------------------------------
def generateHeader(absHeaderPath, shdLib) :
    f = open(absHeaderPath, 'w')
    writeHeaderTop(f, shdLib)
    for bundle in shdLib.bundles.values() :
        writeBundleHeader(f, shdLib, bundle)
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
    f.write('namespace Shaders {\n')
    f.write('#if ORYOL_D3D11\n')
    f.write('typedef unsigned char BYTE;\n')
    f.write('#endif\n')

#-------------------------------------------------------------------------------
def writeSourceBottom(f, shdLib) :
    f.write('}\n')
    f.write('}\n')
    f.write('\n')

#-------------------------------------------------------------------------------
def writeShaderSource(f, absPath, shdLib, shd, slVersion) :
    # note: shd is either a VertexShader or FragmentShader object
    if isGLSL[slVersion] :
        # GLSL source code is directly inlined for runtime-compilation
        f.write('#if ORYOL_OPENGL\n')
        f.write('const char* {}_{}_src = \n'.format(shd.name, slVersion))
        for line in shd.generatedSource[slVersion] :
            f.write('"{}\\n"\n'.format(line.content))
        f.write(';\n')
        f.write('#endif\n')
    elif isHLSL[slVersion] :
        # for HLSL, the actual shader code has been compiled into a header by FXC
        # also write the generated shader source into a C comment as
        # human-readable version
        f.write('#if ORYOL_D3D11\n')
        f.write('/*\n')
        f.write('{}_{}_src: \n\n'.format(shd.name, slVersion))
        for line in shd.generatedSource[slVersion] :
            f.write('{}\n'.format(line.content))
        f.write('*/\n')
        rootPath = os.path.splitext(absPath)[0]
        f.write('#include "{}_{}_{}_src.h"\n'.format(rootPath, shd.name, slVersion))
        f.write('#endif\n')
    else :
        util.fmtError("Invalid shader language id")

#-------------------------------------------------------------------------------
def writeBundleSource(f, shdLib, bundle) :
    f.write('ProgramBundleSetup ' + bundle.name + '::CreateSetup() {\n')
    f.write('    ProgramBundleSetup setup("' + bundle.name + '");\n')
    for i in range(0, len(bundle.programs)) :
        vsName = shdLib.vertexShaders[bundle.programs[i].vs].name
        fsName = shdLib.fragmentShaders[bundle.programs[i].fs].name
        for slVersion in slVersions :
            if isGLSL[slVersion] :
                f.write('    #if ORYOL_OPENGL\n')
            elif isHLSL[slVersion] :
                f.write('    #if ORYOL_D3D11\n')
            slangType = slSlangTypes[slVersion]
            vsSource = '{}_{}_src'.format(vsName, slVersion)
            fsSource = '{}_{}_src'.format(fsName, slVersion)
            if isGLSL[slVersion] :
                f.write('    setup.AddProgramFromSources({}, {}, {}, {});\n'.format(i, slangType, vsSource, fsSource));
            elif isHLSL[slVersion] :
                f.write('    setup.AddProgramFromByteCode({}, {}, {}, sizeof({}), {}, sizeof({}));\n'.format(
                    i, slangType, vsSource, vsSource, fsSource, fsSource))
            f.write('    #endif\n');
    for uniform in bundle.uniforms :
        if uniform.type.startswith('sampler') :
            f.write('    setup.AddTextureUniform("{}", {});\n'.format(uniform.name, uniform.bind))
        else :
            f.write('    setup.AddUniform("{}", {});\n'.format(uniform.name, uniform.bind))
    f.write('    return setup;\n')
    f.write('}\n')

#-------------------------------------------------------------------------------
def generateSource(absSourcePath, shdLib) :
    f = open(absSourcePath, 'w') 
    writeSourceTop(f, absSourcePath, shdLib)
    for slVersion in slVersions :
        for vs in shdLib.vertexShaders.values() :
            writeShaderSource(f, absSourcePath, shdLib, vs, slVersion)
        for fs in shdLib.fragmentShaders.values() :
            writeShaderSource(f, absSourcePath, shdLib, fs, slVersion)

    for bundle in shdLib.bundles.values() :
        writeBundleSource(f, shdLib, bundle)            
    writeSourceBottom(f, shdLib)  
    
    f.close()

#-------------------------------------------------------------------------------
def generate(input, out_src, out_hdr) :
    if util.isDirty(Version, [input], [out_src, out_hdr]) :
        shaderLibrary = ShaderLibrary([input])
        shaderLibrary.parseSources()
        shaderLibrary.resolveAllDependencies()
        shaderLibrary.generateShaderSourcesGLSL()
        shaderLibrary.generateShaderSourcesHLSL()
        shaderLibrary.validateShadersGLSL()
        if platform.system() == 'Windows' :
            shaderLibrary.validateAndWriteShadersHLSL(out_hdr)
        generateSource(out_src, shaderLibrary)
        generateHeader(out_hdr, shaderLibrary)
