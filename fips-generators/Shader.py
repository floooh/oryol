'''
Code generator for shader libraries.
'''

Version = 81

import os
import sys
import glob
import re
import platform
from pprint import pprint
from collections import OrderedDict
import genutil as util
from util import glslcompiler
import zlib # only for crc32

if platform.system() == 'Windows' :
    from util import hlslcompiler

if platform.system() == 'Darwin' :
    from util import metalcompiler

# SL versions for OpenGLES2.0, OpenGL2.1, OpenGL3.0, D3D11
slVersions = [ 'glsl100', 'glsl120', 'glsl330', 'glsles3', 'hlsl5', 'metal' ]

slSlangTypes = {
    'glsl100': 'ShaderLang::GLSL100',
    'glsl120': 'ShaderLang::GLSL120',
    'glsl330': 'ShaderLang::GLSL330',
    'glsles3': 'ShaderLang::GLSLES3',
    'hlsl5':   'ShaderLang::HLSL5',
    'metal':   'ShaderLang::Metal'
}

isGLSL = {
    'glsl100': True,
    'glsl120': True,
    'glsl330': True,
    'glsles3': True,
    'hlsl5': False,
    'metal': False
}

isHLSL = {
    'glsl100': False,
    'glsl120': False,
    'glsl330': False,
    'glsles3': False,
    'hlsl5': True,
    'metal': False
}

isMetal = {
    'glsl100': False,
    'glsl120': False,
    'glsl330': False,
    'glsles3': False,
    'hlsl5': False,
    'metal': True
}

glslVersionNumber = {
    'glsl100': 100,
    'glsl120': 120,
    'glsl330': 330,
    'glsles3': 300,
    'hlsl5': None,
    'metal': None
}

# declare language-specific mapping macros
slMacros = {
    'glsl100': {
        'ORYOL_GLSL': '(1)',
        'ORYOL_HLSL': '(0)',
        'ORYOL_METALSL': '(0)',
        'ORYOL_GLSL_VERSION': '(100)',
        '_vertexid': '(0)',
        '_instanceid': '(0)',
        '_position': 'gl_Position',
        '_pointsize': 'gl_PointSize',
        '_color': 'gl_FragColor',
        '_color1': 'gl_FragColor',
        '_color2': 'gl_FragColor',
        '_color3': 'gl_FragColor',
        '_fragcoord': 'gl_FragCoord',
        '_const': 'const',
        '_func': '',
        'sampler3D': 'sampler2D',       # hack to hide invalid sampler types
        'sampler2DArray': 'sampler2D',  # hack to hide invalid sampler types
        'mul(m,v)': '(m*v)',
        'tex2D(s, t)': 'texture2D(s,t)',
        'tex3D(s, t)': 'vec4(0.0)',
        'tex2DArray(s, t)': 'vec4(0.0)',
        'texCUBE(s, t)': 'textureCube(s,t)',
        'tex2Dvs(s, t)': 'texture2D(s,t)',
        'tex3Dvs(s, t)': 'vec4(0.0)',
        'tex2DArrayvs(s, t)': 'vec4(0.0)',
    },
    'glsl120': {
        'ORYOL_GLSL': '(1)',
        'ORYOL_HLSL': '(0)',
        'ORYOL_METALSL': '(0)',
        'ORYOL_GLSL_VERSION': '(120)',
        '_vertexid': 'gl_VertexID',
        '_instanceid': 'gl_InstanceID',
        '_position': 'gl_Position',
        '_pointsize': 'gl_PointSize',
        '_color': 'gl_FragColor',
        '_color1': 'gl_FragColor',
        '_color2': 'gl_FragColor',
        '_color3': 'gl_FragColor',
        '_fragcoord': 'gl_FragCoord',
        '_const': 'const',
        '_func': '',
        'sampler3D': 'sampler2D',       # hack to hide invalid sampler types
        'sampler2DArray': 'sampler2D',  # hack to hide invalid sampler types
        'mul(m,v)': '(m*v)',
        'tex2D(s, t)': 'texture2D(s,t)',
        'tex3D(s, t)': 'vec4(0.0)',
        'tex2DArray(s, t)': 'vec4(0.0)',
        'texCUBE(s, t)': 'textureCube(s,t)',
        'tex2Dvs(s, t)': 'texture2D(s,t)',
        'tex3Dvs(s, t)': 'vec4(0.0)',
        'tex2DArrayvs(s, t)': 'vec4(0.0)',
    },
    'glsl330': {
        'ORYOL_GLSL': '(1)',
        'ORYOL_HLSL': '(0)',
        'ORYOL_METALSL': '(0)',
        'ORYOL_GLSL_VERSION': '(330)',
        '_vertexid': 'gl_VertexID',
        '_instanceid': 'gl_InstanceID',
        '_position': 'gl_Position',
        '_pointsize': 'gl_PointSize',
        '_color': '_FragColor',
        '_color1': '_FragColor1',
        '_color2': '_FragColor2',
        '_color3': '_FragColor3',
        '_fragcoord': 'gl_FragCoord',
        '_const': 'const',
        '_func': '',
        'mul(m,v)': '(m*v)',
        'tex2D(s, t)': 'texture(s,t)',
        'tex3D(s, t)': 'texture(s,t)',
        'tex2DArray(s, t)': 'texture(s,t)',
        'texCUBE(s, t)': 'texture(s,t)',
        'tex2Dvs(s, t)': 'texture(s,t)',
        'tex3Dvs(s, t)': 'texture(s,t)',
        'tex2DArrayvs(s, t)': 'texture(s,t)',
    },
    'glsles3': {
        'ORYOL_GLSL': '(1)',
        'ORYOL_HLSL': '(0)',
        'ORYOL_METALSL': '(0)',
        'ORYOL_GLSL_VERSION': '(300)',
        '_vertexid': 'gl_VertexID',
        '_instanceid': 'gl_InstanceID',
        '_position': 'gl_Position',
        '_pointsize': 'gl_PointSize',
        '_color': '_FragColor',
        '_color1': '_FragColor1',
        '_color2': '_FragColor2',
        '_color3': '_FragColor3',
        '_fragcoord': 'gl_FragCoord',
        '_const': 'const',
        '_func': '',
        'mul(m,v)': '(m*v)',
        'tex2D(s, t)': 'texture(s,t)',
        'tex3D(s, t)': 'texture(s,t)',
        'tex2DArray(s, t)': 'texture(s,t)',
        'texCUBE(s, t)': 'texture(s,t)',
        'tex2Dvs(s, t)': 'texture(s,t)',
        'tex3Dvs(s, t)': 'texture(s,t)',
        'tex2DArrayvs(s, t)': 'texture(s,t)',
    },
    'hlsl5': {
        'ORYOL_GLSL': '(0)',
        'ORYOL_HLSL': '(1)',
        'ORYOL_METALSL': '(0)',
        '_vertexid': '_iVertexID',
        '_instanceid': '_iInstanceID',
        '_position': '_oPosition',
        '_pointsize': '_oPointSize',
        '_color': '_oColor',
        '_color1': '_oColor1',
        '_color2': '_oColor2',
        '_color3': '_oColor3',
        '_const': 'static const',
        '_func': '',
        'vec2': 'float2',
        'vec3': 'float3',
        'vec4': 'float4',
        'mat2': 'float2x2',
        'mat3': 'float3x3',
        'mat4': 'float4x4',
        'tex2D(_obj, _t)': '_obj.t.Sample(_obj.s,_t)',
        'tex3D(_obj, _t)': '_obj.t.Sample(_obj.s,_t)',
        'tex2DArray(_obj, _t)': '_obj.t.Sample(_obj.s,_t)',
        'texCUBE(_obj, _t)': '_obj.t.Sample(_obj.s,_t)',
        'tex2Dvs(_obj, _t)': '_obj.t.SampleLevel(_obj.s,_t,0.0)',
        'tex3Dvs(_obj, _t)': '_obj.t.SampleLevel(_obj.s,_t,0.0)',
        'tex2DArrayvs(_obj, _t)': '_obj.t.SampleLevel(_obj.s,_t,0.0)',
        'mix(a,b,c)': 'lerp(a,b,c)',
        'mod(x,y)': '(x-y*floor(x/y))',
        'fract(x)': 'frac(x)'
    },
    'metal': {
        'ORYOL_GLSL': '(0)',
        'ORYOL_HLSL': '(0)',
        'ORYOL_METALSL': '(1)',
        '_vertexid': 'vs_vertexid',
        '_instanceid': 'vs_instanceid',
        '_position': 'vs_out._vofi_position',
        '_pointsize': 'vs_out._vofi_pointsize',
        '_color': '_fo_color.color0',
        '_color0': '_fo_color.color0',
        '_color1': '_fo_color.color1',
        '_color2': '_fo_color.color2',
        '_color3': '_fo_color.color3',
        '_const': 'constant',
        '_func': 'static',
        'bool': 'int',
        'vec2': 'float2',
        'vec3': 'float3',
        'vec4': 'float4',
        'mat2': 'float2x2',
        'mat3': 'float3x3',
        'mat4': 'float4x4',
        'mul(m,v)': '(m*v)',
        'mod(x,y)': '(x-y*floor(x/y))',
        'tex2D(_obj, _t)': '_obj.t.sample(_obj.s,_t)',
        'tex2DArray(_obj, _t)': '_obj.t.sample(_obj.s,_t.xy,(uint)_t.z)',
        'texCUBE(_obj, _t)': '_obj.t.sample(_obj.s,_t)',
        'tex3D(_obj, _t)': '_obj.t.sample(_obj.s,_t)',
        'tex2Dvs(_obj, _t)': '_obj.t.sample(_obj.s,_t,level(0))',
        'tex3Dvs(_obj, _t)': '_obj.t.sample(_obj.s,_t,level(0))',
        'tex2DArrayvs(_obj, _t)': '_obj.t.sample(_obj.s,_t.xy,(uint)_t.z,level(0))',
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
    'mat4', 'mat3', 'mat2',
    'vec4', 'vec3', 'vec2',
    'float', 'int', 'bool'
]
validUniformArrayTypes = [
    'mat4', 'vec4'
]

uniformCType = {
    'bool':         'int',
    'int':          'int',
    'float':        'float',
    'vec2':         'glm::vec2',
    'vec3':         'glm::vec3',
    'vec4':         'glm::vec4',
    'mat2':         'glm::mat2',
    'mat3':         'glm::mat3',
    'mat4':         'glm::mat4',
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
}

attrOryolType = {
    'float':    'VertexFormat::Float',
    'vec2':     'VertexFormat::Float2',
    'vec3':     'VertexFormat::Float3',
    'vec4':     'VertexFormat::Float4'
}

attrOryolName = {
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

validTextureTypes = [
    'sampler2D', 'samplerCube', 'sampler3D', 'sampler2DArray'
]

texOryolType = {
    'sampler2D':        'TextureType::Texture2D',
    'samplerCube':      'TextureType::TextureCube',
    'sampler3D':        'TextureType::Texture3D',
    'sampler2DArray':   'TextureType::TextureArray',
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
    A snippet from a shader file, can be a code block, vertex/fragment shader,
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
    A reference to another named object, with information where the 
    ref is located (source, linenumber)
    '''
    def __init__(self, name, path, lineNumber) :
        self.name = name
        self.path = path
        self.lineNumber = lineNumber
        
#-------------------------------------------------------------------------------
class CodeBlock(Snippet) :
    '''
    A code block snippet.
    '''
    def __init__(self, name) :
        Snippet.__init__(self)
        self.name = name

    def getTag(self) :
        return 'code_block'

    def dump(self) :
        Snippet.dump(self)

#-------------------------------------------------------------------------------
class Uniform :
    '''
    A shader uniform definition.
    '''
    def __init__(self, type, num, name, bindName, filePath, lineNumber) :
        self.type = type
        self.name = name
        self.bindName = bindName
        self.filePath = filePath
        self.lineNumber = lineNumber
        self.num = num      # >1 if array

    def dump(self) :
        dumpObj(self)

#-------------------------------------------------------------------------------
class UniformBlock :
    '''
    A group of related shader uniforms.
    '''
    def __init__(self, name, bindName, filePath, lineNumber) :
        self.name = name
        self.bindName = bindName
        self.bindStage = None
        self.bindSlot = None
        self.filePath = filePath
        self.lineNumber = lineNumber
        self.lines = []
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

    def parseUniformType(self, arg) :
        return arg.split('[')[0]

    def parseUniformArraySize(self, arg) :
        tokens = arg.split('[')
        if len(tokens) > 1 :
            return int(tokens[1].strip(']'))
        else : 
            return 1

    def parseUniforms(self) :
        # parses the lines array into uniform objects
        for line in self.lines :
            util.setErrorLocation(line.path, line.lineNumber)
            tokens = line.content.split()
            if len(tokens) != 3:
                util.fmtError("uniform must have 3 args (type name binding)")
            type = self.parseUniformType(tokens[0])
            num  = self.parseUniformArraySize(tokens[0])
            name = tokens[1]
            bind = tokens[2]
            if type not in validUniformTypes :
                util.fmtError("invalid uniform type '{}', must be one of '{}'!".format(type, ','.join(validUniformTypes)))
            # additional type restrictions for uniform array types (because of alignment rules)
            if num > 1 and type not in validUniformArrayTypes :
                util.fmtError("invalid uniform array type '{}', must be '{}'!".format(type, ','.join(validUniformArrayTypes)))
            if checkListDup(name, self.uniforms) :
                util.fmtError("uniform '{}' already defined in '{}'!".format(name, self.name))
            uniform = Uniform(type, num, name, bind, line.path, line.lineNumber)
            self.uniforms.append(uniform)
            self.uniformsByType[type].append(uniform)

    def getHash(self) :
        # returns an integer hash for the uniform block layout,
        # this is used as runtime-type check in Gfx::ApplyUniformBlock
        # to check whether a compatible block is set
        hashString = ''
        for type in self.uniformsByType :
            for uniform in self.uniformsByType[type] :
                hashString += type
                hashString += str(uniform.num)
        return zlib.crc32(hashString) & 0xFFFFFFFF

#-------------------------------------------------------------------------------
class Texture :
    '''
    A texture shader parameter
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
class TextureBlock :
    '''
    A group of related textures.
    '''
    def __init__(self, name, bindName, filePath, lineNumber) :
        self.name = name
        self.bindName = bindName
        self.bindStage = None
        self.filePath = filePath
        self.lineNumber = lineNumber
        self.lines = []
        self.textures = []

    def getTag(self) :
        return 'texture_block'
    
    def dump(self) :
        dumpObj(self)
        for tex in self.textures :
            dumpObj(tex)

    def parseTextures(self) :
        # parses the lines array into uniform objects
        for line in self.lines :
            util.setErrorLocation(line.path, line.lineNumber)
            tokens = line.content.split()
            if len(tokens) != 3:
                util.fmtError("texture must have 3 args (type name binding)")
            type = tokens[0]
            name = tokens[1]
            bind = tokens[2]
            if type not in validTextureTypes :
                util.fmtError("invalid texture type '{}, must be one of '{}'!".format(type, ','.join(validTextureTypes)))
            if checkListDup(name, self.textures) :
                util.fmtError("texture '{}' already defined in '{}'!".format(name, self.name))
            tex = Texture(type, name, bind, line.path, line.lineNumber)
            self.textures.append(tex)

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
class Shader(Snippet) :
    '''
    Generic shader base class.
    '''
    def __init__(self, name) :
        Snippet.__init__(self)
        self.name = name
        self.highPrecision = []
        self.uniformBlockRefs = []
        self.uniformBlocks = []
        self.textureBlockRefs = []
        self.textureBlocks = []
        self.inputs = []
        self.outputs = []
        self.resolvedDeps = []
        self.generatedSource = {}
        self.hasPointSize = False
        self.hasVertexId = False
        self.hasInstanceId = False
        self.hasColor1 = False
        self.hasColor2 = False
        self.hasColor3 = False
        self.hasFragCoord = False

    def dump(self) :
        Snippet.dump(self)
        print('UniformBlockRefs:')
        for uniformBlockRef in self.uniformBlockRefs :
            uniformBlockRef.dump()
        print('UniformBlocks:')
        for uniformBlock in self.uniformBlocks :
            uniformBlock.dump()
        print('TextureBlockRefs:')
        for textureBlockRef in self.textureBlockRefs :
            textureBlockRef.dump()
        print('TextureBlocks:')
        for textureBlock in self.textureBlocks :
            textureBlock.dump()
        print('Inputs:')
        for input in self.inputs :
            input.dump()
        print('Outputs:')
        for output in self.outputs :
            output.dump()

#-------------------------------------------------------------------------------
class VertexShader(Shader) :
    '''
    A vertex shader function.
    '''
    def __init__(self, name) :
        Shader.__init__(self, name)

    def getTag(self) :
        return 'vs' 

#-------------------------------------------------------------------------------
class FragmentShader(Shader) :
    '''
    A fragment shader function.
    '''
    def __init__(self, name) :
        Shader.__init__(self, name)

    def getTag(self) :
        return 'fs'

#-------------------------------------------------------------------------------
class Program() :
    '''
    A shader program, made of vertex/fragment shaders
    '''
    def __init__(self, name, vs, fs, filePath, lineNumber) :
        self.name = name
        self.vs = vs
        self.fs = fs
        self.uniformBlocks = []
        self.textureBlocks = []
        self.filePath = filePath
        self.lineNumber = lineNumber        

    def getTag(self) :
        return 'program'

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
        rx_str = '[\s\w,;=+-/%()*{}\[\]]*'
        self.regexPointSize = re.compile('^{}_pointsize{}$'.format(rx_str, rx_str))
        self.regexVertexId = re.compile('^{}_vertexid{}$'.format(rx_str, rx_str))
        self.regexInstanceId = re.compile('^{}_instanceid{}$'.format(rx_str, rx_str))
        self.regexColor1 = re.compile('^{}_color1{}$'.format(rx_str, rx_str))
        self.regexColor2 = re.compile('^{}_color2{}$'.format(rx_str, rx_str))
        self.regexColor3 = re.compile('^{}_color3{}$'.format(rx_str, rx_str))
        self.regexFragCoord = re.compile('^{}_fragcoord{}$'.format(rx_str, rx_str))

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
    def onCodeBlock(self, args) :
        if len(args) != 1 :
            util.fmtError("@code_block must have 1 arg (name)")
        if self.current is not None :
            util.fmtError("@code_block must be at top level (missing @end in '{}'?)".format(self.current.name))
        name = args[0]
        if name in self.shaderLib.codeBlocks :
            util.fmtError("@code_block '{}' already defined".format(name))
        codeBlock = CodeBlock(name)
        self.shaderLib.codeBlocks[name] = codeBlock
        self.push(codeBlock)

    #---------------------------------------------------------------------------
    def onUniformBlock(self, args) :
        if self.current is not None :
            util.fmtError("@uniform_block must be at top level (missing @end in '{}'?".format(self.current.name))
        if len(args) != 2:
            util.fmtError("@uniform_block must have 2 args (name bind)")
        name = args[0]
        bind = args[1]
        if name in self.shaderLib.uniformBlocks  :
            util.fmtError("@uniform_block '{}' already defined.".format(name))
        ub = UniformBlock(name, bind, self.fileName, self.lineNumber)
        self.shaderLib.uniformBlocks[name] = ub
        self.push(ub)

    #---------------------------------------------------------------------------
    def onTextureBlock(self, args) :
        if self.current is not None :
            util.fmtError("@texture_block must be at top level (missing @end in '{}'?".format(self.current.name))
        if len(args) != 2:
            util.fmtError("@texture_block must have 2 args (name bind)")
        name = args[0]
        bind = args[1]
        if name in self.shaderLib.textureBlocks :
            util.fmtError("@texture_block '{}' already defined.".format(name))
        tb = TextureBlock(name, bind, self.fileName, self.lineNumber)
        self.shaderLib.textureBlocks[name] = tb
        self.push(tb)

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
    def onProgram(self, args) :        
        if len(args) != 3:
            util.fmtError("@program must have 3 args (name vs fs)")
        if self.current is not None :
            util.fmtError("cannot nest @program (missing @end tag in '{}'?)".format(self.current.name))
        name = args[0]
        vs = args[1]
        fs = args[2]
        prog = Program(name, vs, fs, self.fileName, self.lineNumber)
        self.shaderLib.programs[name] = prog

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
        self.current.highPrecision.append(type)

    #---------------------------------------------------------------------------
    def onUseCodeBlock(self, args) :
        if not self.current or not self.current.getTag() in ['code_block', 'vs', 'fs'] :
            util.fmtError("@use_code_block must come after @code_block, @vs or @fs!")
        if len(args) < 1:
            util.fmtError("@use_code_block must have at least one arg!")
        for arg in args :
            self.current.dependencies.append(Reference(arg, self.fileName, self.lineNumber))

    #---------------------------------------------------------------------------
    def onUseUniformBlock(self, args) :
        if not self.current or not self.current.getTag() in ['vs', 'fs'] :
            util.fmtError("@use_uniform_block must come after @vs or @fs!")
        if len(args) < 1:
            util.fmtError("@use_uniform_block must have at least one arg!")
        for arg in args :
            if arg not in self.shaderLib.uniformBlocks :
                util.fmtError("unknown uniform_block name '{}'".format(arg))
            uniformBlock = self.shaderLib.uniformBlocks[arg]
            if uniformBlock.bindStage is not None :
                if uniformBlock.bindStage != self.current.getTag() :
                    util.fmtError("uniform_block '{}' cannot be used both in @vs and @fs!".format(arg))
            uniformBlock.bindStage = self.current.getTag()
            self.current.uniformBlockRefs.append(Reference(arg, self.fileName, self.lineNumber))
            self.current.uniformBlocks.append(uniformBlock)
   
    #---------------------------------------------------------------------------
    def onUseTextureBlock(self, args) :
        if not self.current or not self.current.getTag() in ['vs', 'fs'] :
            util.fmtError("@use_texture_block must come after @vs or @fs!")
        if len(args) < 1 :
            util.fmtError("@use_texture_block must have at least one arg!")
        for arg in args :
            if arg not in self.shaderLib.textureBlocks :
                util.fmtError("unknown texture_block name '{}'".format(arg))
            textureBlock = self.shaderLib.textureBlocks[arg]
            if textureBlock.bindStage is not None :
                if textureBlock.bindStage != self.current.getTag() :
                    util.fmtError("texture_block '{}' cannot be used both in @vs and @fs!".format(arg))
            textureBlock.bindStage = self.current.getTag()
            self.current.textureBlockRefs.append(Reference(arg, self.fileName, self.lineNumber))
            self.current.textureBlocks.append(textureBlock)

    #---------------------------------------------------------------------------
    def onEnd(self, args) :
        if not self.current or not self.current.getTag() in ['uniform_block', 'texture_block', 'code_block', 'vs', 'fs', 'program'] :
            util.fmtError("@end must come after @uniform_block, @texture_block, @code_block, @vs, @fs or @program!")
        if len(args) != 0:
            util.fmtError("@end must not have arguments")
        if self.current.getTag() in ['code_block', 'vs', 'fs'] and len(self.current.lines) == 0 :
            util.fmtError("no source code lines in @code_block, @vs or @fs section")
        if self.current.getTag() == 'uniform_block' :
            self.current.parseUniforms()
        if self.current.getTag() == 'texture_block' :
            self.current.parseTextures()
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
            if tag == 'code_block':
                self.onCodeBlock(args)
            elif tag == 'vs':
                self.onVertexShader(args)
            elif tag == 'fs':
                self.onFragmentShader(args)
            elif tag == 'use_code_block':
                self.onUseCodeBlock(args)
            elif tag == 'use_uniform_block':
                self.onUseUniformBlock(args)
            elif tag == 'use_texture_block':
                self.onUseTextureBlock(args)
            elif tag == 'in':
                self.onIn(args)
            elif tag == 'out':
                self.onOut(args)
            elif tag == 'uniform_block':
                self.onUniformBlock(args)
            elif tag == 'texture_block':
                self.onTextureBlock(args)
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
    def parseSpecialKeyword(self, line) :
        '''
        Checks for special keywords in line, and set internal flags.
        '''
        if self.current is not None :
            if self.regexPointSize.match(line) :
                self.current.hasPointSize = True
            if self.regexVertexId.match(line) :
                self.current.hasVertexId = True
            if self.regexInstanceId.match(line) :
                self.current.hasInstanceId = True
            if self.regexColor1.match(line) :
                self.current.hasColor1 = True
            if self.regexColor2.match(line) :
                self.current.hasColor2 = True
            if self.regexColor3.match(line) :
                self.current.hasColor3 = True
            if self.regexFragCoord.match(line) :
                self.current.hasFragCoord = True

    #---------------------------------------------------------------------------
    def parseLine(self, line) :
        '''
        Parse a single line.
        '''
        line = self.stripComments(line)
        if line != '':
            line = self.parseTags(line)
            self.parseSpecialKeyword(line)
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
    def genUniforms(self, shd, slVersion, lines) :
        # no GLSL uniform blocks
        for ub in shd.uniformBlocks :
            for type in ub.uniformsByType :
                for uniform in ub.uniformsByType[type] :
                    if uniform.num == 1 :
                        lines.append(Line('uniform {} {};'.format(uniform.type, uniform.name), 
                            uniform.filePath, uniform.lineNumber))
                    else :
                        lines.append(Line('uniform {} {}[{}];'.format(uniform.type, uniform.name, uniform.num), 
                            uniform.filePath, uniform.lineNumber))
        for tb in shd.textureBlocks :
            for tex in tb.textures :
                lines.append(Line('uniform {} {};'.format(tex.type, tex.name), tex.filePath, tex.lineNumber))
        return lines 

    #---------------------------------------------------------------------------
    def genVertexShaderSource(self, vs, slVersion) :
        lines = []

        # version tag
        if slVersion == 'glsles3' :
            lines.append(Line('#version 300 es'))
        elif glslVersionNumber[slVersion] > 100 :
            lines.append(Line('#version {}'.format(glslVersionNumber[slVersion])))

        # write compatibility macros
        for macro in slMacros[slVersion] :
            lines.append(Line('#define {} {}'.format(macro, slMacros[slVersion][macro])))

        # precision modifiers 
        # (NOTE: GLSL spec says that GL_FRAGMENT_PRECISION_HIGH is also avl. in vertex language)
        if slVersion == 'glsl100' or slVersion == 'glsles3' :
            if vs.highPrecision :
                if slVersion == 'glsl100' :
                    lines.append(Line('#ifdef GL_FRAGMENT_PRECISION_HIGH'))
                for type in vs.highPrecision :
                    lines.append(Line('precision highp {};'.format(type)))
                if slVersion == 'glsl100' :
                    lines.append(Line('#endif'))

        # write uniform definition 
        lines = self.genUniforms(vs, slVersion, lines)

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
            lines = self.genLines(lines, self.shaderLib.codeBlocks[dep].lines)

        # write vertex shader function
        lines.append(Line('void main() {', vs.lines[0].path, vs.lines[0].lineNumber))
        lines = self.genLines(lines, vs.lines)
        lines.append(Line('}', vs.lines[-1].path, vs.lines[-1].lineNumber))
        vs.generatedSource[slVersion] = lines

    #---------------------------------------------------------------------------
    def genFragmentShaderSource(self, fs, slVersion) :
        lines = []

        # version tag
        if slVersion == 'glsles3' :
            lines.append(Line('#version 300 es'))
        elif glslVersionNumber[slVersion] > 100 :
            lines.append(Line('#version {}'.format(glslVersionNumber[slVersion])))

        # write compatibility macros
        for func in slMacros[slVersion] :
            lines.append(Line('#define {} {}'.format(func, slMacros[slVersion][func])))

        # precision modifiers
        if slVersion == 'glsl100' or slVersion == 'glsles3' :
            lines.append(Line('precision mediump float;'))
            if fs.highPrecision :
                if 'glsl100' == slVersion :
                    lines.append(Line('#ifdef GL_FRAGMENT_PRECISION_HIGH'))
                for type in fs.highPrecision :
                    lines.append(Line('precision highp {};'.format(type)))
                if 'glsl100' == slVersion :
                    lines.append(Line('#endif'))

        # write uniform definition
        lines = self.genUniforms(fs, slVersion, lines)

        # write fragment shader inputs
        for input in fs.inputs :
            if glslVersionNumber[slVersion] < 130 :
                lines.append(Line('varying {} {};'.format(input.type, input.name), input.filePath, input.lineNumber))
            else :
                lines.append(Line('in {} {};'.format(input.type, input.name), input.filePath, input.lineNumber))

        # write the fragcolor output
        if glslVersionNumber[slVersion] >= 130 :
            if glslVersionNumber[slVersion] >= 300 :
                lines.append(Line('layout (location = 0) out vec4 _FragColor;'))
                if fs.hasColor1 :
                    lines.append(Line('layout (location = 1) out vec4 _FragColor1;'))
                if fs.hasColor2 :
                    lines.append(Line('layout (location = 2) out vec4 _FragColor2;'))
                if fs.hasColor3 :
                    lines.append(Line('layout (location = 3) out vec4 _FragColor3;'))
            else :
                lines.append(Line('out vec4 _FragColor;'))

        # write blocks the fs depends on
        for dep in fs.resolvedDeps :
            lines = self.genLines(lines, self.shaderLib.codeBlocks[dep].lines)

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
        lines.append(Line('class _tex3d {'))
        lines.append(Line('    Texture3D t;'))
        lines.append(Line('    SamplerState s;'))
        lines.append(Line('};'))
        lines.append(Line('class _tex2darray {'))
        lines.append(Line('    Texture2DArray t;'))
        lines.append(Line('    SamplerState s;'))
        lines.append(Line('};'))
        lines.append(Line('#define sampler2D _tex2d'))
        lines.append(Line('#define samplerCube _texcube'))
        lines.append(Line('#define sampler3D _tex3d'))
        lines.append(Line('#define sampler2DArray _tx2darray'))
        lines.append(Line('#endif'))
        return lines

    #---------------------------------------------------------------------------
    def genUniforms(self, shd, lines) :
       
        # write textures
        for tb in shd.textureBlocks :
            for tex in tb.textures :
                if tex.type == 'sampler2D':
                    texType = 'Texture2D'
                elif tex.type == 'sampler2DArray':
                    texType = 'Texture2DArray'
                elif tex.type == 'sampler3D':
                    texType = 'Texture3D'
                else :
                    texType = 'TextureCube'
                lines.append(Line('{} _t_{} : register(t{});'.format(texType, tex.name, tex.bindSlot), tex.filePath, tex.lineNumber))
                lines.append(Line('SamplerState _s_{} : register(s{});'.format(tex.name, tex.bindSlot), tex.filePath, tex.lineNumber))

        # write uniform blocks
        for uBlock in shd.uniformBlocks :
            lines.append(Line('cbuffer {} : register(b{}) {{'.format(uBlock.name, uBlock.bindSlot), uBlock.filePath, uBlock.lineNumber))
            for type in uBlock.uniformsByType :
                for uniform in uBlock.uniformsByType[type] :
                    if uniform.num == 1 :
                        lines.append(Line('  {} {};'.format(uniform.type, uniform.name), 
                            uniform.filePath, uniform.lineNumber))
                    else :
                        lines.append(Line('  {} {}[{}];'.format(uniform.type, uniform.name, uniform.num), 
                            uniform.filePath, uniform.lineNumber))
                    # pad vec3's to 16 bytes
                    if type == 'vec3' :
                        lines.append(Line('  float _pad_{};'.format(uniform.name)))
            lines.append(Line('};', uBlock.filePath, uBlock.lineNumber))
        return lines
     
    #---------------------------------------------------------------------------
    def genLocalTexObjects(self, shd, lines) :
        for tb in shd.textureBlocks :
            for tex in tb.textures :
                if tex.type == 'sampler2D' :
                    lines.append(Line('_tex2d {}; {}.t=_t_{}; {}.s=_s_{};'.format(
                        tex.name, tex.name, tex.name, tex.name, tex.name),
                        tex.filePath, tex.lineNumber))
                elif tex.type == 'samplerCube' :
                    lines.append(Line('_texcube {}; {}.t=_t_{}; {}.s=_s_{};'.format(
                        tex.name, tex.name, tex.name, tex.name, tex.name),
                        tex.filePath, tex.lineNumber))
                elif tex.type == 'sampler2DArray' :
                    lines.append(Line('_tex2darray {}; {}.t=_t_{}; {}.s=_s_{};'.format(
                        tex.name, tex.name, tex.name, tex.name, tex.name),
                        tex.filePath, tex.lineNumber))
                elif tex.type == 'sampler3D':
                    lines.append(Line('_tex3d {}; {}.t=_t_{}; {}.s=_s_{};'.format(
                        tex.name, tex.name, tex.name, tex.name, tex.name),
                        tex.filePath, tex.lineNumber))
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
        lines = self.genUniforms(vs, lines)

        # write code blocks the vs depends on
        for dep in vs.resolvedDeps :
            lines = self.genLines(lines, self.shaderLib.codeBlocks[dep].lines)
    
        # write the main() function
        lines.append(Line('void main(', vs.lines[0].path, vs.lines[0].lineNumber))
        for input in vs.inputs :
            l = 'in {} {} : {},'.format(input.type, input.name, input.name)
            lines.append(Line(l, input.filePath, input.lineNumber))
        if vs.hasVertexId :
            lines.append(Line('in uint _iVertexID : SV_VertexID,'))
        if vs.hasInstanceId :
            lines.append(Line('in uint _iInstanceID : SV_InstanceID,'))
        for output in vs.outputs :
            l = 'out {} {} : {},'.format(output.type, output.name, output.name)
            lines.append(Line(l, output.filePath, output.lineNumber))
        # NOTE: Point Size isn't actually supported in D3D11!
        if vs.hasPointSize :
            lines.append(Line('out float _oPointSize : PSIZE,'))
        lines.append(Line('out vec4 _oPosition : SV_Position) {', vs.lines[0].path, vs.lines[0].lineNumber))
        lines = self.genLocalTexObjects(vs, lines)
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
        lines = self.genUniforms(fs, lines)

        # write blocks the fs depends on
        for dep in fs.resolvedDeps :
            lines = self.genLines(lines, self.shaderLib.codeBlocks[dep].lines)
        
        # write the main function
        lines.append(Line('void main(', fs.lines[0].path, fs.lines[0].lineNumber))
        for input in fs.inputs :
            l = 'in {} {} : {},'.format(input.type, input.name, input.name)
            lines.append(Line(l, input.filePath, input.lineNumber))
        if fs.hasFragCoord:
            lines.append(Line('in float4 _fragcoord : SV_Position,'))
        if fs.hasColor1:
            lines.append(Line('out vec4 _oColor1 : SV_Target1,'))
        if fs.hasColor2:
            lines.append(Line('out vec4 _oColor2 : SV_Target2,'))
        if fs.hasColor3:
            lines.append(Line('out vec4 _oColor3 : SV_Target3,'))
        lines.append(Line('out vec4 _oColor : SV_Target) {', fs.lines[0].path, fs.lines[0].lineNumber))
        lines = self.genLocalTexObjects(fs, lines)
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
        lines.append(Line('typedef _tex<texture3d<float,access::sample>> sampler3D;'))
        lines.append(Line('typedef _tex<texture2d_array<float,access::sample>> sampler2DArray;'))
        lines.append(Line('#endif'))
        return lines

    #---------------------------------------------------------------------------
    def genUniforms(self, shd, lines) :
        uniformDefs = {}
        for uBlock in shd.uniformBlocks :
            lines.append(Line('struct {}_{}_t {{'.format(shd.name, uBlock.name), uBlock.filePath, uBlock.lineNumber))
            for type in uBlock.uniformsByType :
                for uniform in uBlock.uniformsByType[type] :
                    if uniform.num == 1 :
                        lines.append(Line('  {} {};'.format(uniform.type, uniform.name), 
                            uniform.filePath, uniform.lineNumber))
                    else :
                        lines.append(Line('  {} {}[{}];'.format(uniform.type, uniform.name, uniform.num), 
                            uniform.filePath, uniform.lineNumber))
                    uniformDefs[uniform.name] = '{}.{}'.format(uBlock.name, uniform.name)
            lines.append(Line('};', uBlock.filePath, uBlock.lineNumber))
        return lines, uniformDefs

    #---------------------------------------------------------------------------
    def genFuncArgs(self, shd, lines) :
        for uBlock in shd.uniformBlocks :
            lines.append(Line('constant {}_{}_t& {} [[buffer({})]],'.format(shd.name, uBlock.name, uBlock.name, uBlock.bindSlot)))
        for tb in shd.textureBlocks :
            for tex in tb.textures :
                if tex.type == 'sampler2D' :
                    lines.append(Line('texture2d<float,access::sample> _t_{} [[texture({})]],'.format(
                        tex.name, tex.bindSlot), tex.filePath, tex.lineNumber))
                    lines.append(Line('sampler _s_{} [[sampler({})]],'.format(
                        tex.name, tex.bindSlot), tex.filePath, tex.lineNumber))
                elif tex.type == 'samplerCube' :
                    lines.append(Line('texturecube<float,access::sample> _t_{} [[texture({})]],'.format(
                        tex.name, tex.bindSlot), tex.filePath, tex.lineNumber))
                    lines.append(Line('sampler _s_{} [[sampler({})]],'.format(
                        tex.name, tex.bindSlot), tex.filePath, tex.lineNumber))
                elif tex.type == 'sampler3D' :
                    lines.append(Line('texture3d<float,access::sample> _t_{} [[texture({})]],'.format(
                        tex.name, tex.bindSlot), tex.filePath, tex.lineNumber))
                    lines.append(Line('sampler _s_{} [[sampler({})]],'.format(
                        tex.name, tex.bindSlot), tex.filePath, tex.lineNumber))
                elif tex.type == 'sampler2DArray' :
                    lines.append(Line('texture2d_array<float,access::sample> _t_{} [[texture({})]],'.format(
                        tex.name, tex.bindSlot), tex.filePath, tex.lineNumber))
                    lines.append(Line('sampler _s_{} [[sampler({})]],'.format(
                        tex.name, tex.bindSlot), tex.filePath, tex.lineNumber))
        if shd.hasVertexId :
            lines.append(Line('uint vs_vertexid [[vertex_id]],'))
        if shd.hasInstanceId :
            lines.append(Line('uint vs_instanceid [[instance_id]],'))
        return lines

    #---------------------------------------------------------------------------
    def genLocalTexObjects(self, shd, lines) :
        for tb in shd.textureBlocks :
            for tex in tb.textures :
                if tex.type == 'sampler2D' :
                    lines.append(Line('sampler2D {}(_t_{},_s_{});'.format(
                        tex.name, tex.name, tex.name), tex.filePath, tex.lineNumber))
                elif tex.type == 'samplerCube' :
                    lines.append(Line('samplerCube {}(_t_{},_s_{});'.format(
                        tex.name, tex.name, tex.name), tex.filePath, tex.lineNumber))
                elif tex.type == 'sampler3D' :
                    lines.append(Line('sampler3D {}(_t_{},_s_{});'.format(
                        tex.name, tex.name, tex.name), tex.filePath, tex.lineNumber))
                elif tex.type == 'sampler2DArray' :
                    lines.append(Line('sampler2DArray {}(_t_{},_s_{});'.format(
                        tex.name, tex.name, tex.name), tex.filePath, tex.lineNumber))
        return lines

    #---------------------------------------------------------------------------
    def genVertexShaderSource(self, vs, slVersion) :
        lines = []

        lines = self.genHeader(lines)

        # write compatibility macros
        for func in slMacros[slVersion] :
            lines.append(Line('#define {} {}'.format(func, slMacros[slVersion][func])))

        # write uniform blocks as cbuffers
        lines, uniformDefs = self.genUniforms(vs, lines)

        # write blocks the vs depends on
        # (Metal specific: protect against multiple inclusion)
        for dep in vs.resolvedDeps :
            guard = '_ORYOL_BLOCK_{}'.format(self.shaderLib.codeBlocks[dep].name)
            lines.append(Line('#ifndef {}'.format(guard)))
            lines.append(Line('#define {}'.format(guard)))
            lines = self.genLines(lines, self.shaderLib.codeBlocks[dep].lines)
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
        if vs.hasPointSize :
            lines.append(Line('    float _vofi_pointsize [[point_size]];'))
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
        lines = self.genLocalTexObjects(vs, lines)
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
        lines, uniformDefs = self.genUniforms(fs, lines)

        # write blocks the fs depends on
        for dep in fs.resolvedDeps :
            guard = '{}_INCLUDED'.format(self.shaderLib.codeBlocks[dep].name)
            lines.append(Line('#ifndef {}'.format(guard)))
            lines.append(Line('#define {}'.format(guard)))
            lines = self.genLines(lines, self.shaderLib.codeBlocks[dep].lines)
            lines.append(Line('#endif'))

        # write fragment shader input structure
        lines.append(Line('struct {}_fs_in_t {{'.format(fs.name)))
        for input in fs.inputs :
            lines.append(Line('    {} _vofi_{};'.format(input.type, input.name), input.filePath, input.lineNumber))
        lines.append(Line('};'))

        # write output struct
        lines.append(Line('struct {}_fs_out_t {{'.format(fs.name)))
        lines.append(Line('    float4 color0 [[color(0)]];'))
        if fs.hasColor1:
            lines.append(Line('    float4 color1 [[color(1)]];'))
        if fs.hasColor2:
            lines.append(Line('    float4 color2 [[color(2)]];'))
        if fs.hasColor3:
            lines.append(Line('    float4 color3 [[color(3)]];'))
        lines.append(Line('};'))

        # write the main function
        for input in fs.inputs :
            l = '#define {} fs_in._vofi_{}'.format(input.name, input.name)
            lines.append(Line(l, input.filePath, input.lineNumber))
        for uniformDef in uniformDefs :
            lines.append(Line('#define {} {}'.format(uniformDef, uniformDefs[uniformDef])))
        lines.append(Line('fragment {}_fs_out_t {}('.format(fs.name, fs.name), fs.lines[0].path, fs.lines[0].lineNumber))
        lines = self.genFuncArgs(fs, lines)
        if fs.hasFragCoord:
            lines.append(Line('float4 _fragcoord [[position]],'))
        lines.append(Line('{}_fs_in_t fs_in [[stage_in]]) {{'.format(fs.name, fs.name), fs.lines[0].path, fs.lines[0].lineNumber))
        lines.append(Line('{}_fs_out_t _fo_color;'.format(fs.name)))
        lines = self.genLocalTexObjects(fs, lines)
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
        self.codeBlocks = {}
        self.uniformBlocks = {}
        self.textureBlocks = {}
        self.vertexShaders = {}
        self.fragmentShaders = {}
        self.programs = {}
        self.current = None

    def dump(self) :
        dumpObj(self)
        print('Blocks:')
        for cb in self.codeBlocks.values() :
            cb.dump()
        print('UniformBlocks:')
        for ub in self.uniformBlocks.values() :
            ub.dump()
        print('TextureBlocks:')
        for tb in self.textureBlocks.values() :
            tb.dump()
        print('Vertex Shaders:')
        for vs in self.vertexShaders.values() :
            vs.dump()
        print('Fragment Shaders:')
        for fs in self.fragmentShaders.values() :
            fs.dump()
        print('Programs:')
        for prog in self.programs.values() :
            program.dump()

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
        if not dep.name in self.codeBlocks :
            util.setErrorLocation(dep.path, dep.lineNumber)
            util.fmtError("unknown code_block dependency '{}'".format(dep.name))
        shd.resolvedDeps.append(dep.name)
        for depdep in self.codeBlocks[dep.name].dependencies :
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

    def checkAddUniformBlock(self, uniformBlockRef, list) :
        '''
        Resolve a uniform block ref and add to list with sanity checks.
        '''
        if uniformBlockRef.name in self.uniformBlocks :
            if not findByName(uniformBlockRef.name, list) :
                uniformBlock = self.uniformBlocks[uniformBlockRef.name]
                list.append(uniformBlock)
        else :
            util.setErrorLocation(uniformBlockRef.filePath, uniformBlockRef.lineNumber)
            util.fmtError("uniform_block '%s' not found!".format(uniformBlockRef.name))

    def checkAddTextureBlock(self, textureBlockRef, list) :
        '''
        Resolve a texture block ref and add to list with sanity checks
        '''
        if textureBlockRef.name in self.textureBlocks :
            if not findByName(textureBlockRef.name, list) :
                textureBlock = self.textureBlocks[textureBlockRef.name]
                list.append(textureBlock)
            else :
                util.setErrorLocation(textureBlockRef.filePath, textureBlockRef.lineNumber)
                util.fmtError("texture_block '%s' not found!".format(textureBlockRef.name))

    def resolveUniformAndTextureBlocks(self, program) :
        '''
        Gathers all uniform- and texture-blocks from all shaders in the program
        and assigns the bindStage
        '''
        if program.vs not in self.vertexShaders :
            util.setErrorLocation(program.filePath, program.lineNumber)
            util.fmtError("unknown vertex shader '{}'".format(program.vs))
        for uniformBlockRef in self.vertexShaders[program.vs].uniformBlockRefs :
            self.checkAddUniformBlock(uniformBlockRef, program.uniformBlocks)
        for textureBlockRef in self.vertexShaders[program.vs].textureBlockRefs :
            self.checkAddTextureBlock(textureBlockRef, program.textureBlocks)

        if program.fs not in self.fragmentShaders :
            util.setErrorLocation(program.filePath, program.lineNumber)
            util.fmtError("unknown fragment shader '{}'".format(program.fs))
        for uniformBlockRef in self.fragmentShaders[program.fs].uniformBlockRefs :
            self.checkAddUniformBlock(uniformBlockRef, program.uniformBlocks)
        for textureBlockRef in self.fragmentShaders[program.fs].textureBlockRefs :
            self.checkAddTextureBlock(textureBlockRef, program.textureBlocks)
    
    def assignBindSlotIndices(self, program) :
        '''
        Assigns bindSlotIndex to uniform-blocks and
        to textures inside texture blocks. These
        are counted separately for the different shader stages (each
        shader stage has its own bind slots)
        '''
        vsUBSlot = 0
        fsUBSlot = 0
        for ub in program.uniformBlocks :
            if ub.bindStage == 'vs' :
                ub.bindSlot = vsUBSlot
                vsUBSlot += 1
            else :
                ub.bindSlot = fsUBSlot
                fsUBSlot += 1
        vsTexSlot = 0
        fsTexSlot = 0
        for tb in program.textureBlocks :
            if tb.bindStage == 'vs' :
                for tex in tb.textures :
                    tex.bindSlot = vsTexSlot
                    vsTexSlot += 1
            else :
                for tex in tb.textures :
                    tex.bindSlot = fsTexSlot
                    fsTexSlot += 1

    def resolveAllDependencies(self) :
        '''
        Resolve all dependencies for vertex- and fragment shaders.
        This populates the resolvedDeps, uniformBlocks and textureBlocks arrays. 
        '''
        for vs in self.vertexShaders.values() :
            for dep in vs.dependencies :
                self.resolveDeps(vs, dep)
            self.removeDuplicateDeps(vs)
        for fs in self.fragmentShaders.values() :
            for dep in fs.dependencies :
                self.resolveDeps(fs, dep)
            self.removeDuplicateDeps(fs)
        for program in self.programs.values() :
            self.resolveUniformAndTextureBlocks(program)
            self.assignBindSlotIndices(program)

    def validate(self) :
        '''
        Runs additional validation check after programs are resolved and before
        shader code is generated:

        - check whether each vs and fs is part of a program

        - check whether vertex shader output signatures match fragment
        shader input signatures, this is a D3D11 requirement, signatures 
        must match exactly, even if the fragment shader doesn't use all output
        from the vertex shader
        '''        
        for vs_name,vs in self.vertexShaders.iteritems() :
            vs_found = False
            for prog in self.programs.values() :
                if vs_name == prog.vs :
                    vs_found = True
            if not vs_found :
                util.setErrorLocation(vs.lines[0].path, vs.lines[0].lineNumber)
                util.fmtError("vertex shader '{}' is not part of a program".format(vs_name), False)
                fatalError = True

        for fs_name,fs in self.fragmentShaders.iteritems() :
            fs_found = False
            for prog in self.programs.values() :
                if fs_name == prog.fs :
                    fs_found = True
            if not fs_found :
                util.setErrorLocation(fs.lines[0].path, fs.lines[0].lineNumber)
                util.fmtError("fragment shader '{}' is not part of a program".format(fs_name), False)
                fatalError = True
            
        for prog in self.programs.values() :
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

    def validateAndWriteShadersHLSL(self, absHdrPath, args) :
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
                    hlslcompiler.validate(srcLines, 'vs', slVersion, outPath, cName, args)
                for fs in self.fragmentShaders.values() :
                    srcLines = fs.generatedSource[slVersion]
                    cName = fs.name + '_' + slVersion + '_src'
                    outPath = rootPath + '_' + cName + '.h'
                    hlslcompiler.validate(srcLines, 'fs', slVersion, outPath, cName, args)

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
                metalcompiler.validate(util.getEnv('target_platform'), srcLines, outPath, cName)

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

#-------------------------------------------------------------------------------
def writeHeaderBottom(f, shdLib) :
    f.write('}\n')
    f.write('\n')

#-------------------------------------------------------------------------------
def writeProgramHeader(f, shdLib, program) :
    f.write('    class ' + program.name + ' {\n')
    f.write('    public:\n')
    
    # write uniform block structs
    for ub in program.uniformBlocks :
        if ub.bindStage == 'vs' :
            stageName = 'VS'
        else :
            stageName = 'FS'
        f.write('        #pragma pack(push,1)\n')
        f.write('        struct {} {{\n'.format(ub.bindName))
        f.write('            static const int _bindSlotIndex = {};\n'.format(ub.bindSlot))
        f.write('            static const ShaderStage::Code _bindShaderStage = ShaderStage::{};\n'.format(stageName))
        f.write('            static const uint32_t _layoutHash = {};\n'.format(ub.getHash()))
        for type in ub.uniformsByType :
            for uniform in ub.uniformsByType[type] :
                if uniform.num == 1 :
                    f.write('            {} {};\n'.format(uniformCType[uniform.type], uniform.bindName))
                else :
                    f.write('            {} {}[{}];\n'.format(uniformCType[uniform.type], uniform.bindName, uniform.num))
                # for vec3's we need to add a padding field, FIXME: would be good
                # to try filling the padding fields with float params!
                if type == 'vec3' :
                    f.write('            float _pad_{};\n'.format(uniform.bindName))
        f.write('        };\n')
        f.write('        #pragma pack(pop)\n')
    f.write('        static ShaderSetup Setup();\n')
    f.write('    };\n')

#-------------------------------------------------------------------------------
def writeTextureBlocksHeader(f, shdLib) :
    # write texture bind slot constants
    for tb in shdLib.textureBlocks.values() :
        f.write('    struct {} {{\n'.format(tb.bindName))
        for tex in tb.textures :
            f.write('        static const int {} = {};\n'.format(tex.bindName, tex.bindSlot))
        f.write('    };\n')

#-------------------------------------------------------------------------------
def generateHeader(absHeaderPath, shdLib) :
    f = open(absHeaderPath, 'w')
    writeHeaderTop(f, shdLib)
    for prog in shdLib.programs.values() :
        writeProgramHeader(f, shdLib, prog)
    writeTextureBlocksHeader(f, shdLib)
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
    f.write('#if ORYOL_D3D11 || ORYOL_D3D12\n')
    f.write('typedef unsigned char BYTE;\n')
    f.write('#endif\n')

#-------------------------------------------------------------------------------
def writeSourceBottom(f, shdLib) :
    f.write('}\n')
    f.write('\n')

#-------------------------------------------------------------------------------
def writeShaderSource(f, absPath, shdLib, shd, slVersion) :
    # note: shd is either a VertexShader or FragmentShader object
    if isGLSL[slVersion] :
        # GLSL source code is directly inlined for runtime-compilation
        f.write('#if ORYOL_OPENGL\n')
        f.write('static const char* {}_{}_src = \n'.format(shd.name, slVersion))
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
def writeInputVertexLayout(f, vs) :
    # writes a C++ VertexLayout definition into the generated source
    # code, this is used to match mesh vertex layouts with 
    # vertex shader input signatures (e.g. required in D3D11),
    # return the C++ name of the vertex layout
    layoutName = '{}_input'.format(vs.name)
    f.write('    VertexLayout {};\n'.format(layoutName))
    for attr in vs.inputs :
        f.write('    {}.Add({}, {});\n'.format(layoutName, attrOryolName[attr.name], attrOryolType[attr.type]))
    return layoutName

#-------------------------------------------------------------------------------
def writeProgramSource(f, shdLib, prog) :

    # write the Setup() function
    f.write('ShaderSetup ' + prog.name + '::Setup() {\n')
    f.write('    ShaderSetup setup("' + prog.name + '");\n')
    vs = shdLib.vertexShaders[prog.vs]
    fs = shdLib.fragmentShaders[prog.fs]
    vsInputLayout = writeInputVertexLayout(f, vs)
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
            f.write('    setup.SetProgramFromSources({}, {}, {});\n'.format(
                slangType, vsSource, fsSource));
        elif isHLSL[slVersion] :
            f.write('    setup.SetProgramFromByteCode({}, {}, sizeof({}), {}, sizeof({}));\n'.format(
                slangType, vsSource, vsSource, fsSource, fsSource))
        elif isMetal[slVersion] :
            f.write('    setup.SetProgramFromLibrary({}, "{}", "{}");\n'.format(
                slangType, vsName, fsName))
        f.write('    setup.SetInputLayout({});\n'.format(vsInputLayout))
        f.write('    #endif\n');

    # add uniform layouts to setup object
    for ub in prog.uniformBlocks :
        layoutName = '{}_ublayout'.format(ub.bindName)
        f.write('    UniformBlockLayout {};\n'.format(layoutName))
        f.write('    {}.TypeHash = {};\n'.format(layoutName, ub.getHash()))
        for type in ub.uniformsByType :
            for uniform in ub.uniformsByType[type] :
                if uniform.num == 1 :
                    f.write('    {}.Add("{}", {});\n'.format(layoutName, uniform.name, uniformOryolType[uniform.type]))
                else :
                    f.write('    {}.Add("{}", {}, {});\n'.format(layoutName, uniform.name, uniformOryolType[uniform.type], uniform.num))
        f.write('    setup.AddUniformBlock("{}", {}, {}::_bindShaderStage, {}::_bindSlotIndex);\n'.format(
            ub.name, layoutName, ub.bindName, ub.bindName))

    # add texture layouts to setup objects
    for tb in prog.textureBlocks :
        layoutName = '{}_tblayout'.format(tb.bindName)
        f.write('    TextureBlockLayout {};\n'.format(layoutName))
        for tex in tb.textures :
            f.write('    {}.Add("{}", {}, {});\n'.format(layoutName, tex.name, texOryolType[tex.type], tex.bindSlot))
        if tb.bindStage == 'vs' :
            stageName = 'VS'
        else :
            stageName = 'FS'
        f.write('    setup.AddTextureBlock("{}", {}, ShaderStage::{});\n'.format(
            tb.name, layoutName, stageName))
                
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
    for prog in shdLib.programs.values() :
        writeProgramSource(f, shdLib, prog)
    writeSourceBottom(f, shdLib)  
    
    f.close()

#-------------------------------------------------------------------------------
def generate(input, out_src, out_hdr, args) :
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
            shaderLibrary.validateAndWriteShadersHLSL(out_hdr, args)
        if platform.system() == 'Darwin' :
            shaderLibrary.validateAndWriteShadersMetal(out_hdr)
        generateSource(out_src, shaderLibrary)
        generateHeader(out_hdr, shaderLibrary)

