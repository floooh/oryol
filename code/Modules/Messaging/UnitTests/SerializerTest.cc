//------------------------------------------------------------------------------
//  SerializerTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Messaging/Serializer.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"

using namespace Oryol;

// a POD struct
struct podStruct {
    int32 bla;
    int8 blub;
    int16 blob;
};

TEST(SerializerTest) {
    
    // scratch space
    uint8 space[256];
    const uint8* decodePtr = (const uint8*) space;
    uint8* encodePtr = space;
    const uint8* maxPtr = decodePtr + sizeof(space);
    
    // int32
    const int32 i32Write = 23;
    int32 i32Read;
    CHECK(Serializer::EncodedSize<int32>(i32Write) == sizeof(int32));
    CHECK(Serializer::Encode<int32>(i32Write, encodePtr, maxPtr) == space + sizeof(int32));
    CHECK(Serializer::Decode<int32>(decodePtr, maxPtr, i32Read) == space + sizeof(int32));
    CHECK(23 == i32Read);
    
    // bool
    const bool bWrite = true;
    bool bRead;
    CHECK(Serializer::EncodedSize<bool>(bWrite) == sizeof(bool));
    CHECK(Serializer::Encode<bool>(bWrite, encodePtr, maxPtr) == space + sizeof(bool));
    CHECK(Serializer::Decode<bool>(decodePtr, maxPtr, bRead) == space + sizeof(bool));
    
    // uint64
    const uint64 u64Write = 123;
    uint64 u64Read;
    CHECK(Serializer::EncodedSize<uint64>(u64Write) == sizeof(uint64));
    CHECK(Serializer::Encode<uint64>(u64Write, encodePtr, maxPtr) == space + sizeof(uint64));
    CHECK(Serializer::Decode<uint64>(decodePtr, maxPtr, u64Read) == space + sizeof(uint64));
    
    // test POD struct
    const podStruct podWrite = { 1, 2, 3 };
    podStruct podRead;
    CHECK(Serializer::EncodedSize<podStruct>(podWrite) == sizeof(podStruct));
    CHECK(Serializer::Encode<podStruct>(podWrite, encodePtr, maxPtr) == space + sizeof(podStruct));
    CHECK(Serializer::Decode<podStruct>(decodePtr, maxPtr, podRead) == space + sizeof(podStruct));
    CHECK(podWrite.bla == podRead.bla && podWrite.blub == podRead.blub && podWrite.blob == podRead.blob);
    
    // test String
    const String strWrite("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    String strRead;
    CHECK(Serializer::EncodedSize<String>(strWrite) == 30);
    CHECK(Serializer::Encode<String>(strWrite, encodePtr, maxPtr) == space + 30);
    CHECK(Serializer::Decode<String>(decodePtr, maxPtr, strRead) == space + 30);
    CHECK(strWrite == strRead);
    
    // test StringAtom
    const StringAtom strAtomWrite("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    StringAtom strAtomRead;
    CHECK(Serializer::EncodedSize<StringAtom>(strAtomWrite) == 30);
    CHECK(Serializer::Encode<StringAtom>(strAtomWrite, encodePtr, maxPtr) == space + 30);
    CHECK(Serializer::Decode<StringAtom>(decodePtr, maxPtr, strAtomRead) == space + 30);
    CHECK(strWrite == strRead);
    
    // test Array of int32
    Array<int32> int32ArrayWrite;
    int32ArrayWrite.Add(1);
    int32ArrayWrite.Add(2);
    int32ArrayWrite.Add(3);
    Array<int32> int32ArrayRead;
    CHECK(Serializer::EncodedArraySize<int32>(int32ArrayWrite) == 16);
    CHECK(Serializer::EncodeArray<int32>(int32ArrayWrite, encodePtr, maxPtr) == space + 16);
    CHECK(Serializer::DecodeArray<int32>(decodePtr, maxPtr, int32ArrayRead) == space + 16);
    CHECK(1 == int32ArrayRead[0]);
    CHECK(2 == int32ArrayRead[1]);
    CHECK(3 == int32ArrayRead[2]);
}


