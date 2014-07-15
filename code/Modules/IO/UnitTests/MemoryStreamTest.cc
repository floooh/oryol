//------------------------------------------------------------------------------
//  MemoryStreamTest.cc
//  Test URLs
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/Stream/MemoryStream.h"

#include <cstring>

using namespace Oryol;
using namespace Oryol::Core;
using namespace Oryol::IO;

TEST(MemoryStreamTest) {

    const char* data = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int32 dataSize = 26;
    
    // create empty stream
    Ptr<MemoryStream> stream0 = MemoryStream::Create();
    CHECK(stream0->GetURL().Empty());
    CHECK(!stream0->IsOpen());
    CHECK(stream0->GetOpenMode() == OpenMode::InvalidOpenMode);
    CHECK(stream0->Size() == 0);
    CHECK(stream0->GetWritePosition() == 0);
    CHECK(stream0->GetReadPosition() == 0);
    CHECK(!stream0->IsWriteMapped());
    CHECK(!stream0->IsReadMapped());
    CHECK(stream0->IsEndOfStream());
    CHECK(stream0->GetMinGrow() == ORYOL_STREAM_DEFAULT_MIN_GROW);
    CHECK(stream0->GetMaxGrow() == ORYOL_STREAM_DEFAULT_MAX_GROW);
    CHECK(stream0->Capacity() == 0);
    CHECK(stream0->Size() == 0);
    
    // open for writing
    CHECK(stream0->Open(OpenMode::WriteOnly));
    CHECK(stream0->GetOpenMode() == OpenMode::WriteOnly);
    CHECK(stream0->IsOpen());
    CHECK(stream0->Size() == 0);
    CHECK(stream0->Capacity() == 0);
    
    // write data
    CHECK(dataSize == stream0->Write(data, dataSize));
    CHECK(stream0->Size() == dataSize);
    CHECK(stream0->Capacity() == ORYOL_STREAM_DEFAULT_MIN_GROW);
    CHECK(stream0->GetWritePosition() == dataSize);
    
    // close the stream
    stream0->Close();
    CHECK(stream0->GetOpenMode() == OpenMode::InvalidOpenMode);
    CHECK(!stream0->IsOpen());
    CHECK(stream0->Size() == dataSize);
    
    // open same stream for reading
    CHECK(stream0->Open(OpenMode::ReadOnly));
    CHECK(stream0->GetOpenMode() == OpenMode::ReadOnly);
    CHECK(stream0->Size() == dataSize);
    CHECK(stream0->GetReadPosition() == 0);
    
    // read data in 4-byte chunks
    char readData[2 * dataSize];
    char* ptr = readData;
    do {
        int32 bytesRead = stream0->Read(ptr, 4);
        ptr += bytesRead;
    }
    while (!stream0->IsEndOfStream());
    *ptr = 0;
    CHECK(stream0->GetReadPosition() == dataSize);
    CHECK(0 == std::strcmp(readData, data));
    
    // close the stream and re-open in read/write/append mode
    stream0->Close();
    CHECK(stream0->Open(OpenMode::ReadWriteAppend));
    CHECK(stream0->GetOpenMode() == OpenMode::ReadWriteAppend);
    CHECK(stream0->Size() == dataSize);
    CHECK(stream0->GetReadPosition() == 0);
    CHECK(stream0->GetWritePosition() == dataSize);
    
    // append data
    CHECK(dataSize == stream0->Write(data, dataSize));
    CHECK(stream0->Size() == 2 * dataSize);
    CHECK(stream0->GetWritePosition() == 2 * dataSize);
    
    // read everything
    CHECK(stream0->Read(readData, sizeof(readData)) == 2 * dataSize);
    CHECK(std::memcmp(data, readData, dataSize) == 0);
    CHECK(std::memcmp(data, &readData[dataSize], dataSize) == 0);
    CHECK(stream0->GetReadPosition() == 2 * dataSize);
    CHECK(stream0->IsEndOfStream());
    
    // try to read past end-of-stream
    Memory::Clear(readData, sizeof(readData));
    stream0->SetReadPosition(dataSize);
    CHECK(stream0->GetReadPosition() == dataSize);
    CHECK(stream0->Read(readData, 123456) == dataSize);
    CHECK(std::memcmp(data, readData, dataSize) == 0);
    CHECK(readData[dataSize + 1] == 0);
    CHECK(stream0->GetReadPosition() == 2 * dataSize);
    CHECK(stream0->IsEndOfStream());
    
    /// @todo: test reading and MapRead with EndOfStream
    
    /// @todo: test with small initial capacity and small min/max grow
    Ptr<MemoryStream> stream1 = MemoryStream::Create(4, 4, 8);
}
