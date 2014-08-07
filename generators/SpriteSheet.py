'''
Code generator for sprite sheets.
'''
import util
import png
import os
from pprint import pprint

Version = 2

#-------------------------------------------------------------------------------
class Sprite :
    def __init__(self) :
        self.name = ''
        self.x = 0
        self.y = 0
        self.w = 0
        self.h = 0
        self.frames = 1
        self.anim = ''

#-------------------------------------------------------------------------------
class SpriteSheet :
    def __init__(self, xmlTree, absXmlPath) :
        self.xmlPath = absXmlPath
        self.xmlRoot = xmlTree.getroot()
        self.name = ''
        self.imagePath = ''
        self.imageWidth = 0
        self.imageHeight = 0
        self.clampWidth = 0
        self.clampHeight = 0
        self.imagePixels = None
        self.imageInfo = None
        self.defSpriteWidth = 0
        self.defSpriteHeight = 0
        self.sprites = []

    def parseXml(self) :
        self.name = self.xmlRoot.get('name')
        xmlSheet = self.xmlRoot.find('Sheet')
        if xmlSheet is not None :
            self.imagePath = os.path.dirname(self.xmlPath) + '/' + xmlSheet.get('file')
            self.defSpriteWidth = int(xmlSheet.get('spriteWidth', 16))
            self.defSpriteHeight = int(xmlSheet.get('spriteHeight', 16))
            self.clampWidth = int(xmlSheet.get('clampWidth', 0))
            self.clampHeight = int(xmlSheet.get('clampHeight', 0))
            for xmlSprite in xmlSheet.findall('Sprite') :
                sprite = Sprite()
                sprite.name = xmlSprite.get('name', 'undefined')
                sprite.x = int(xmlSprite.get('x', 0))
                sprite.y = int(xmlSprite.get('y', 0))
                sprite.w = int(xmlSprite.get('w', self.defSpriteWidth))
                sprite.h = int(xmlSprite.get('h', self.defSpriteHeight))
                sprite.frames = int(xmlSprite.get('frames', 1))
                sprite.anim = xmlSprite.get('anim', 'none')
                self.sprites.append(sprite)
        else :
            util.error('No Sheet element in sprite sheet XML file!')
        pprint(vars(self))
        for s in self.sprites :
            pprint(vars(s))

    def loadImage(self) :
        pngReader = png.Reader(self.imagePath)
        img = pngReader.asRGBA8()
        self.imageWidth = img[0]
        self.imageHeight = img[1]
        if self.clampWidth > 0 and self.clampWidth < self.imageWidth :
            self.imageWidth = self.clampWidth
            print 'Clamped image width to {}'.format(self.imageWidth)
        if self.clampHeight > 0 and self.clampHeight < self.imageHeight :
            self.imageHeight = self.clampHeight
            print 'Clamped image height to {}'.format(self.imageHeight)

        self.imagePixels = img[2]
        self.imageInfo = img[3]
        print 'w={} h={} pixels={} info={}'.format(self.imageWidth, self.imageHeight, self.imagePixels, self.imageInfo)

#-------------------------------------------------------------------------------
def writeHeaderTop(f, spriteSheet) :
    f.write('#pragma once\n')
    f.write('//-----------------------------------------------------------------------------\n')
    f.write('/*  #version:{}#\n'.format(Version))
    f.write('    machine generated, do not edit!\n')
    f.write('*/\n')
    f.write('#include "Core/Types.h"\n')
    f.write('namespace Oryol {\n')

#-------------------------------------------------------------------------------
def writeHeaderBottom(f, spriteSheet) :
    f.write('}\n')
    f.write('\n')

#-------------------------------------------------------------------------------
def writeSpriteSheet(f, spriteSheet) :
    f.write('struct ' + spriteSheet.name + ' {\n')
    f.write('    static const uint32 Width;\n')
    f.write('    static const uint32 Height;\n')
    f.write('    static const uint32 Pixels[{}];\n'.format(spriteSheet.imageWidth * spriteSheet.imageHeight))
    f.write('    enum Sprite {\n')
    for sprite in spriteSheet.sprites :
        f.write('        ' + sprite.name + ',\n')
    f.write('\n')
    f.write('        NumSprites,\n')
    f.write('        InvalidSprite\n')
    f.write('    };\n')
    f.write('    class Anim {\n')
    f.write('    public:\n')
    f.write('        enum Code {\n')
    f.write('            None,\n')
    f.write('            Loop,\n')
    f.write('            PingPong,\n')
    f.write('            Clamp,\n')
    f.write('        };\n')
    f.write('    };\n')
    f.write('    static const struct {\n')
    f.write('        int32 X;\n')
    f.write('        int32 Y;\n')
    f.write('        int32 W;\n')
    f.write('        int32 H;\n')
    f.write('        int32 NumFrames;\n')
    f.write('        Anim::Code AnimType;\n')
    f.write('    } Sprites[NumSprites];\n')
    f.write('};\n')
#-------------------------------------------------------------------------------
def generateHeader(absHeaderPath, spriteSheet) :
    f = open(absHeaderPath, 'w')
    writeHeaderTop(f, spriteSheet)
    writeSpriteSheet(f, spriteSheet)
    writeHeaderBottom(f, spriteSheet)
    f.close()

#-------------------------------------------------------------------------------
def writeSourceTop(f, absSourcePath, spriteSheet) :
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
def writeImageData(f, spriteSheet) :
    width = spriteSheet.imageWidth
    height = spriteSheet.imageHeight
    numPixels = width * height
    f.write('const uint32 ' + spriteSheet.name + '::Width = ' + str(width) + ';\n')
    f.write('const uint32 ' + spriteSheet.name + '::Height = ' + str(height) + ';\n')    
    f.write('const uint32 ' + spriteSheet.name + '::Pixels[' + str(numPixels) + '] = {\n')
    for y,row in enumerate(spriteSheet.imagePixels) :
        if y < spriteSheet.imageHeight :
            f.write('    ')
            for x in xrange(0, width) :
                offset = x * 4
                r = row[offset]
                g = row[offset + 1]
                b = row[offset + 2]
                a = row[offset + 3]
                f.write('0x{:02x}{:02x}{:02x}{:02x},'.format(a, r, g, b))
            f.write('\n')
    f.write('};\n')

#-------------------------------------------------------------------------------
def writeSourceBottom(f, spriteSheet) :
    f.write('}\n')
    f.write('\n')

#-------------------------------------------------------------------------------
def generateSource(absSourcePath, spriteSheet) :
    f = open(absSourcePath, 'w')
    writeSourceTop(f, absSourcePath, spriteSheet)
    writeImageData(f, spriteSheet)
    writeSourceBottom(f, spriteSheet)
    f.close()

#-------------------------------------------------------------------------------
def isDirty(xmlTree, absXmlPath, absSourcePath, absHeaderPath) :
    return True
    # return util.fileVersionDirty(absSourcePath, Version) or util.fileVersionDirty(absHeaderPath, Version)

#-------------------------------------------------------------------------------
def generate(xmlTree, absXmlPath, absSourcePath, absHeaderPath) :
    spriteSheet = SpriteSheet(xmlTree, absXmlPath)
    spriteSheet.parseXml()
    spriteSheet.loadImage()
    generateHeader(absHeaderPath, spriteSheet)
    generateSource(absSourcePath, spriteSheet)


