'''
Code generator for sprite sheets.
'''
import genutil as util
import png
import os

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
        self.char = None

#-------------------------------------------------------------------------------
class SpriteSheet :
    def __init__(self, directory, name, dct) :
        self.directory = directory
        self.name = name
        self.namespace = dct['namespace']
        self.imagePath = directory + dct['image']
        self.imageWidth = 0
        self.imageHeight = 0
        self.clampWidth = dct.get('clampWidth', 0)
        self.clampHeight = dct.get('clampHeight', 0)
        self.imagePixels = None
        self.imageInfo = None
        self.defSpriteWidth = dct['spriteWidth']
        self.defSpriteHeight = dct['spriteHeight']
        self.sprites = []
        for s in dct['sprites'] :
            sprite = Sprite()
            sprite.name = s['name']
            sprite.x = s['x']
            sprite.y = s['y']
            sprite.w = s.get('w', self.defSpriteWidth)
            sprite.h = s.get('h', self.defSpriteHeight)
            sprite.frames = s.get('frames', 1)
            sprite.anim = s.get('anim', 'none')
            sprite.char = s.get('char', None)
            self.sprites.append(sprite)

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

    def buildCharMap(self) :
        charMap = [None] * 256
        for sprite in self.sprites :
            if sprite.char is not None:
                ascii = ord(sprite.char[0])
                if ascii < 256 :
                    charMap[ascii] = sprite
        return charMap

    def writeHeaderTop(self, f) :
        f.write('#pragma once\n')
        f.write('//-----------------------------------------------------------------------------\n')
        f.write('/*  #version:{}#\n'.format(Version))
        f.write('    machine generated, do not edit!\n')
        f.write('*/\n')
        f.write('#include "Core/Types.h"\n')
        f.write('namespace ' + self.namespace + ' {\n')

    def writeHeaderBottom(self, f) :
        f.write('}\n')
        f.write('\n')

    def writeSpriteSheet(self, f) :
        numPixels = self.imageWidth * self.imageHeight
        numBytes = numPixels * 4
        f.write('struct Sheet {\n')
        f.write('    static const Oryol::int32 Width{' + str(self.imageWidth) + '};\n')
        f.write('    static const Oryol::int32 Height{' + str(self.imageHeight) + '};\n')
        f.write('    static const Oryol::int32 NumBytes{' + str(numBytes) + '};\n')
        f.write('    static const Oryol::uint32 Pixels[{}];\n'.format(numPixels))
        f.write('    enum SpriteId {\n')
        for sprite in self.sprites :
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
        f.write('    static const SpriteId CharMap[256];\n')
        f.write('    static const struct sprite {\n')
        f.write('        SpriteId id;\n')
        f.write('        Oryol::int32 X;\n')
        f.write('        Oryol::int32 Y;\n')
        f.write('        Oryol::int32 W;\n')
        f.write('        Oryol::int32 H;\n')
        f.write('        Oryol::int32 NumFrames;\n')
        f.write('        Anim::Code AnimType;\n')
        f.write('        Oryol::uint8 Char;\n')
        f.write('    } Sprite[NumSprites];\n')
        f.write('};\n')

    def genHeader(self, absHeaderPath) :
        f = open(absHeaderPath, 'w')
        self.writeHeaderTop(f)
        self.writeSpriteSheet(f)
        self.writeHeaderBottom(f)
        f.close()

    def writeSourceTop(self, f, absSourcePath) :
        path, hdrFileAndExt = os.path.split(absSourcePath)
        hdrFile, ext = os.path.splitext(hdrFileAndExt)

        f.write('//-----------------------------------------------------------------------------\n')
        f.write('// #version:{}# machine generated, do not edit!\n'.format(Version))
        f.write('//-----------------------------------------------------------------------------\n')
        f.write('#include "Pre.h"\n')
        f.write('#include "' + hdrFile + '.h"\n')
        f.write('\n')
        f.write('namespace ' + self.namespace + ' {\n')

    def writeImageData(self, f) :
        width = self.imageWidth
        height = self.imageHeight
        numPixels = width * height
        f.write('const Oryol::uint32 Sheet::Pixels[' + str(numPixels) + '] = {\n')
        for y,row in enumerate(self.imagePixels) :
            if y < self.imageHeight :
                f.write('    ')
                for x in xrange(0, width) :
                    offset = x * 4
                    r = row[offset]
                    g = row[offset + 1]
                    b = row[offset + 2]
                    a = row[offset + 3]
                    f.write('0x{:02x}{:02x}{:02x}{:02x},'.format(a, b, g, r))
                f.write('\n')
        f.write('};\n')

    def writeSpriteData(self, f) :
        mapAnimType = {
            '':         'None',
            'none':     'None',
            'loop':     'Loop',
            'pingpong': 'PingPong',
            'clamp':    'Clamp'
        }

        f.write('const Sheet::sprite Sheet::Sprite[Sheet::NumSprites] = {\n')
        for sprite in self.sprites :
            name = str(sprite.name)
            x = str(sprite.x)
            y = str(sprite.y)
            w = str(sprite.w)
            h = str(sprite.h)
            frs = str(sprite.frames)
            anm = 'Sheet::Anim::' + mapAnimType[sprite.anim]
            char = 0
            if sprite.char is not None :
                char = ord(sprite.char[0])
            char = str(char)
            f.write('    { '+name+','+x+','+y+','+w+','+h+','+frs+','+anm+','+char+' },\n')
        f.write('};\n')

    def writeSpriteCharMap(self, f) :
        f.write('const Sheet::SpriteId Sheet::CharMap[256] = {\n')
        charMap = self.buildCharMap()
        for sprite in charMap :
            if sprite is None :
                f.write('    Sheet::InvalidSprite,\n')
            else :
                f.write('    Sheet::' + sprite.name + ',\n')
        f.write('};\n')

    def writeSourceBottom(self, f) :
        f.write('}\n')
        f.write('\n')

    def genSource(self, absSourcePath) :
        f = open(absSourcePath, 'w')
        self.writeSourceTop(f, absSourcePath)
        self.writeImageData(f)
        self.writeSpriteData(f)
        self.writeSpriteCharMap(f)
        self.writeSourceBottom(f)
        f.close()

    def isDirty(self, pyPath, srcPath, hdrPath, imgPath) :
        # first check version stamps
        if util.fileVersionDirty(srcPath, Version) or util.fileVersionDirty(hdrPath, Version) :
            return True

        # now need to check all used image files for their modified date
        pyTime  = os.path.getmtime(pyPath)
        srcTime = os.path.getmtime(srcPath)    
        hdrTime = os.path.getmtime(hdrPath)
        imgTime = os.path.getmtime(imgPath)
        if pyTime > srcTime or pyTime > hdrTime or imgTime > srcTime or imgTime > hdrTime :
            return True
        return False

    #-------------------------------------------------------------------------------
    def generate(self) :
        selfPath = self.directory + self.name + '.py'
        hdrPath = self.directory + self.name + '.h'
        srcPath = self.directory + self.name + '.cc'
        if self.isDirty(selfPath, hdrPath, srcPath, self.imagePath) :
            self.loadImage()
            print '>> generating {}'.format(hdrPath)
            self.genHeader(hdrPath)
            print '>> generating {}'.format(srcPath)
            self.genSource(srcPath)
        else :
            print '>> nothing to do for {}'.format(selfPath)
