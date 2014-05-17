'''
Code generator for message protocol xml files.
'''

import os
import sys
import util

Version = 2

#-------------------------------------------------------------------------------
def checkValidAttr(attr) :
    for key in attr.keys() :
        if not key in ('name', 'type', 'def', 'dir') :
            util.error('Invalid Attr attr "{}"'.format(key))
    
#-------------------------------------------------------------------------------
def writeHeaderTop(f, xmlRoot) :
    '''
        Write header area for the generated C++ header.
    '''
    f.write('#pragma once\n')
    f.write('//-----------------------------------------------------------------------------\n')
    f.write('/* #version:{}#\n'.format(Version))
    f.write('    machine generated, do not edit!\n')
    f.write('*/\n')
    f.write('#include <cstring>\n')

#-------------------------------------------------------------------------------
def writeIncludes(f, xmlRoot) :
    '''
        Write include statements in the generated C++ header.
    '''
    f.write('#include "Messaging/Message.h"\n')
    f.write('#include "Messaging/Serializer.h"\n')
    parentHdr = xmlRoot.get('parenthdr', 'Messaging/Protocol.h')
    f.write('#include "{}"\n'.format(parentHdr))

    for hdr in xmlRoot.findall('Header') :
        f.write('#include "{}"\n'.format(hdr.get('path')))
    f.write('\n')

#-------------------------------------------------------------------------------
def writeProtocolMethods(f, xmlRoot) :
    '''
    Write the protocol methods
    '''
    f.write('    static Messaging::ProtocolIdType GetProtocolId() {\n')
    f.write("        return '{}';\n".format(xmlRoot.get('id')))
    f.write('    };\n')

#-------------------------------------------------------------------------------
def writeMessageIdEnum(f, xmlRoot) :
    '''
    Write the enum with message ids
    '''
    protocol = xmlRoot.get('name')
    parentProtocol = xmlRoot.get('parent', 'Messaging::Protocol')

    f.write('    class MessageId {\n')
    f.write('    public:\n')
    f.write('        enum {\n')
    msgCount = 0
    for msg in xmlRoot.findall('Message') :
        if msgCount == 0:
            f.write('            ' + msg.get('name') + 'Id = ' + parentProtocol + '::MessageId::NumMessageIds, \n')
        else :
            f.write('            ' + msg.get('name') + 'Id,\n')
        msgCount += 1
    f.write('            NumMessageIds\n')
    f.write('        };\n')
    f.write('        static const char* ToString(Messaging::MessageIdType c) {\n')
    f.write('            switch (c) {\n')
    for msg in xmlRoot.findall('Message') :
        msgName = msg.get('name') + 'Id'
        f.write('                case ' + msgName + ': return "' + msgName + '";\n')
    f.write('                default: return "InvalidMessageId";\n')
    f.write('            }\n')
    f.write('        };\n')
    f.write('        static Messaging::MessageIdType FromString(const char* str) {\n')
    for msg in xmlRoot.findall('Message') :
        msgName = msg.get('name') + 'Id'
        f.write('            if (std::strcmp("' + msgName + '", str) == 0) return ' + msgName + ';\n')
    f.write('            return Messaging::InvalidMessageId;\n')
    f.write('        };\n')
    f.write('    };\n')
    f.write('    typedef Core::Ptr<Messaging::Message> (*CreateCallback)();\n')
    f.write('    static CreateCallback jumpTable[' + protocol + '::MessageId::NumMessageIds];\n')

#-------------------------------------------------------------------------------
def writeFactoryClassDecl(f, xmlRoot) :
    '''
    Writes the message factory for this protocol
    '''
    f.write('    class Factory {\n')
    f.write('    public:\n')
    f.write('        static Core::Ptr<Messaging::Message> Create(Messaging::MessageIdType id);\n')
    f.write('    };\n')

#-------------------------------------------------------------------------------
def writeFactoryClassImpl(f, xmlRoot) :
    '''
    Writes the factory class implementation
    '''
    protocol = xmlRoot.get('name')
    parentProtocol = xmlRoot.get('parent', 'Messaging::Protocol')

    f.write(protocol + '::CreateCallback ' + protocol + '::jumpTable[' + protocol + '::MessageId::NumMessageIds] = { \n') 
    for msg in xmlRoot.findall('Message') :
        f.write('    &' + protocol + '::' + msg.get('name') + '::FactoryCreate,\n')
    f.write('};\n')
    f.write('Core::Ptr<Messaging::Message>\n')
    f.write(protocol + '::Factory::Create(Messaging::MessageIdType id) {\n')
    f.write('    if (id < ' + parentProtocol + '::MessageId::NumMessageIds) {\n')
    f.write('        return ' + parentProtocol + '::Factory::Create(id);\n')
    f.write('    }\n')
    f.write('    else {\n')
    f.write('        o_assert(id < ' + protocol + '::MessageId::NumMessageIds);\n')
    f.write('        return jumpTable[id - ' + parentProtocol + '::MessageId::NumMessageIds]();\n')
    f.write('    };\n')
    f.write('}\n')

#-------------------------------------------------------------------------------
def getAttrDefaultValue(attr) :
    '''
    Get the default value for a given attribute
    '''
    defValue = attr.get('def')
    attrType = attr.get('type')
    if attrType in ('int8', 'int16', 'int32', 'int64', 'uint8', 'uint16', 'uint32', 'uint64') :
        if not defValue :
            defValue = '0'
    elif attrType in ('char', 'unsigned char', 'int', 'unsigned int', 'short', 'unsigned short', 'long', 'unsigned long') :
        if not defValue :
            defValue = '0'
    elif attrType == 'bool' :
        if not defValue :
            defValue = 'false'
    elif attrType in ('float32', 'float') :
        if not defValue :
            defValue = '0.0f'
    elif attrType in ('float64', 'double') :
        if not defValue :
            defValue = '0.0'
    return defValue;

#-------------------------------------------------------------------------------
def getRefType(attrType) :
    '''
    Get the reference type string for an attribute type
    ''' 
    if attrType in ('int8', 'int16', 'int32', 'int64', 'uint8', 'uint16', 'uint32', 'uint64') :
        return attrType
    elif attrType in ('bool', 'char', 'unsigned char', 'int', 'unsigned int', 'short', 'unsigned short', 'long', 'unsigned long') :
        return attrType
    elif attrType in ('float32', 'float') :
        return attrType
    elif attrType in ('float64', 'double') :
        return attrType
    else :
        return 'const ' + attrType + '&'

#-------------------------------------------------------------------------------
def getValueType(attrType) :
    '''
    Get the value type string for an attribute type
    ''' 
    return attrType

#-------------------------------------------------------------------------------
def isArrayType(attrType) :
    '''
    Test if the type string is an array type (Core::Array<TYPE>)
    '''
    return attrType.startswith('Core::Array<') and attrType.endswith('>')

#-------------------------------------------------------------------------------
def getArrayType(attrType) :
    '''
    Get the element type of an array type.
    '''
    # strip the 'Core::Array<' at the left, and the '>' at the right
    return attrType[12:-1]

#-------------------------------------------------------------------------------
def writeMessageClasses(f, xmlRoot) :
    '''
    Write the message classes to the generated C++ header
    '''
    protocolId = xmlRoot.get('id')
    for msg in xmlRoot.findall('Message') :
        msgClassName = msg.get('name')
        msgParentClassName = msg.get('parent', 'Messaging::Message')
        f.write('    class ' + msgClassName + ' : public ' + msgParentClassName + ' {\n')
        f.write('        OryolClassPoolAllocDecl(' + msgClassName + ');\n')
        f.write('    public:\n')

        # write constructor
        f.write('        ' + msgClassName + '() {\n')
        f.write('            this->msgId = MessageId::' + msgClassName + 'Id;\n')
        for attr in msg.findall('Attr') :
            attrName = attr.get('name').lower()
            defValue = getAttrDefaultValue(attr)
            if defValue :
                f.write('            this->' + attrName + ' = ' + defValue + ';\n')
        f.write('        };\n')

        # special factory create method
        f.write('        static Core::Ptr<Messaging::Message> FactoryCreate() {\n')
        f.write('            return Create();\n')
        f.write('        };\n')

        # special class message id static method
        f.write('        static Messaging::MessageIdType ClassMessageId() {\n')
        f.write('            return MessageId::' + msgClassName + 'Id;\n')
        f.write('        };\n')

        # virtual method which checks whether the method belongs to a protocol
        f.write('        virtual bool IsMemberOf(Messaging::ProtocolIdType protId) const {\n')
        f.write("            if (protId == '" + protocolId + "') return true;\n")
        f.write('            else return ' + msgParentClassName + '::IsMemberOf(protId);\n')
        f.write('        };\n') 

        # write serializer methods
        if msg.get('serialize', 'true') == 'true' :
            f.write('        virtual int32 EncodedSize() const override;\n')
            f.write('        virtual uint8* Encode(uint8* dstPtr, const uint8* maxValidPtr) const override;\n')
            f.write('        virtual const uint8* Decode(const uint8* srcPtr, const uint8* maxValidPtr) override;\n')

        # write setters/getters
        for attr in msg.findall('Attr') :
            checkValidAttr(attr)
            attrName = attr.get('name')
            attrType = attr.get('type')
            f.write('        void Set' + attrName + '(' + getRefType(attrType) + ' val) {\n')
            f.write('            this->' + attrName.lower() + ' = val;\n')
            f.write('        };\n')
            f.write('        ' + getRefType(attrType) + ' Get' + attrName + '() const {\n')
            f.write('            return this->' + attrName.lower() + ';\n')
            f.write('        };\n')

        # write members
        f.write('private:\n')
        for attr in msg.findall('Attr') :
            attrName = attr.get('name').lower()
            attrType = attr.get('type')
            f.write('        ' + getValueType(attrType) + ' ' + attrName + ';\n')
        f.write('    };\n')

#-------------------------------------------------------------------------------
def writeSerializeMethods(f, xmlRoot) :
    '''
    Writes the serializer methods of the message to the source file.
    '''
    for msg in xmlRoot.findall('Message') : 
        if msg.get('serialize', 'true') == 'true' :   
            protocol = xmlRoot.get('name')
            msgClassName = msg.get('name')
            msgParentClassName = msg.get('parent', 'Messaging::Message')

            # EncodedSize()
            f.write('int32 ' + protocol + '::' + msgClassName + '::EncodedSize() const {\n')
            f.write('    int32 s = ' + msgParentClassName + '::EncodedSize();\n')
            for attr in msg.findall('Attr') :
                attrName = attr.get('name').lower()
                attrType = attr.get('type')
                if isArrayType(attrType) :
                    elmType = getArrayType(attrType)                
                    f.write('    s += Messaging::Serializer::EncodedArraySize<' + elmType + '>(this->' + attrName + ');\n')
                else :
                    f.write('    s += Messaging::Serializer::EncodedSize<' + attrType + '>(this->' + attrName + ');\n')                
            f.write('    return s;\n')
            f.write('}\n')

            # Encode
            # FIXME: I think we need to diffentiate between "send" and "receive" attributes!
            # ... so: EncodeSend/DecodeSend, EncodeReceive/DecodeReceive
            f.write('uint8* ' + protocol + '::' + msgClassName + '::Encode(uint8* dstPtr, const uint8* maxValidPtr) const {\n')
            f.write('    dstPtr = ' + msgParentClassName + '::Encode(dstPtr, maxValidPtr);\n')
            for attr in msg.findall('Attr') :
                attrName = attr.get('name').lower()
                attrType = attr.get('type')
                if isArrayType(attrType) :
                    elmType = getArrayType(attrType)                
                    f.write('    dstPtr = Messaging::Serializer::EncodeArray<' + elmType + '>(this->' + attrName + ', dstPtr, maxValidPtr);\n')
                else :            
                    f.write('    dstPtr = Messaging::Serializer::Encode<' + attrType + '>(this->' + attrName + ', dstPtr, maxValidPtr);\n')
            f.write('    return dstPtr;\n')
            f.write('}\n')

            # Decode
            f.write('const uint8* ' + protocol + '::' + msgClassName + '::Decode(const uint8* srcPtr, const uint8* maxValidPtr) {\n')
            f.write('    srcPtr = ' + msgParentClassName + '::Decode(srcPtr, maxValidPtr);\n')
            for attr in msg.findall('Attr') :
                attrName = attr.get('name').lower()
                attrType = attr.get('type')
                if isArrayType(attrType) :
                    elmType = getArrayType(attrType)
                    f.write('    srcPtr = Messaging::Serializer::DecodeArray<' + elmType + '>(srcPtr, maxValidPtr, this->' + attrName + ');\n')
                else :
                    f.write('    srcPtr = Messaging::Serializer::Decode<' + attrType + '>(srcPtr, maxValidPtr, this->' + attrName + ');\n')
            f.write('    return srcPtr;\n')
            f.write('}\n')            

#-------------------------------------------------------------------------------
def generateHeader(xmlTree, absHeaderPath) :
    '''
    Generate the C++ header file 
    '''
    xmlRoot = xmlTree.getroot()
    f = open(absHeaderPath, 'w')

    nameSpace = xmlRoot.get('ns')
    protocol = xmlRoot.get('name')
    
    writeHeaderTop(f, xmlRoot) 
    writeIncludes(f, xmlRoot)
    f.write('namespace Oryol {\n')
    f.write('namespace ' + nameSpace + ' {\n')
    f.write('class ' + protocol + ' {\n')
    f.write('public:\n')
    writeProtocolMethods(f, xmlRoot)
    writeMessageIdEnum(f, xmlRoot)
    writeFactoryClassDecl(f, xmlRoot)
    writeMessageClasses(f, xmlRoot)
    f.write('};\n')
    f.write('}\n')
    f.write('}\n')
    f.close()

#-------------------------------------------------------------------------------
def writeSourceTop(f, xmlRoot, absSourcePath) :
    '''
    Write source file header area
    '''
    path, hdrFileAndExt = os.path.split(absSourcePath)
    hdrFile, ext = os.path.splitext(hdrFileAndExt)

    f.write('//-----------------------------------------------------------------------------\n')
    f.write('// #version:{}# machine generated, do not edit!\n'.format(Version))
    f.write('//-----------------------------------------------------------------------------\n')
    f.write('#include "Pre.h"\n')
    f.write('#include "' + hdrFile + '.h"\n')
    f.write('\n')

#-------------------------------------------------------------------------------
def generateSource(xmlTree, absSourcePath) :
    '''
    Generate the C++ source file
    '''
    xmlRoot = xmlTree.getroot()
    nameSpace = xmlRoot.get('ns')
    protocol = xmlRoot.get('name')

    f = open(absSourcePath, 'w')
    writeSourceTop(f, xmlRoot, absSourcePath)
    f.write('namespace Oryol {\n')
    f.write('namespace ' + nameSpace + ' {\n')
    for msg in xmlRoot.findall('Message') :
        msgClassName = msg.get('name')
        f.write('OryolClassPoolAllocImpl(' + protocol + '::' + msgClassName + ');\n')
        
    writeFactoryClassImpl(f, xmlRoot)
    writeSerializeMethods(f, xmlRoot)
    f.write('}\n')
    f.write('}\n')
    f.close()

#-------------------------------------------------------------------------------
def isDirty(xmlTree, absXmlPath, absSourcePath, absHeaderPath) :
    return util.fileVersionDirty(absSourcePath, Version) or util.fileVersionDirty(absHeaderPath, Version)

#-------------------------------------------------------------------------------
def generate(xmlTree, absXmlPath, absSourcePath, absHeaderPath) :
    generateHeader(xmlTree, absHeaderPath)
    generateSource(xmlTree, absSourcePath)
