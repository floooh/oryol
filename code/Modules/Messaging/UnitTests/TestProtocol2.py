import MessageProtocol as msg

def generate(directory, name) :
    msg.generate(directory, name, dict(
        protocolName='TestProtocol2',
        protocolId='TSP2',
        parentProtocol='TestProtocol',
        parentProtocolHeader='Messaging/UnitTests/TestProtocol.h',
        messages=[
            dict(name='TestMsgEx', parent='TestProtocol::TestMsg1', attrs=[
                dict(name='ExVal2', type='int8'),
            ])
        ]))
