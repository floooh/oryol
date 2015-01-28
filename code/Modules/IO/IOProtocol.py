import MessageProtocol as msg

def generate(selfPath, inputs, outputs) :
    msg.generate(selfPath, outputs, dict(
        protocolName='IOProtocol',
        protocolId='IOPT',
        headers=[
            'Core/Ptr.h',
            'IO/Core/URL.h',
            'IO/Core/IOStatus.h',
            'IO/Stream/MemoryStream.h'],
        messages=[
            dict(name='Request', attrs=[ 
                dict(name='URL', type='URL'),
                dict(name='Lane', type='int32'),
                dict(name='CacheReadEnabled', type='bool'),
                dict(name='CacheWriteEnabled', type='bool'),
                dict(name='Status', type='IOStatus::Code', default='IOStatus::InvalidIOStatus', dir='out'),
                dict(name='ErrorDesc', type='String', dir='out'),
                dict(name='Stream', type='Ptr<MemoryStream>', dir='out'),
                dict(name='StartOffset', type='int32', default='0'),
                dict(name='EndOffset', type='int32', default='0')]),
            dict(name='notifyLanes', attrs=[
                dict(name='Scheme', type='StringAtom')]),
            dict(name='notifyFileSystemRemoved', parent='notifyLanes'),
            dict(name='notifyFileSystemReplaced', parent='notifyLanes'),
            dict(name='notifyFileSystemAdded', parent='notifyLanes')
            ]))
