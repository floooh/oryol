import MessageProtocol as msg

def generate(inputs, outputs) :
    msg.generate(inputs, outputs, dict(
        protocolName='GfxProtocol',
        protocolId='GXPT',
        messages=[
            dict(name='DisplaySetup'),
            dict(name='DisplayDiscarded'),
            dict(name='DisplayModified')
        ]))
