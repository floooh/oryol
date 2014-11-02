import MessageProtocol as msg

def generate(directory, name) :
    msg.generate(directory, name, dict(
        protocolName='TestProtocol',
        protocolId='TSTP',
        headers=[
            'Core/String/String.h',
            'Core/String/StringAtom.h',
            'Core/Containers/Array.h'
        ],
        messages=[
            dict(name='TestMsg1', attrs=[
                dict(name='Int8Val', type='int8'),
                dict(name='Int16Val', type='int16', default='-1'),
                dict(name='Int32Val', type='int32'),
                dict(name='Int64Val', type='int64'),
                dict(name='UInt8Val', type='uint8'),
                dict(name='UInt16Val', type='uint16'),
                dict(name='UInt32Val', type='uint32'),
                dict(name='UInt64Val', type='uint64'),
                dict(name='Float32Val', type='float32', default='123.0f'),
                dict(name='Float64Val', type='float64', default='12.0')
                ]),
            dict(name='TestMsg2', parent='TestMsg1', attrs=[
                dict(name='StringVal', type='String', default='"Test"'),
                dict(name='StringAtomVal', type='StringAtom')
                ]),
            dict(name='TestArrayMsg', attrs=[
                dict(name='Int32ArrayVal', type='Array<int32>'),
                dict(name='StringArrayVal', type='Array<String>')
                ])
        ]))
