import MessageProtocol as msg

def generate(directory, name) :
    msg.generate(directory, name, dict(
        protocolName='HTTPProtocol',
        protocolId='HTPR',
        headers=[
            'IO/Core/URL.h',
            'HTTP/HTTPMethod.h',
            'Core/Containers/Map.h',
            'Core/String/String.h',
            'IO/Stream/Stream.h',
            'IO/Core/IOStatus.h',
            'IO/IOProtocol.h'
        ],
        messages=[
            dict(name='HTTPResponse', attrs=[
                dict(name='Status', type='IOStatus::Code', default='IOStatus::InvalidIOStatus'),
                dict(name='ResponseHeaders', type='Map<String,String>'),
                dict(name='Body', type='Ptr<Stream>'),
                dict(name='ErrorDesc', type='String')
            ]),
            dict(name='HTTPRequest', attrs=[
                dict(name='Method', type='HTTPMethod::Code', default='HTTPMethod::Get'),
                dict(name='URL', type='URL'),
                dict(name='RequestHeaders', type='Map<String,String>'),
                dict(name='Body', type='Ptr<Stream>'),
                dict(name='IoRequest', type='Ptr<IOProtocol::Request>'),
                dict(name='Response', type='Ptr<HTTPProtocol::HTTPResponse>', dir='out')
            ]),
        ]))
