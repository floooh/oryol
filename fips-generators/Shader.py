'''
Code generator for shader libraries.
'''

Version = 43

import os
import sys
import glob
import platform
from pprint import pprint
from collections import OrderedDict
import genutil as util
from util import glslcompiler

if platform.system() == 'Windows' :
    from util import hlslcompiler

if platform.system() == 'Darwin' :
    from util import metalcompiler

# SL versions for OpenGLES2.0, OpenGL2.1, OpenGL3.0, D3D11
slVersions = [ 'glsl100', 'glsl120', 'glsl150', 'hlsl5', 'metal' ]

slSlangTypes = {
    'glsl100': 'ShaderLang::GLSL100',
    'glsl120': 'ShaderLang::GLSL120',
    'glsl150': 'ShaderLang::GLSL150',
    'hlsl5':   'ShaderLang::HLSL5',
    'metal':   'ShaderLang::Metal'
}

isGLSL = {
    'glsl100': True,
    'glsl120': True,
    'glsl150': True,
    'hlsl5': False,
    'metal': False
}

isHLSL = {
    'glsl100': False,
    'glsl120': False,
    'glsl150': False,
    'hlsl5': True,
    'metal': False
}

isMetal = {
    'glsl100': False,
    'glsl120': False,
    'glsl150': False,
    'hlsl5': False,
    'metal': True
}

glslVersionNumber = {
    'glsl100': 100,
    'glsl120': 120,
    'glsl150': 150,
    'hlsl5': None,
    'metal': None
}

# declare language-specific mapping macros
slMacros = {
    'glsl100': {
        '_position': 'gl_Position',
        '_color': 'gl_FragColor',
        '_fragcoord': 'gl_FragCoord',
        '_const': 'const',
        '_func': '',
        'mul(m,v)': '(m*v)',
        'tex2D(s, t)': 'texture2D(s,t)',
        'texCUBE(s, t)': 'textureCube(s,t)',
        'tex2Dvs(s, t)': 'texture2D(s,t)'
    },
    'glsl120': {
        '_position': 'gl_Position',
        '_color': 'gl_FragColor',
        '_fragcoord': 'gl_FragCoord',
        '_const': 'const',
        '_func': '',
        'mul(m,v)': '(m*v)',
        'tex2D(s, t)': 'texture2D(s,t)',
        'texCUBE(s, t)': 'textureCube(s,t)',
        'tex2Dvs(s, t)': 'texture2D(s,t)'
    },
    'glsl150': {
        '_position': 'gl_Position',
        '_color': '_FragColor',
        '_fragcoord': 'gl_FragCoord',
        '_const': 'const',
        '_func': '',
        'mul(m,v)': '(m*v)',
        'tex2D(s, t)': 'texture(s,t)',
        'texCUBE(s, t)': 'texture(s,t)',
        'tex2Dvs(s, t)': 'texture(s,t)'
    },
    'hlsl5': {
        '_position': '_oPosition',
        '_color': '_oColor',
        '_const': 'static const',
        '_func': '',
        'vec2': 'float2',
        'vec3': 'float3',
        'vec4': 'float4',
        'mat2': 'float2x2',
        'mat3': 'float3x3',
        'mat4': 'float4x4',
        'tex2D(_obj, _t)': '_obj.t.Sample(_obj.s,_t)',
        'texCUBE(_obj, _t)': '_obj.t.Sample(_obj.s,_t)',
        'tex2Dvs(_obj, _t)': '_obj.t.SampleLevel(_obj.s,_t,0.0)',
        'mix(a,b,c)': 'lerp(a,b,c)',
        'mod(x,y)': '(x-y*floor(x/y))',
        'fract(x)': 'frac(x)'
    },
    'metal': {
        '_position': 'vs_out._vofi_position',
        '_color': '_fo_color',
        '_const': 'constant',
        '_func': 'static',
        'vec2': 'float2',
        'vec3': 'float3',
        'vec4': 'float4',
        'mat2': 'float2x2',
        'mat3': 'float3x3',
        'mat4': 'float4x4',
        'mul(m,v)': '(m*v)',
        'mod(x,y)': '(x-y*floor(x/y))',
        'tex2D(_obj, _t)': '_obj.t.sample(_obj.s,_t)',
        'texCUBE(_obj, _t)': '_obj.t.sample(_obj.s,_t)',
        'tex2Dvs(_obj, _t)': '_obj.t.sample(_obj.s,_t,level(0))',
        'discard': 'discard_fragment()'
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

# NOTE: order is important, always go from greatest to smallest type,
# and keep texture samplers at start!
validUniformTypes = [
    'sampler2D', 'samplerCube',
    'mat4', 'mat3', 'mat2',
    'vec4', 'vec3', 'vec2',
    'float', 'int', 'bool'
]

uniformCType = {
    'bool':         'int32',
    'int':          'int32',
    'float':        'float32',
    'vec2':         'glm::vec2',
    'vec3':         'glm::vec3',
    'vec4':         'glm::vec4',
    'mat2':         'glm::mat2',
    'mat3':         'glm::mat3',
    'mat4':         'glm::mat4',
    'sampler2D':    'Id',
    'samplerCube':  'Id',
}

uniformOryolType = {
    'bool':         'UniformType::Bool',
    'int':          'UniformType::Int',
    'float':        'UniformType::Float',
    'vec2':         'UniformType::Vec2',
    'vec3':         'UniformType::Vec3',
    'vec4':         'UniformType::Vec4',
    'mat2':         'UniformType::Mat2',
    'mat3':         'UniformType::Mat3',
    'mat4':         'UniformType::Mat4',
    'sampler2D':    'UniformType::Texture',
    'samplerCube':  'UniformType::Texture'
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
    def __init__(self, type, name, bindName, filePath, lineNumber) :
        self.type = type
        self.name = name
        self.bindName = bindName
        self.bindSlot = None
        self.filePath = filePath
        self.lineNumber = lineNumber

    def dump(self) :
        dumpObj(self)

#-------------------------------------------------------------------------------
class UniformBlock :
    '''
    A group of related shader uniforms.
    '''
    def __init__(self, name, bindName, shaderStage, filePath, lineNumber) :
        self.name = name
        self.bindName = bindName
        self.bindSlot = None
        self.shaderStage = shaderStage
        self.filePath = filePath
        self.lineNumber = lineNumber
        self.uniforms = []
        self.uniformsByType = OrderedDict()
        # uniformsByType must be in the order of greatest to smallest
        # type, with samplers at the start
        for type in validUniformTypes :
            self.uniformsByType[type] = []

    def getTag(self) :
        return 'uniform_block'
    
    def dump(self) :
        dumpObj(self)
        for uniform in self.uniforms :
            dumpObj(uniform)
        for type in self.uniformsByType :
            for uniform in self.uniformsByType[type] :
                dumpObj(uniform)

    def isEquivalent(self, other) :
        if self.name != other.name :
            return False
        if self.bindName != other.bindName :
            return False
        if self.shaderStage != other.shaderStage :
            return False
        for uniform in self.uniforms :
            other_uniform = findByName(uniform.name, other.uniforms)
            if not other_uniform :
                return False
            else :
                if uniform.type != other_uniform.type :
                    return False
                elif uniform.bindName != other_uniform.bindName :
                    return False
        return True

    def getHash(self) :
        # returns an integer hash for the uniform block layout,
        # this is used as runtime-type check in Gfx::ApplyUniformBlock
        # to check whether a compatible block is set
        hashString = ''
        for type in self.uniformsByType :
            for uniform in self.uniformsByType[type] :
                hashString += type
        return hash(hashString)

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

    def __eq__(self, other) :
        return (self.type == other.type) and (self.name == other.name)

    def __ne__(self, other) :
        return (self.type != other.type) or (self.name != other.name)

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
        self.uniformBlocks = []
        self.inputs = []
        self.outputs = []
        self.resolvedDeps = []
        self.generatedSource = {}

    def getTag(self) :
        return 'vs' 

    def dump(self) :
        Snippet.dump(self)
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
        self.uniformBlocks = []
        self.inputs = []
        self.resolvedDeps = []        
        self.generatedSource = {}

    def getTag(self) :
        return 'fs'

    def dump(self) :
        Snippet.dump(self)
        print 'UniformBlocks:'
        for uniformBlock in self.uniformBlocks :
            uniformBlock.dump()
        print 'Inputs:'
        for input in self.inputs :
            input.dump()

#-------------------------------------------------------------------------------
class Program() :
    '''
    A shader program, made of vertex/fragment shaders
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
        if not self.current or self.current.getTag() != 'uniform_block' :
            util.fmtError("@uniform must come after @uniform_block tag!")
        if len(args) != 3:
            util.fmtError("@uniform must have 3 args (type name binding)")
        type = args[0]
        name = args[1]
        bind = args[2]
        if type not in validUniformTypes :
            util.fmtError("invalid block uniform type '{}', must be one of '{}'!".format(type, ','.join(validUniformTypes)))

        if checkListDup(name, self.current.uniforms) :
            util.fmtError("@uniform '{}' already defined in '{}'!".format(name, self.current.name))
        uniform = Uniform(type, name, bind, self.fileName, self.lineNumber)
        self.current.uniformsByType[type].append(uniform)

    #---------------------------------------------------------------------------
    def onUniformBlock(self, args) :
        if not self.current or not self.current.getTag() in ['vs', 'fs'] :
            util.fmtError("@uniform_block must come after @vs or @fs tag!")
        if len(args) != 2:
            util.fmtError("@uniform_block must have 2 args (name bind)")
        name = args[0]
        bind = args[1]
        shaderStage = self.current.getTag()
        if checkListDup(name, self.current.uniformBlocks) :
            util.fmtError("@uniform_block '{}' already defined in '{}'!".format(name, self.current.name))
        uniformBlock = UniformBlock(name, bind, shaderStage, self.fileName, self.lineNumber)
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
    def genUniformBlocks(self, shd, slVersion, lines) :
        for uBlock in shd.uniformBlocks :
            '''
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
            '''
            for type in uBlock.uniformsByType :
                for uniform in uBlock.uniformsByType[type] :
                    lines.append(Line('uniform {} {};'.format(uniform.type, uniform.name), uniform.filePath, uniform.lineNumber))
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
    def genHeader(self, lines) :
        lines.append(Line('#ifndef ORYOL_HLSL_TYPES'))
        lines.append(Line('#define ORYOL_HLSL_TYPES'))
        lines.append(Line('class _tex2d {'))
        lines.append(Line('    Texture2D t;'))
        lines.append(Line('    SamplerState s;'))
        lines.append(Line('};'))
        lines.append(Line('class _texcube {'))
        lines.append(Line('    TextureCube t;'))
        lines.append(Line('    SamplerState s;'))
        lines.append(Line('};'))
        lines.append(Line('#define sampler2D _tex2d'))
        lines.append(Line('#define samplerCube _texcube'))
        lines.append(Line('#endif'))
        return lines

    #---------------------------------------------------------------------------
    def genUniformBlocks(self, shd, lines) :
        
        # first write texture uniforms outside of cbuffers, and count
        # non-texture uniforms
        for uBlock in shd.uniformBlocks :
            for type in uBlock.uniformsByType :
                for uniform in uBlock.uniformsByType[type] :
                    if type == 'sampler2D' :
                        lines.append(Line('Texture2D _t_{} : register(t{});'.format(uniform.name, uniform.bindSlot), 
                            uniform.filePath, uniform.lineNumber))
                        lines.append(Line('SamplerState _s_{} : register(s{});'.format(uniform.name, uniform.bindSlot), 
                            uniform.filePath, uniform.lineNumber))
                    elif type == 'samplerCube' :
                        lines.append(Line('TextureCube _t_{} : register(t{});'.format(uniform.name, uniform.bindSlot), 
                            uniform.filePath, uniform.lineNumber))
                        lines.append(Line('SamplerState _s_{} : register(s{});'.format(uniform.name, uniform.bindSlot), 
                            uniform.filePath, uniform.lineNumber))

            # if there are non-texture uniforms, groups the rest into a cbuffer
            if uBlock.bindSlot is not None :
                lines.append(Line('cbuffer {} : register(b{}) {{'.format(uBlock.name, uBlock.bindSlot), uBlock.filePath, uBlock.lineNumber))
                for type in uBlock.uniformsByType :
                    if type not in ['sampler2D', 'samplerCube' ] :
                        for uniform in uBlock.uniformsByType[type] :
                            lines.append(Line('  {} {};'.format(uniform.type, uniform.name), uniform.filePath, uniform.lineNumber))
                            # pad vec3's to 16 bytes
                            if type == 'vec3' :
                                lines.append(Line('  float _pad_{};'.format(uniform.name)))
                lines.append(Line('};', uBlock.filePath, uBlock.lineNumber))
        return lines
     
    #---------------------------------------------------------------------------
    def genTexObjects(self, shd, lines) :
        for uBlock in shd.uniformBlocks :
            for type in uBlock.uniformsByType :
                for uniform in uBlock.uniformsByType[type] :
                    if type == 'sampler2D' :
                        lines.append(Line('_tex2d {}; {}.t=_t_{}; {}.s=_s_{};'.format(
                            uniform.name, uniform.name, uniform.name, uniform.name, uniform.name),
                            uniform.filePath, uniform.lineNumber))
                    elif type == 'samplerCube' :
                        lines.append(Line('_texcube {}; {}.t=_t_{}; {}.s=_s_{};'.format(
                            uniform.name, uniform.name, uniform.name, uniform.name, uniform.name),
                            uniform.filePath, uniform.lineNumber))
        return lines

    #---------------------------------------------------------------------------
    def genVertexShaderSource(self, vs, slVersion) :
        lines = []
        
        # write header source
        lines = self.genHeader(lines)

        # write compatibility macros
        for func in slMacros[slVersion] :
            lines.append(Line('#define {} {}'.format(func, slMacros[slVersion][func])))

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
        lines = self.genTexObjects(vs, lines)
        lines = self.genLines(lines, vs.lines)
        lines.append(Line('}', vs.lines[-1].path, vs.lines[-1].lineNumber))
        vs.generatedSource[slVersion] = lines

    #---------------------------------------------------------------------------
    def genFragmentShaderSource(self, fs, slVersion) :
        lines = []

        # write header source
        lines = self.genHeader(lines)

        # write compatibility macros
        for func in slMacros[slVersion] :
            lines.append(Line('#define {} {}'.format(func, slMacros[slVersion][func])))

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
        lines.append(Line('in float4 _fragcoord : SV_Position,'))
        lines.append(Line('out vec4 _oColor : SV_TARGET) {', fs.lines[0].path, fs.lines[0].lineNumber))
        lines = self.genTexObjects(fs, lines)
        lines = self.genLines(lines, fs.lines)
        lines.append(Line('}', fs.lines[-1].path, fs.lines[-1].lineNumber))
        fs.generatedSource[slVersion] = lines

#-------------------------------------------------------------------------------
class MetalGenerator :
    '''
    Generate vertex and fragment shader source code for Metal shader language
    '''
    def __init__(self, shaderLib) :
        self.shaderLib = shaderLib

    #---------------------------------------------------------------------------
    def genLines(self, dstLines, srcLines) :
        for srcLine in srcLines :
            dstLines.append(srcLine)
        return dstLines

    #---------------------------------------------------------------------------
    def genHeader(self, lines) :
        # write general Metal declarations at top of shader
        lines.append(Line('#ifndef _ORYOL_METAL_HEADER'))
        lines.append(Line('#define _ORYOL_METAL_HEADER'))
        lines.append(Line('#include <metal_stdlib>'))
        lines.append(Line('#include <simd/simd.h>'))
        lines.append(Line('using namespace metal;'))
        lines.append(Line('template<typename TYPE> class _tex {'))
        lines.append(Line('public:'))
        lines.append(Line('    _tex(TYPE t_, sampler s_) : t(t_), s(s_) {};'))
        lines.append(Line('    TYPE t;'))
        lines.append(Line('    sampler s;'))
        lines.append(Line('};'))
        lines.append(Line('typedef _tex<texture2d<float,access::sample>> sampler2D;'))
        lines.append(Line('typedef _tex<texturecube<float,access::sample>> samplerCube;'))
        lines.append(Line('#endif'))
        return lines

    #---------------------------------------------------------------------------
    def genUniformBlocks(self, shd, lines) :
        
        # first write texture uniforms outside of cbuffers, and count
        # non-texture uniforms
        uniformDefs = {}
        for uBlock in shd.uniformBlocks :
            # if there are non-texture uniforms, groups the rest into a cbuffer
            if uBlock.bindSlot is not None:
                lines.append(Line('struct {}_{}_t {{'.format(shd.name, uBlock.name), uBlock.filePath, uBlock.lineNumber))
                for type in uBlock.uniformsByType :
                    if type not in ['sampler2D', 'samplerCube' ] :
                        for uniform in uBlock.uniformsByType[type] :
                            lines.append(Line('  {} {};'.format(uniform.type, uniform.name), uniform.filePath, uniform.lineNumber))
                            uniformDefs[uniform.name] = '{}.{}'.format(uBlock.name, uniform.name)
                lines.append(Line('};', uBlock.filePath, uBlock.lineNumber))
        return lines, uniformDefs

    #---------------------------------------------------------------------------
    def genFuncArgs(self, shd, lines) :
        for uBlock in shd.uniformBlocks :
            if uBlock.bindSlot is not None :
                lines.append(Line('constant {}_{}_t& {} [[buffer({})]],'.format(shd.name, uBlock.name, uBlock.name, uBlock.bindSlot)))
            for type in uBlock.uniformsByType :
                for uniform in uBlock.uniformsByType[type] :
                    if type == 'sampler2D' :
                        lines.append(Line('texture2d<float,access::sample> _t_{} [[texture({})]],'.format(uniform.name, uniform.bindSlot), 
                            uniform.filePath, uniform.lineNumber))
                        lines.append(Line('sampler _s_{} [[sampler({})]],'.format(uniform.name, uniform.bindSlot), 
                            uniform.filePath, uniform.lineNumber))
                    elif type == 'samplerCube' :
                        lines.append(Line('texturecube<float,access::sample> _t_{} [[texture({})]],'.format(uniform.name, uniform.bindSlot), 
                            uniform.filePath, uniform.lineNumber))
                        lines.append(Line('sampler _s_{} [[sampler({})]],'.format(uniform.name, uniform.bindSlot), 
                            uniform.filePath, uniform.lineNumber))
        return lines

    #---------------------------------------------------------------------------
    def genTexObjects(self, shd, lines) :
        for uBlock in shd.uniformBlocks :
            for type in uBlock.uniformsByType :
                for uniform in uBlock.uniformsByType[type] :
                    if type == 'sampler2D' :
                        lines.append(Line('sampler2D {}(_t_{},_s_{});'.format(
                            uniform.name, uniform.name, uniform.name),
                            uniform.filePath, uniform.lineNumber))
                    elif type == 'samplerCube' :
                        lines.append(Line('samplerCube {}(_t_{},_s_{});'.format(
                            uniform.name, uniform.name, uniform.name),
                            uniform.filePath, uniform.lineNumber))
        return lines

    #---------------------------------------------------------------------------
    def genVertexShaderSource(self, vs, slVersion) :
        lines = []

        lines = self.genHeader(lines)

        # write compatibility macros
        for func in slMacros[slVersion] :
            lines.append(Line('#define {} {}'.format(func, slMacros[slVersion][func])))

        # write uniform blocks as cbuffers
        lines, uniformDefs = self.genUniformBlocks(vs, lines)

        # write blocks the vs depends on
        # (Metal specific: protect against multiple inclusion)
        for dep in vs.resolvedDeps :
            guard = '_ORYOL_BLOCK_{}'.format(self.shaderLib.blocks[dep].name)
            lines.append(Line('#ifndef {}'.format(guard)))
            lines.append(Line('#define {}'.format(guard)))
            lines = self.genLines(lines, self.shaderLib.blocks[dep].lines)
            lines.append(Line('#endif'))
   
        # write vertex shader input attributes
        vertex_attrs = {
            'position':     0,
            'normal':       1,
            'texcoord0':    2,
            'texcoord1':    3,
            'texcoord2':    4,
            'texcoord3':    5,
            'tangent':      6,
            'binormal':     7,
            'weights':      8,
            'indices':      9,
            'color0':       10,
            'color1':       11,
            'instance0':    12,
            'instance1':    13,
            'instance2':    14,
            'instance3':    15
        }

        # write vertex shader in/out structs
        lines.append(Line('struct {}_vs_in_t {{'.format(vs.name)))
        for input in vs.inputs :
            l = '    {} _vi_{} [[ attribute({}) ]];'.format(input.type, input.name, vertex_attrs[input.name])
            lines.append(Line(l, input.filePath, input.lineNumber))
        lines.append(Line('};'))
        lines.append(Line('struct {}_vs_out_t {{'.format(vs.name)))
        lines.append(Line('    float4 _vofi_position [[position]];'))
        for output in vs.outputs :
            lines.append(Line('    {} _vofi_{};'.format(output.type, output.name), output.filePath, output.lineNumber))
        lines.append(Line('};'))

        # write the main() function
        for input in vs.inputs :
            l = '#define {} vs_in._vi_{}'.format(input.name, input.name)
            lines.append(Line(l, input.filePath, input.lineNumber))
        for output in vs.outputs :
            l = '#define {} vs_out._vofi_{}'.format(output.name, output.name)
            lines.append(Line(l, output.filePath, output.lineNumber))
        for uniformDef in uniformDefs :
            lines.append(Line('#define {} {}'.format(uniformDef, uniformDefs[uniformDef])))

        lines.append(Line('vertex {}_vs_out_t {}('.format(vs.name, vs.name), vs.lines[0].path, vs.lines[0].lineNumber))
        lines = self.genFuncArgs(vs, lines)
        lines.append(Line('{}_vs_in_t vs_in [[stage_in]]) {{'.format(vs.name)))
        lines.append(Line('{}_vs_out_t vs_out;'.format(vs.name)))
        lines = self.genTexObjects(vs, lines)
        lines = self.genLines(lines, vs.lines)
        lines.append(Line('return vs_out;', vs.lines[-1].path, vs.lines[-1].lineNumber))
        lines.append(Line('}', vs.lines[-1].path, vs.lines[-1].lineNumber))
        for input in vs.inputs :
            lines.append(Line('#undef {}'.format(input.name), input.filePath, input.lineNumber))
        for output in vs.outputs :
            lines.append(Line('#undef {}'.format(output.name), output.filePath, output.lineNumber))
        for uniformDef in uniformDefs :
            lines.append(Line('#undef {}'.format(uniformDef)))
        vs.generatedSource[slVersion] = lines

    #---------------------------------------------------------------------------
    def genFragmentShaderSource(self, fs, slVersion) :
        lines = []

        lines = self.genHeader(lines)
        
        # write compatibility macros
        for func in slMacros[slVersion] :
            lines.append(Line('#define {} {}'.format(func, slMacros[slVersion][func])))

        # write uniform blocks as cbuffers
        lines, uniformDefs = self.genUniformBlocks(fs, lines)

        # write blocks the fs depends on
        for dep in fs.resolvedDeps :
            guard = '{}_INCLUDED'.format(self.shaderLib.blocks[dep].name)
            lines.append(Line('#ifndef {}'.format(guard)))
            lines.append(Line('#define {}'.format(guard)))
            lines = self.genLines(lines, self.shaderLib.blocks[dep].lines)
            lines.append(Line('#endif'))

        # write fragment shader input structure
        lines.append(Line('struct {}_fs_in_t {{'.format(fs.name)))
        lines.append(Line('    float4 _vofi_position [[position]];'))
        for input in fs.inputs :
            lines.append(Line('    {} _vofi_{};'.format(input.type, input.name), input.filePath, input.lineNumber))
        lines.append(Line('};'))

        # write the main function
        for input in fs.inputs :
            l = '#define {} fs_in._vofi_{}'.format(input.name, input.name)
            lines.append(Line(l, input.filePath, input.lineNumber))
        for uniformDef in uniformDefs :
            lines.append(Line('#define {} {}'.format(uniformDef, uniformDefs[uniformDef])))
        lines.append(Line('fragment float4 {}('.format(fs.name), fs.lines[0].path, fs.lines[0].lineNumber))
        lines = self.genFuncArgs(fs, lines)
        lines.append(Line('float4 _fragcoord [[position]],'))
        lines.append(Line('{}_fs_in_t fs_in [[stage_in]]) {{'.format(fs.name, fs.name), fs.lines[0].path, fs.lines[0].lineNumber))
        lines.append(Line('float4 _fo_color;'))
        lines = self.genTexObjects(fs, lines)
        lines = self.genLines(lines, fs.lines)
        lines.append(Line('return _fo_color;', fs.lines[-1].path, fs.lines[-1].lineNumber))
        lines.append(Line('}', fs.lines[-1].path, fs.lines[-1].lineNumber))
        for input in fs.inputs :
            lines.append(Line('#undef {}'.format(input.name), input.filePath, input.lineNumber))
        for uniformDef in uniformDefs :
            lines.append(Line('#undef {}'.format(uniformDef)))
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

    def checkAddUniformBlock(self, uniformBlock, list) :
        '''
        Add uniform bundle to list with sanity checks.
        '''
        listUniformBlock = findByName(uniformBlock.name, list)
        if listUniformBlock :
            if not uniformBlock.isEquivalent(listUniformBlock) :
                util.setErrorLocation(uniformBlock.filePath, uniformBlock.lineNumber)
                util.fmtError("uniform blocks named '{}' not equivalent".format(uniformBlock.name), False)
                util.setErrorLocation(listUniformBlock.filePath, listUniformBlock.lineNumber)
                util.fmtError("uniform blocks named '{}' not equivalent".format(uniformBlock.name), False)
        else :
            # new uniform block, add to list
            list.append(uniformBlock)

    def resolveBundleUniformBlocks(self, bundle) :
        '''
        Gathers all uniform blocks from all shaders in the bundle
        '''
        for program in bundle.programs :
            if program.vs not in self.vertexShaders :
                util.setErrorLocation(program.filePath, program.lineNumber)
                util.fmtError("unknown vertex shader '{}'".format(program.vs))
            for uniformBlock in self.vertexShaders[program.vs].uniformBlocks :
                self.checkAddUniformBlock(uniformBlock, bundle.uniformBlocks)

            if program.fs not in self.fragmentShaders :
                util.setErrorLocation(program.filePath, program.lineNumber)
                util.fmtError("unknown fragment shader '{}'".format(program.fs))
            for uniformBlock in self.fragmentShaders[program.fs].uniformBlocks :
                self.checkAddUniformBlock(uniformBlock, bundle.uniformBlocks)

    def assignParamSlotIndices(self, bundle) :
        '''
        Assigns bindSlotIndex to uniform blocks and texture variables
        in all slots, these are used on some platforms for shader parameter
        binding instead of parameter names.
        '''
        uniformBlockSlot = 0
        textureSlot = 0
        for uBlock in bundle.uniformBlocks :
            numBlockUniforms = 0
            for type in uBlock.uniformsByType :
                for uniform in uBlock.uniformsByType[type] :
                    if type in ['sampler2D', 'samplerCube' ] :
                        uniform.bindSlot = textureSlot
                        textureSlot += 1;
                    else :
                        numBlockUniforms += 1

            # if there are non-texture uniforms, groups the rest into a cbuffer
            if numBlockUniforms > 0 :
                uBlock.bindSlot = uniformBlockSlot
                uniformBlockSlot += 1

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
            self.resolveBundleUniformBlocks(bundle)
            self.assignParamSlotIndices(bundle)

    def validate(self) :
        '''
        Runs additional validation check after bundles are resolved and before
        shader code is generated:

        - check whether vertex shader output signatures match fragment
        shader input signatures, this is a D3D11 requirement, signatures 
        must match exactly, even if the fragment shader doesn't use all output
        from the vertex shader
        '''
        for bundle in self.bundles.values() :
            for prog in bundle.programs :
                fatalError = False
                vs = self.vertexShaders[prog.vs]
                fs = self.fragmentShaders[prog.fs]
                if len(vs.outputs) != len(fs.inputs) :
                    if len(fs.inputs) > 0 :
                        util.setErrorLocation(fs.inputs[0].filePath, fs.inputs[0].lineNumber)
                        util.fmtError("number of fs inputs doesn't match number of vs outputs", False)
                        fatalError = True
                    if len(vs.outputs) > 0 :
                        util.setErrorLocation(vs.outputs[0].filePath, vs.outputs[0].lineNumber)
                        util.fmtError("number of vs outputs doesn't match number of fs inputs", False)
                        fatalError = True
                    if fatalError :
                        sys.exit(10)
                else :
                    for index, outAttr in enumerate(vs.outputs) :
                        if outAttr != fs.inputs[index] :
                            util.setErrorLocation(fs.inputs[index].filePath, fs.inputs[index].lineNumber)
                            util.fmtError("fs input doesn't match vs output (names, types and order must match)", False)
                            util.setErrorLocation(outAttr.filePath, outAttr.lineNumber)
                            util.fmtError("vs output doesn't match fs input (names, types and order must match)")

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

    def generateShaderSourcesMetal(self) :
        '''
        Generates vertex- and fragment shader source for Metal
        '''
        gen = MetalGenerator(self)
        for slVersion in slVersions :
            if isMetal[slVersion] :
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
        and let HLSL compiler generate shader byte code in header file
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

    def validateAndWriteShadersMetal(self, absHdrPath) :
        '''
        Bundles all vertex- and fragment-shaders into a single metal source
        file, compiles this into a binary library files which is 
        written into a C header file as embedded byte code.
        '''
        srcLines = []
        rootPath = os.path.splitext(absHdrPath)[0]
        for slVersion in slVersions :
            if isMetal[slVersion] :
                for vs in self.vertexShaders.values() :
                    srcLines.extend(vs.generatedSource[slVersion])
                for fs in self.fragmentShaders.values() :
                    srcLines.extend(fs.generatedSource[slVersion])
            if len(srcLines) > 0 :
                outPath = rootPath + '.h'
                cName = slVersion + '_lib'
                metalcompiler.validate(srcLines, outPath, cName)

#-------------------------------------------------------------------------------
def writeHeaderTop(f, shdLib) :
    f.write('#pragma once\n')
    f.write('//-----------------------------------------------------------------------------\n')
    f.write('/*  #version:{}#\n'.format(Version))
    f.write('    machine generated, do not edit!\n')
    f.write('*/\n')
    f.write('#include "Gfx/Setup/ShaderSetup.h"\n')
    f.write('#include "glm/vec2.hpp"\n')
    f.write('#include "glm/vec3.hpp"\n')
    f.write('#include "glm/vec4.hpp"\n')
    f.write('#include "glm/mat2x2.hpp"\n')
    f.write('#include "glm/mat3x3.hpp"\n')
    f.write('#include "glm/mat4x4.hpp"\n')
    f.write('#include "Resource/Id.h"\n')
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
    
    # write uniform block structs
    for blockIndex, uBlock in enumerate(bundle.uniformBlocks) :
        if uBlock.shaderStage == 'vs' :
            shaderStage = 'ShaderType::VertexShader'
        else :
            shaderStage = 'ShaderType::FragmentShader'
        if uBlock.bindSlot is None :
            bindSlotIndex = "InvalidIndex"
        else :
            bindSlotIndex = uBlock.bindSlot
        f.write('        #pragma pack(push,1)\n')
        f.write('        struct {} {{\n'.format(uBlock.bindName))
        f.write('            static const int32 _uniformBlockIndex = {};\n'.format(blockIndex))
        f.write('            static const int32 _bindSlotIndex = {};\n'.format(bindSlotIndex))
        f.write('            static const ShaderType::Code _bindShaderStage = {};\n'.format(shaderStage))
        f.write('            static const int64 _layoutHash = {};\n'.format(uBlock.getHash()))
        for type in uBlock.uniformsByType :
            for uniform in uBlock.uniformsByType[type] :
                f.write('            {} {};\n'.format(uniformCType[uniform.type], uniform.bindName))
                # for vec3's we need to add a padding field, FIXME: would be good
                # to try filling the padding fields with float params!
                if type == 'vec3' :
                    f.write('            float32 _pad_{};\n'.format(uniform.bindName))
        f.write('        };\n')
        f.write('        #pragma pack(pop)\n')

    f.write('        static ShaderSetup CreateSetup();\n')
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
    f.write('#if ORYOL_D3D11 || ORYOL_D3D12\n')
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
        f.write('#if ORYOL_D3D11 || ORYOL_D3D12\n')
        f.write('/*\n')
        f.write('{}_{}_src: \n\n'.format(shd.name, slVersion))
        for line in shd.generatedSource[slVersion] :
            f.write('{}\n'.format(line.content))
        f.write('*/\n')
        rootPath = os.path.splitext(absPath)[0]
        f.write('#include "{}_{}_{}_src.h"\n'.format(rootPath, shd.name, slVersion))
        f.write('#endif\n')
    elif isMetal[slVersion] :
        # for Metal, the shader has been compiled into a binary shader
        # library file, which needs to be embedded into the C header
        f.write('#if ORYOL_METAL\n')
        f.write('/*\n')
        f.write("Metal source for '{}'\n".format(shd.name))
        for line in shd.generatedSource[slVersion] :
            f.write('{}\n'.format(line.content))
        f.write('*/\n')
        rootPath = os.path.splitext(absPath)[0]
        f.write('#include "{}.metallib.h"\n'.format(rootPath))
        f.write('#endif\n')
    else :
        util.fmtError("Invalid shader language id")

#-------------------------------------------------------------------------------
def writeVertexLayout(f, vs) :
    # writes a C++ VertexLayout definition into the generated source
    # code, this is used to match mesh vertex layouts with 
    # vertex shader input signatures (e.g. required in D3D11),
    # return the C++ name of the vertex layout
    mapAttrName = {
        'position':     'VertexAttr::Position',
        'normal':       'VertexAttr::Normal',
        'texcoord0':    'VertexAttr::TexCoord0',
        'texcoord1':    'VertexAttr::TexCoord1',
        'texcoord2':    'VertexAttr::TexCoord2',
        'texcoord3':    'VertexAttr::TexCoord3',
        'tangent':      'VertexAttr::Tangent',
        'binormal':     'VertexAttr::Binormal',
        'weights':      'VertexAttr::Weights',
        'indices':      'VertexAttr::Indices',
        'color0':       'VertexAttr::Color0',
        'color1':       'VertexAttr::Color1',
        'instance0':    'VertexAttr::Instance0',
        'instance1':    'VertexAttr::Instance1',
        'instance2':    'VertexAttr::Instance2',
        'instance3':    'VertexAttr::Instance3'
    }
    mapAttrType = {
        'float':    'VertexFormat::Float',
        'vec2':     'VertexFormat::Float2',
        'vec3':     'VertexFormat::Float3',
        'vec4':     'VertexFormat::Float4'
    }
    layoutName = '{}_layout'.format(vs.name)
    f.write('    VertexLayout {};\n'.format(layoutName))
    for attr in vs.inputs :
        f.write('    {}.Add({}, {});\n'.format(layoutName, mapAttrName[attr.name], mapAttrType[attr.type]))
    return layoutName

#-------------------------------------------------------------------------------
def writeBundleSource(f, shdLib, bundle) :

    # write the CreateSetup() function
    f.write('ShaderSetup ' + bundle.name + '::CreateSetup() {\n')
    f.write('    ShaderSetup setup("' + bundle.name + '");\n')
    for i, prog in enumerate(bundle.programs) :
        vs = shdLib.vertexShaders[prog.vs]
        fs = shdLib.fragmentShaders[prog.fs]
        vsInputLayout = writeVertexLayout(f, vs)
        vsName = vs.name
        fsName = fs.name
        for slVersion in slVersions :
            slangType = slSlangTypes[slVersion]
            if isGLSL[slVersion] :
                f.write('    #if ORYOL_OPENGL\n')
            elif isHLSL[slVersion] :
                f.write('    #if ORYOL_D3D11 || ORYOL_D3D12\n')
            elif isMetal[slVersion] :
                f.write('    #if ORYOL_METAL\n')
                f.write('    setup.SetLibraryByteCode({}, metal_lib, sizeof(metal_lib));\n'.format(slangType))
            vsSource = '{}_{}_src'.format(vsName, slVersion)
            fsSource = '{}_{}_src'.format(fsName, slVersion)
            if isGLSL[slVersion] :
                f.write('    setup.AddProgramFromSources({}, {}, {}, {}, {});\n'.format(
                    i, slangType, vsInputLayout, vsSource, fsSource));
            elif isHLSL[slVersion] :
                f.write('    setup.AddProgramFromByteCode({}, {}, {}, {}, sizeof({}), {}, sizeof({}));\n'.format(
                    i, slangType, vsInputLayout, vsSource, vsSource, fsSource, fsSource))
            elif isMetal[slVersion] :
                f.write('    setup.AddProgramFromLibrary({}, {}, {}, "{}", "{}");\n'.format(
                    i, slangType, vsInputLayout, vsName, fsName))
            f.write('    #endif\n');
    for uBlock in bundle.uniformBlocks :
        layoutName = '{}_layout'.format(uBlock.bindName)
        f.write('    UniformLayout {};\n'.format(layoutName))
        f.write('    {}.TypeHash = {};\n'.format(layoutName, uBlock.getHash()))
        for type in uBlock.uniformsByType :
            for uniform in uBlock.uniformsByType[type] :
                if uniform.bindSlot is None :
                    bindSlotIndex = 'InvalidIndex'
                else :
                    bindSlotIndex = uniform.bindSlot
                f.write('    {}.Add("{}", {}, 1, {});\n'.format(layoutName, uniform.name, uniformOryolType[uniform.type], bindSlotIndex))
        f.write('    setup.AddUniformBlock("{}", {}, {}::_bindShaderStage, {}::_bindSlotIndex);\n'.format(
            uBlock.name, layoutName, uBlock.bindName, uBlock.bindName))
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
        shaderLibrary.validate()
        shaderLibrary.generateShaderSourcesGLSL()
        shaderLibrary.generateShaderSourcesHLSL()
        shaderLibrary.generateShaderSourcesMetal()
        shaderLibrary.validateShadersGLSL()
        if platform.system() == 'Windows' :
            shaderLibrary.validateAndWriteShadersHLSL(out_hdr)
        if platform.system() == 'Darwin' :
            shaderLibrary.validateAndWriteShadersMetal(out_hdr)
        generateSource(out_src, shaderLibrary)
        generateHeader(out_hdr, shaderLibrary)
