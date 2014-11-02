import MessageProtocol as msg

def generate(directory, name) :
    msg.generate(directory, name, dict(
        protocolName='GfxProtocol',
        protocolId='GXPT',
        messages=[
            dict(name='DisplaySetup'),
            dict(name='DisplayDiscarded'),
            dict(name='DisplayModified')
        ]))
