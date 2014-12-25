'''
Code generator for 'sound sheets' (audio wave forms and sound effects)
'''
import genutil as util
import png
import os

Version = 1

#-------------------------------------------------------------------------------
class Wave :
    def __init__(self, name, x, y, w, h) :
        self.name = name
        self.x = x
        self.y = y
        self.w = w
        self.h = h

#-------------------------------------------------------------------------------
class SoundSheet :
    def __init__(self, directory, name) :
        self.directory = directory
        self.name = name
        self.ns = ''
        self.imagePath = ''
        self.imageWidth = 0
        self.imageHeight = 0
        self.imagePixels = None
        self.imageInfo = None
        self.waves = []
        self.effects = []

    def namespace(self, ns) :
        self.ns = ns

    def image(self, img) :
        self.imagePath = self.directory + img

    def wave(self, name, x, y, w, h) :
        wave = Wave(name, x, y, w, h)
        self.waves.append(wave)

    def loadImage(self) :
        pngReader = png.Reader(self.imagePath)
        img = pngReader.asRGBA8()
        self.imageWidth = img[0]
        self.imageHeight = img[1]
        self.imagePixels = img[2]
        self.imageInfo = img[3]

    def generate(self) :
        selfPath = self.directory + self.name + '.py'
        hdrPath = self.directory + self.name + '.h'
        srcPath = self.directory + self.name + '.cc'
        if util.isDirty([selfPath, self.imagePath], Version, hdrPath, srcPath) :
            self.loadImage()
            self.genHeader(hdrPath)
            self.genSource(srcPath)
