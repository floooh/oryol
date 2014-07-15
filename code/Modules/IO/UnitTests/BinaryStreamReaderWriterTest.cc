//------------------------------------------------------------------------------
//  BinaryStreamReaderWriterTest.cc
//  Test URLs
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/Stream/BinaryStreamWriter.h"
#include "IO/Stream/BinaryStreamReader.h"
#include "IO/Stream/MemoryStream.h"

using namespace Oryol;
using namespace Oryol::IO;
using namespace Oryol::Core;

TEST(BinaryStreamReaderWriterTest) {

    // create a memory stream object and attach to BinaryWriter
    Ptr<MemoryStream> stream = MemoryStream::Create();
    CHECK(stream->Open(OpenMode::WriteOnly));
    Ptr<BinaryStreamWriter> writer = BinaryStreamWriter::Create(stream);
    CHECK(writer->Write<bool>(true));
    CHECK(stream->Size() == 1);
    CHECK(stream->GetWritePosition() == 1);
    CHECK(writer->Write<int8>(1));
    CHECK(stream->Size() == 2);
    CHECK(stream->GetWritePosition() == 2);
    CHECK(writer->Write<int16>(2));
    CHECK(stream->Size() == 4);
    CHECK(stream->GetWritePosition() == 4);
    CHECK(writer->Write<int32>(3));
    CHECK(stream->Size() == 8);
    CHECK(stream->GetWritePosition() == 8);
    Array<int32> i32Array;
    for (int i = 0; i < 16; i++) {
        i32Array.AddBack(i);
    }
    CHECK(writer->Write(i32Array));
    CHECK(stream->Size() == 76);
    CHECK(stream->GetWritePosition() == 76);
    
    Array<String> strArray;
    strArray.AddBack("One");
    strArray.AddBack("Two");
    strArray.AddBack("Three");
    CHECK(writer->Write(strArray));
    CHECK(stream->Size() == 103);
    CHECK(stream->GetWritePosition() == 103);
    
    writer = 0;
    stream->Close();
    
    // ...and read the stuff we've just written
    stream->Open(OpenMode::ReadOnly);
    Ptr<BinaryStreamReader> reader = BinaryStreamReader::Create(stream);
    CHECK(stream->GetReadPosition() == 0);
    bool bVal;
    CHECK(reader->Read<bool>(bVal));
    CHECK(stream->GetReadPosition() == 1);
    CHECK(bVal);
    int8 i8Val;
    CHECK(reader->Read<int8>(i8Val));
    CHECK(stream->GetReadPosition() == 2);
    CHECK(1 == i8Val);
    int16 i16Val;
    CHECK(reader->Read<int16>(i16Val));
    CHECK(stream->GetReadPosition() == 4);
    CHECK(2 == i16Val);
    int32 i32Val;
    CHECK(reader->Read<int32>(i32Val));
    CHECK(stream->GetReadPosition() == 8);
    CHECK(3 == i32Val);
    
    i32Array.Clear();
    CHECK(reader->Read(i32Array));
    CHECK(stream->GetReadPosition() == 76);
    for (int i = 0; i < 16; i++) {
        CHECK(i32Array[i] == i);
    }
    
    strArray.Clear();
    CHECK(reader->Read(strArray));
    CHECK(stream->GetReadPosition() == 103);
    CHECK(strArray.Size() == 3);
    CHECK(strArray[0] == "One");
    CHECK(strArray[1] == "Two");
    CHECK(strArray[2] == "Three");
    
    reader->DetachStream();
    stream->Close();
    stream = 0;
    reader = 0;
}
