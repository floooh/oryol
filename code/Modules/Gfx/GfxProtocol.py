import MessageProtocol as msg

def generate(selfPath, inputs, outputs) :
    msg.generate(selfPath, outputs, dict(
        protocolName='GfxProtocol',
        protocolId='GXPT',
        messages=[
            dict(name='DisplaySetup'),
            dict(name='DisplayDiscarded'),
            dict(name='DisplayModified')
        ]))
