import SpriteSheet as sprites

def generate(directory, name) :

    sheet = sprites.SpriteSheet(directory, name, dict( 
        namespace    = 'Paclone',
        image        = 'sprites.png',
        spriteWidth  = 8,
        spriteHeight = 8,
        sprites      = [
            dict(name='Space',          x=120, y=0, char=' '),
            dict(name='TestWall',       x=112, y=8, char='#'),
            dict(name='Pill',           x=112, y=0, char='.'),
            dict(name='Energizer',      x=0,   y=112, w=16, h=16),

            dict(name='PacmanNoDir',    x=0,   y=16,  w=16, h=16),
            dict(name='PacmanRight',    x=0,   y=16,  w=16, h=16, frames=3, anim='pingpong'),
            dict(name='PacmanDown',     x=48,  y=16,  w=16, h=16, frames=3, anim='pingpong'),
            dict(name='PacmanUp',       x=0,   y=32,  w=16, h=16, frames=3, anim='pingpong'),
            dict(name='PacmanLeft',     x=48,  y=32,  w=16, h=16, frames=3, anim='pingpong'),
            dict(name='PacmanDead',     x=0,   y=152, w=16, h=16, frames=11, anim='clamp'),
    
            dict(name='BlinkyNoDir',    x=0,   y=48,  w=16, h=16),
            dict(name='BlinkyRight',    x=0,   y=48,  w=16, h=16, frames=2, anim='loop'),
            dict(name='BlinkyLeft',     x=32,  y=48,  w=16, h=16, frames=2, anim='loop'),
            dict(name='BlinkyUp',       x=64,  y=48,  w=16, h=16, frames=2, anim='loop'),
            dict(name='BlinkyDown',     x=96,  y=48,  w=16, h=16, frames=2, anim='loop'),

            dict(name='PinkyNoDir',     x=0,   y=64,  w=16, h=16),
            dict(name='PinkyRight',     x=0,   y=64,  w=16, h=16, frames=2, anim='loop'),
            dict(name='PinkyLeft',      x=32,  y=64,  w=16, h=16, frames=2, anim='loop'),
            dict(name='PinkyUp',        x=64,  y=64,  w=16, h=16, frames=2, anim='loop'),
            dict(name='PinkyDown',      x=96,  y=64,  w=16, h=16, frames=2, anim='loop'),

            dict(name='InkyNoDir',      x=0,   y=80,  w=16, h=16),
            dict(name='InkyRight',      x=0,   y=80,  w=16, h=16, frames=2, anim='loop'),
            dict(name='InkyLeft',       x=32,  y=80,  w=16, h=16, frames=2, anim='loop'),
            dict(name='InkyUp',         x=64,  y=80,  w=16, h=16, frames=2, anim='loop'),
            dict(name='InkyDown',       x=96,  y=80,  w=16, h=16, frames=2, anim='loop'),

            dict(name='ClydeNoDir',     x=0,   y=96,  w=16, h=16),
            dict(name='ClydeRight',     x=0,   y=96,  w=16, h=16, frames=2, anim='loop'),
            dict(name='ClydeLeft',      x=32,  y=96,  w=16, h=16, frames=2, anim='loop'),
            dict(name='ClydeUp',        x=64,  y=96,  w=16, h=16, frames=2, anim='loop'),
            dict(name='ClydeDown',      x=96,  y=96,  w=16, h=16, frames=2, anim='loop'),

            dict(name='FrightenedGhost', x=96, y=16,  w=16, h=16, frames=2, anim='loop'),
            dict(name='FlashingGhost',   x=96, y=32,  w=16, h=16, frames=2, anim='loop'),

            dict(name='EyesRight',       x=16, y=112, w=16, h=16),
            dict(name='EyesLeft',        x=32, y=112, w=16, h=16),
            dict(name='EyesUp',          x=48, y=112, w=16, h=16),
            dict(name='EyesDown',        x=64, y=112, w=16, h=16),

            dict(name='DoubleLeft',      x=0,  y=0, char='L'),
            dict(name='DoubleRight',     x=8,  y=0, char='R'),
            dict(name='DoubleUp',        x=16, y=0, char='U'),
            dict(name='DoubleBottom',    x=24, y=0, char='B'),
            dict(name='DoubleUpLeft',    x=32, y=0, char='0'),
            dict(name='DoubleUpRight',   x=40, y=0, char='1'),
            dict(name='DoubleBottomLeft',  x=48, y=0, char='2'),
            dict(name='DoubleBottomRight', x=56, y=0, char='3'),
            dict(name='DoubleTUpLeft',     x=64, y=0, char='4'),
            dict(name='DoubleTUpRight',    x=72, y=0, char='5'),
            dict(name='DoubleTLeftUp',     x=80, y=0, char='6'),
            dict(name='DoubleTLeftDown',   x=80, y=8, char='7'),
            dict(name='DoubleTRightUp',    x=88, y=0, char='8'),
            dict(name='DoubleTRightDown',  x=88, y=8, char='9'),

            dict(name='WallLeft',   x=0,  y=8, char='l'),
            dict(name='WallRight',  x=8,  y=8, char='r'),
            dict(name='WallUp',     x=16, y=8, char='u'),
            dict(name='WallBottom', x=24, y=8, char='b'),

            dict(name='InnerCorner0', x=32, y=8, char='x'),
            dict(name='InnerCorner1', x=40, y=8, char='y'),
            dict(name='InnerCorner2', x=48, y=8, char='z'),
            dict(name='InnerCorner3', x=56, y=8, char='w'),

            dict(name='OuterCorner0', x=96,  y=0, char='e'),
            dict(name='OuterCorner1', x=104, y=0, char='f'),
            dict(name='OuterCorner2', x=96,  y=8, char='g'),
            dict(name='OuterCorner3', x=104, y=8, char='h'),

            dict(name='HouseVert',        x=0, y=128, char='i'),
            dict(name='HouseHori',        x=8, y=128, char='j'),
            dict(name='HouseTopLeft',     x=16, y=128, char='m'),
            dict(name='HouseTopRight',    x=24, y=128, char='n'),
            dict(name='HouseBottomLeft',  x=32, y=128, char='o'),
            dict(name='HouseBottomRight', x=40, y=128, char='p'),
            dict(name='HouseDoor',        x=48, y=128, char='-'),

            dict(name='P200', x=0,  y=136, w=16, h=16),
            dict(name='P400', x=16, y=136, w=16, h=16),
            dict(name='P800', x=32, y=136, w=16, h=16),
            dict(name='P1600', x=48, y=136, w=16, h=16)
        ]))
    sheet.generate()












