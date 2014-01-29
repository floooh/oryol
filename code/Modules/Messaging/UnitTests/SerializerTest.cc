//------------------------------------------------------------------------------
//  SerializerTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Messaging/Serializer.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::Messaging;

// a POD struct
struct podStruct {
    int32 bla;
    int8 blub;
    int16 blob;
};

TEST(SerializerTest) {
    
    // scratch space
    uchar space[256];
    const uchar* decodePtr = (const uchar*) space;
    uchar* encodePtr = space;
    
    // int32
    const int32 i32Write = 23;
    int32 i32Read;
    CHECK(Serializer<int32>::SizeOf(i32Write) == sizeof(int32));
    CHECK(Serializer<int32>::Encode(i32Write, encodePtr, sizeof(space)) == space + sizeof(int32));
    CHECK(Serializer<int32>::Decode(decodePtr, sizeof(space), i32Read) == space + sizeof(int32));
    CHECK(23 == i32Read);
    
    // bool
    const bool bWrite = true;
    bool bRead;
    CHECK(Serializer<bool>::SizeOf(bWrite) == sizeof(bool));
    CHECK(Serializer<bool>::Encode(bWrite, encodePtr, sizeof(space)) == space + sizeof(bool));
    CHECK(Serializer<bool>::Decode(decodePtr, sizeof(space), bRead) == space + sizeof(bool));
    
    // uint64
    const uint64 u64Write = 123;
    uint64 u64Read;
    CHECK(Serializer<uint64>::SizeOf(u64Write) == sizeof(uint64));
    CHECK(Serializer<uint64>::Encode(u64Write, encodePtr, sizeof(space)) == space + sizeof(uint64));
    CHECK(Serializer<uint64>::Decode(decodePtr, sizeof(space), u64Read) == space + sizeof(uint64));
    
    // test POD struct
    const podStruct podWrite = { 1, 2, 3 };
    podStruct podRead;
    CHECK(Serializer<podStruct>::SizeOf(podWrite) == sizeof(podStruct));
    CHECK(Serializer<podStruct>::Encode(podWrite, encodePtr, sizeof(space)) == space + sizeof(podStruct));
    CHECK(Serializer<podStruct>::Decode(decodePtr, sizeof(space), podRead) == space + sizeof(podStruct));
    CHECK(podWrite.bla == podRead.bla && podWrite.blub == podRead.blub && podWrite.blob == podRead.blob);
    
    // test Core::String
    const String strWrite("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    String strRead;
    CHECK(Serializer<String>::SizeOf(strWrite) == 30);
    CHECK(Serializer<String>::Encode(strWrite, encodePtr, sizeof(space)) == space + 30);
    CHECK(Serializer<String>::Decode(decodePtr, sizeof(space), strRead) == space + 30);
    CHECK(strWrite == strRead);
    
    // test Core::StringAtom
    const StringAtom strAtomWrite("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    StringAtom strAtomRead;
    CHECK(Serializer<StringAtom>::SizeOf(strAtomWrite) == 30);
    CHECK(Serializer<StringAtom>::Encode(strAtomWrite, encodePtr, sizeof(space)) == space + 30);
    CHECK(Serializer<StringAtom>::Decode(decodePtr, sizeof(space), strAtomRead) == space + 30);
    CHECK(strWrite == strRead);
}


