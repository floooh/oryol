'''
Code generator for shader libraries.

FIXME: the generated Metal and HLSL bytecode arrays must be made 'unique' (wrap them in a namespace)
'''

Version = 22

import os, platform, json
import genutil as util
from util import glslcompiler, shdc
from mod import log
import zlib # only for crc32

if platform.system() == 'Windows' :
    from util import hlslcompiler

if platform.system() == 'Darwin' :
    from util import metalcompiler

# SL versions for OpenGLES2.0, OpenGL2.1, OpenGL3.0, D3D11
slVersions = [ 'glsl100', 'glsl120', 'glsl330', 'glsles3', 'metal', 'hlsl']

slSlangTypes = {
    'glsl100': 'ShaderLang::GLSL100',
    'glsl120': 'ShaderLang::GLSL120',
    'glsl330': 'ShaderLang::GLSL330',
    'glsles3': 'ShaderLang::GLSLES3',
    'hlsl':    'ShaderLang::HLSL5',
    'metal':   'ShaderLang::Metal'
}

isGLSL = {
    'glsl100': True,
    'glsl120': True,
    'glsl330': True,
    'glsles3': True,
    'hlsl': False,
    'metal': False
}

isHLSL = {
    'glsl100': False,
    'glsl120': False,
    'glsl330': False,
    'glsles3': False,
    'hlsl': True,
    'metal': False
}

isMetal = {
    'glsl100': False,
    'glsl120': False,
    'glsl330': False,
    'glsles3': False,
    'hlsl': False,
    'metal': True
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
    'mat4', 'mat2',
    'vec4', 'vec3', 'vec2',
    'float', 'int'
]

# size of uniform array types must currently be multiple of 16,
# because of std140 padding rules
validUniformArrayTypes = [
    'mat4', 'mat2', 'vec4'
]

# note: alignment is in number of floats
uniformBaseAlignment = {
    'mat4': 4,
    'mat2': 4,
    'vec4': 4,
    'vec3': 4,
    'vec2': 2,
    'float': 1,
    'int': 1
}

# these are unpadded sizes in number of floats
uniformSize = {
    'mat4': 16,
    'mat2': 4,
    'vec4': 4,
    'vec3': 3,
    'vec2': 2,
    'float': 1,
    'int': 1
}

uniformCType = {
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

#-------------------------------------------------------------------------------
class Shader(Snippet) :
    '''
    Generic shader base class.
    '''
    def __init__(self, name) :
        Snippet.__init__(self)
        self.name = name
        self.resolvedDeps = []
        self.reflection = {}    # generic 'main reflection'
        self.slReflection = {}  # reflection by shader language 
        self.generatedSource = None

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
        self.filePath = filePath
        self.lineNumber = lineNumber        

    def getTag(self) :
        return 'program'

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
    def onUseCodeBlock(self, args) :
        if not self.current or not self.current.getTag() in ['code_block', 'vs', 'fs'] :
            util.fmtError("@use_code_block must come after @code_block, @vs or @fs!")
        if len(args) < 1:
            util.fmtError("@use_code_block must have at least one arg!")
        for arg in args :
            self.current.dependencies.append(Reference(arg, self.fileName, self.lineNumber))

    #---------------------------------------------------------------------------
    def onEnd(self, args) :
        if not self.current or not self.current.getTag() in ['uniform_block', 'texture_block', 'code_block', 'vs', 'fs', 'program'] :
            util.fmtError("@end must come after @uniform_block, @texture_block, @code_block, @vs, @fs or @program!")
        if len(args) != 0:
            util.fmtError("@end must not have arguments")
        if self.current.getTag() in ['code_block', 'vs', 'fs'] and len(self.current.lines) == 0 :
            util.fmtError("no source code lines in @code_block, @vs or @fs section")
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
class Generator :
    '''
    Generate vertex and fragment shader source code for generic GLSL
    as input to glslangValidator for SPIR-V generation.
    '''
    def __init__(self, shaderLib) :
        self.shaderLib = shaderLib

    #---------------------------------------------------------------------------
    def genLines(self, dstLines, srcLines) :
        for srcLine in srcLines :
            dstLines.append(srcLine)
        return dstLines
    #---------------------------------------------------------------------------
    def genVertexShaderSource(self, vs) :
        lines = []
        for dep in vs.resolvedDeps :
            lines = self.genLines(lines, self.shaderLib.codeBlocks[dep].lines)
        lines.extend(vs.lines)
        vs.generatedSource = lines

    #---------------------------------------------------------------------------
    def genFragmentShaderSource(self, fs) :
        lines = []
        for dep in fs.resolvedDeps :
            lines = self.genLines(lines, self.shaderLib.codeBlocks[dep].lines)
        lines.extend(fs.lines)
        fs.generatedSource = lines

#-------------------------------------------------------------------------------
class ShaderLibrary :
    '''
    This represents the entire shader lib.
    '''
    def __init__(self, inputs) :
        self.sources = inputs
        self.codeBlocks = {}
        self.vertexShaders = {}
        self.fragmentShaders = {}
        self.programs = {}
        self.current = None

    def parseSources(self) :
        parser = Parser(self)
        for source in self.sources :            
            parser.parseSource(source)

    def resolveDeps(self, shd, dep) :
        # just add new dependencies at the end of resolvedDeps,
        # and remove dups in a second pass after recursion
        if not dep.name in self.codeBlocks :
            util.setErrorLocation(dep.path, dep.lineNumber)
            util.fmtError("unknown code_block dependency '{}'".format(dep.name))
        shd.resolvedDeps.append(dep.name)
        for depdep in self.codeBlocks[dep.name].dependencies :
            self.resolveDeps(shd, depdep)

    def removeDuplicateDeps(self, shd) :
        deps = []
        for dep in shd.resolvedDeps :
            if not dep in deps :
                deps.append(dep)
        deps.reverse()
        shd.resolvedDeps = deps

    def resolveAllDependencies(self) :
        for vs in self.vertexShaders.values() :
            for dep in vs.dependencies :
                self.resolveDeps(vs, dep)
            self.removeDuplicateDeps(vs)
        for fs in self.fragmentShaders.values() :
            for dep in fs.dependencies :
                self.resolveDeps(fs, dep)
            self.removeDuplicateDeps(fs)

    def validate(self) :
        '''
        Runs additional validation check after programs are resolved and before
        shader code is generated:

        - check whether each vs and fs is part of a program
        - check vertex shader inputs for valid types and names
        - check whether vertex shader output matches fragment shader input
        '''        
        for vs_name,vs in self.vertexShaders.items() :
            vs_found = False
            for prog in self.programs.values() :
                if vs_name == prog.vs :
                    vs_found = True
            if not vs_found :
                util.setErrorLocation(vs.lines[0].path, vs.lines[0].lineNumber)
                util.fmtError("vertex shader '{}' is not part of a program".format(vs_name), False)
                fatalError = True
        for fs_name,fs in self.fragmentShaders.items() :
            fs_found = False
            for prog in self.programs.values() :
                if fs_name == prog.fs :
                    fs_found = True
            if not fs_found :
                util.setErrorLocation(fs.lines[0].path, fs.lines[0].lineNumber)
                util.fmtError("fragment shader '{}' is not part of a program".format(fs_name), False)
                fatalError = True
        for vs in self.vertexShaders.values():
            util.setErrorLocation(vs.lines[0].path, vs.lines[0].lineNumber)
            vs_inputs = vs.reflection['inputs']
            for vs_input in vs_inputs:
                if vs_input['name'] not in validVsInNames:
                    util.fmtError("invalid vertex shader input name '{}', must be ({})".format(vs_input['name'], ','.join(validVsInNames)))
                if vs_input['type'] not in validInOutTypes:
                    util.fmtError("invalid vertex shader input type '{}', must be ({})".format(vs_input['type'], ','.join(validInOutTypes)))
            for ub in vs.reflection['uniform_blocks']:
                for m in ub['members']:
                    validTypes = validUniformTypes if m['num']==1 else validUniformArrayTypes
                    if m['type'] not in validTypes:
                        util.fmtError("invalid uniform block member type '{}', must be ({})".format(m['type'], ','.join(validTypes)))
        for fs in self.fragmentShaders.values():
            util.setErrorLocation(fs.lines[0].path, fs.lines[0].lineNumber)
            for ub in fs.reflection['uniform_blocks']:
                for m in ub['members']:
                    validTypes = validUniformTypes if m['num']==1 else validUniformArrayTypes
                    if m['type'] not in validTypes:
                        util.fmtError("invalid uniform block member type '{}', must be ({})".format(m['type'], ','.join(validTypes)))
        for prog in self.programs.values():
            vs = self.vertexShaders[prog.vs]
            fs = self.fragmentShaders[prog.fs]
            vs_outputs = vs.reflection['outputs']
            fs_inputs = fs.reflection['inputs']
            vs_fs_error = False
            if len(vs_outputs) == len(fs_inputs):
                for vs_out in vs_outputs:
                    in_out_match = False
                    for fs_in in fs_inputs:
                        if (vs_out['name'] == fs_in['name']) and (vs_out['type'] == fs_in['type']):
                            in_out_match = True
                            break
                    if not in_out_match:
                        vs_fs_error = True
            if vs_fs_error:
                # number of inputs/outputs don't match
                vs_fs_error = True
                util.setErrorLocation(vs.lines[0].path, vs.lines[0].lineNumber)
                util.fmtError("outputs of vs '{}' don't match inputs of fs '{}' (unused items might have been removed)".format(vs.name, fs.name))

    def generateShaderSources(self) :
        gen = Generator(self)
        for vs in self.vertexShaders.values() :
            gen.genVertexShaderSource(vs)
        for fs in self.fragmentShaders.values() :
            gen.genFragmentShaderSource(fs)

    def loadReflection(self, shd, base_path):
        for sl in slVersions:
            refl_path = '{}.{}.json'.format(base_path, sl)
            with open(refl_path, 'r') as f:
                shd.slReflection[sl] = json.load(f)
        shd.reflection = shd.slReflection['glsl100']

    def compileShader(self, input, shd, shd_type, base_path, args):
        shd_base_path = base_path + '_' + shd.name
        glslcompiler.compile(shd.generatedSource, shd_type, shd_base_path, args)
        shdc.compile(input, shd_base_path, args)
#        for slVersion in slVersions :
#            if isGLSL[slVersion]:
#                glslcompiler.validate(slVersion, shd_type, shd_base_path, args)
        self.loadReflection(shd, shd_base_path)
        if platform.system() == 'Darwin':
            c_name = '{}_{}_metallib'.format(shd.name, shd_type)
            metalcompiler.compile(shd.generatedSource, shd_base_path, c_name, args)
        if platform.system() == 'Windows':
            c_name = '{}_{}_hlsl5'.format(shd.name, shd_type)
            hlslcompiler.compile(shd.generatedSource, shd_base_path, shd_type, c_name, args)

    def compile(self, input, out_hdr, args) :
        log.info('## shader code gen: {}'.format(input)) 
        base_path = os.path.splitext(out_hdr)[0]
        for vs in self.vertexShaders.values():
            self.compileShader(input, vs, 'vs', base_path, args)
        for fs in self.fragmentShaders.values():
            self.compileShader(input, fs, 'fs', base_path, args)

#-------------------------------------------------------------------------------
def writeHeaderTop(f, shdLib) :
    f.write('#pragma once\n')
    f.write('//-----------------------------------------------------------------------------\n')
    f.write('/*  #version:{}#\n'.format(Version))
    f.write('    machine generated, do not edit!\n')
    f.write('*/\n')
    f.write('#include "Gfx/Core/GfxTypes.h"\n')
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
def getUniformBlockTypeHash(ub_refl):
    hashString = ''
    for member in ub_refl['members']:
        hashString += member['type']
        hashString += str(member['num'])
    return zlib.crc32(hashString) & 0xFFFFFFFF

#-------------------------------------------------------------------------------
def alignedOffset(type, cur_offset):
    aligned_offset = cur_offset
    base_align = uniformBaseAlignment[type]
    if (aligned_offset & (base_align - 1)) == 0:
        # already aligned
        return aligned_offset
    else:
        return (aligned_offset + base_align) & ~(base_align - 1)

#-------------------------------------------------------------------------------
def writeProgramHeader(f, shdLib, prog) :
    f.write('    namespace ' + prog.name + ' {\n')
    
    # write uniform block structs
    for stage in ['VS', 'FS']:
        shd = shdLib.vertexShaders[prog.vs] if stage == 'VS' else shdLib.fragmentShaders[prog.fs]
        refl = shd.reflection
        for ub in refl['uniform_blocks']:
            f.write('        #pragma pack(push,1)\n')
            f.write('        struct {} {{\n'.format(ub['type']))
            f.write('            static const int _bindSlotIndex = {};\n'.format(ub['slot']))
            f.write('            static const ShaderStage::Code _bindShaderStage = ShaderStage::{};\n'.format(stage))
            f.write('            static const uint32_t _layoutHash = {};\n'.format(getUniformBlockTypeHash(ub)))
            for slang in ['metal', 'other']:
                cur_offset = 0
                if slang == 'metal':
                    f.write('            #if ORYOL_METAL\n')
                else:
                    f.write('            #if !ORYOL_METAL\n')
                for m in ub['members']:
                    # add padding
                    aligned_offset = alignedOffset(m['type'], cur_offset)
                    while cur_offset != aligned_offset:
                        f.write('                float _pad_{};\n'.format(cur_offset))
                        cur_offset += 1
                    if m['num'] == 1:
                        f.write('                {} {};\n'.format(uniformCType[m['type']], m['name']))
                        # special case: sizeof(vec3) is 16 bytes on metal
                        if slang == 'metal' and m['type'] == 'vec3':
                            f.write('                float _pad_{};\n'.format(cur_offset))
                            cur_offset += 1
                    else:
                        f.write('                {} {}[{}];\n'.format(uniformCType[m['type']], m['name'], m['num']))
                    cur_offset += uniformSize[m['type']] * m['num'] 
                f.write('            #endif\n')
            f.write('        };\n')
            f.write('        #pragma pack(pop)\n')
        for tex in refl['textures']:
            f.write('        static const int {} = {};\n'.format(tex['name'], tex['slot']))
    f.write('        extern ShaderSetup Setup();\n')
    f.write('    }\n')

#-------------------------------------------------------------------------------
def generateHeader(absHeaderPath, shdLib) :
    f = open(absHeaderPath, 'w')
    writeHeaderTop(f, shdLib)
    for prog in shdLib.programs.values() :
        writeProgramHeader(f, shdLib, prog)
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
    f.write('#if ORYOL_D3D11\n')
    f.write('typedef unsigned char BYTE;\n')
    f.write('#endif\n')

#-------------------------------------------------------------------------------
def writeSourceBottom(f, shdLib) :
    f.write('}\n')
    f.write('\n')

#-------------------------------------------------------------------------------
def writeShaderSource(f, absPath, shdLib, shd, slVersion) :
    # note: shd is either a VertexShader or FragmentShader object
    base_path = os.path.splitext(absPath)[0] + '_' + shd.name
    if isGLSL[slVersion] :
        # GLSL source code is directly inlined for runtime-compilation
        f.write('#if ORYOL_OPENGL\n')
        f.write('static const char* {}_{}_src = \n'.format(shd.name, slVersion))
        glsl_src_path = '{}.{}'.format(base_path, slVersion)
        with open(glsl_src_path, 'r') as rf:
            lines = rf.read().splitlines()
            for line in lines:
                f.write('"{}\\n"\n'.format(line))
        f.write(';\n')
        f.write('#endif\n')
    elif isHLSL[slVersion] :
        # for HLSL, the actual shader code has been compiled into a header by FXC
        # also write the generated shader source into a C comment as
        # human-readable version
        f.write('#if ORYOL_D3D11\n')
        f.write('/*\n')
        hlsl_src_path = base_path + '.hlsl'
        hlsl_bin_path = base_path + '.hlsl.h'
        with open(hlsl_src_path, 'r') as rf:
            lines = rf.read().splitlines()
            for line in lines:
                line = line.replace('/*', '__').replace('*/', '__')
                f.write('"{}\\n"\n'.format(line))
        f.write('*/\n')
        f.write('#include "{}"\n'.format(hlsl_bin_path))
        f.write('#endif\n')
    elif isMetal[slVersion] :
        # for Metal, the shader has been compiled into a binary shader
        # library file, which needs to be embedded into the C header
        f.write('#if ORYOL_METAL\n')
        f.write('/*\n')
        metal_src_path = base_path + '.metal'
        metal_bin_path = base_path + '.metallib.h'
        with open(metal_src_path, 'r') as rf:
            lines = rf.read().splitlines()
            for line in lines:
                line = line.replace('/*', '__').replace('*/', '__')
                f.write('"{}\\n"\n'.format(line))
        f.write('*/\n')
        f.write('#include "{}"\n'.format(metal_bin_path))
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
    for inp in vs.reflection['inputs'] :
        f.write('    {}.Add({}, {});\n'.format(layoutName, attrOryolName[inp['name']], attrOryolType[inp['type']]))
    return layoutName

#-------------------------------------------------------------------------------
def writeProgramSource(f, shdLib, prog) :

    # write the Setup() function
    f.write('ShaderSetup ' + prog.name + '::Setup() {\n')
    f.write('    ShaderSetup setup("' + prog.name + '");\n')
    vs = shdLib.vertexShaders[prog.vs]
    fs = shdLib.fragmentShaders[prog.fs]
    vsInputLayout = writeInputVertexLayout(f, vs)
    f.write('    setup.SetInputLayout({});\n'.format(vsInputLayout))
    vsName = vs.name
    fsName = fs.name
    for slVersion in slVersions :
        slangType = slSlangTypes[slVersion]
        if isGLSL[slVersion] :
            f.write('    #if ORYOL_OPENGL\n')
        elif isHLSL[slVersion] :
            f.write('    #if ORYOL_D3D11\n')
        elif isMetal[slVersion] :
            f.write('    #if ORYOL_METAL\n')
        vsSource = '{}_{}_src'.format(vsName, slVersion)
        fsSource = '{}_{}_src'.format(fsName, slVersion)
        if isGLSL[slVersion] :
            f.write('    setup.SetProgramFromSources({}, {}, {});\n'.format(
                slangType, vsSource, fsSource));
        elif isHLSL[slVersion] :
            vs_c_name = '{}_vs_hlsl5'.format(vs.name)
            fs_c_name = '{}_fs_hlsl5'.format(fs.name)
            f.write('    setup.SetProgramFromByteCode({}, {}, sizeof({}), {}, sizeof({}));\n'.format(
                slangType, vs_c_name, vs_c_name, fs_c_name, fs_c_name))
        elif isMetal[slVersion] :
            vs_c_name = '{}_vs_metallib'.format(vs.name)
            fs_c_name = '{}_fs_metallib'.format(fs.name)
            f.write('    setup.SetProgramFromByteCode({}, {}, sizeof({}), {}, sizeof({}), "main0", "main0");\n'.format(
                slangType, vs_c_name, vs_c_name, fs_c_name, fs_c_name))
        f.write('    #endif\n');

    # add uniform layouts to setup object
    for stage in ['VS', 'FS']:
        shd = shdLib.vertexShaders[prog.vs] if stage == 'VS' else shdLib.fragmentShaders[prog.fs]
        refl = shd.reflection
        # add uniform block layouts
        for ub in refl['uniform_blocks']:
            layoutName = '{}_ublayout'.format(ub['type'])
            f.write('    UniformBlockLayout {};\n'.format(layoutName))
            f.write('    {}.TypeHash = {};\n'.format(layoutName, getUniformBlockTypeHash(ub)))
            for m in ub['members']:
                if m['num'] == 1:
                    f.write('    {}.Add("{}", {});\n'.format(layoutName, m['name'], uniformOryolType[m['type']]))
                else:
                    f.write('    {}.Add("{}", {}, {});\n'.format(layoutName, m['name'], uniformOryolType[m['type']], m['num']))
            f.write('    setup.AddUniformBlock("{}", "{}", {}, {}::_bindShaderStage, {}::_bindSlotIndex);\n'.format(
                ub['type'], ub['name'], layoutName, ub['type'], ub['type']))
        # add textures layouts to setup objects
        for tex in refl['textures']:
            f.write('    setup.AddTexture("{}", {}, ShaderStage::{}, {});\n'.format(tex['name'], texOryolType[tex['type']], stage, tex['slot']))
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
        shaderLibrary.generateShaderSources()
        shaderLibrary.compile(input, out_hdr, args)
        shaderLibrary.validate()
        generateSource(out_src, shaderLibrary)
        generateHeader(out_hdr, shaderLibrary)

