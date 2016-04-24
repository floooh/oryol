//------------------------------------------------------------------------------
//  VertexWriterTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Assets/Gfx/VertexWriter.h"
#include "Core/Memory/Memory.h"

using namespace Oryol;

//------------------------------------------------------------------------------
TEST(VertexWriterTest) {

    uint8_t scratchSpace[1024];
    
    // test non-packed functions
    Memory::Clear(scratchSpace, sizeof(scratchSpace));
    uint8_t* ptr = scratchSpace;
    ptr = VertexWriter::Write(ptr, VertexFormat::Float, 1.0f);
    ptr = VertexWriter::Write(ptr, VertexFormat::Float2, 2.0f, 3.0f);
    ptr = VertexWriter::Write(ptr, VertexFormat::Float3, 4.0f, 5.0f, 6.0f);
    ptr = VertexWriter::Write(ptr, VertexFormat::Float4, 7.0f, 8.0f, 9.0f, 10.0f);
    CHECK(ptr == (scratchSpace + 40));
    const float* fp = (const float*) scratchSpace;
    CHECK(fp[0] == 1.0f);
    CHECK(fp[1] == 2.0f);
    CHECK(fp[2] == 3.0f);
    CHECK(fp[3] == 4.0f);
    CHECK(fp[4] == 5.0f);
    CHECK(fp[5] == 6.0f);
    CHECK(fp[6] == 7.0f);
    CHECK(fp[7] == 8.0f);
    CHECK(fp[8] == 9.0f);
    CHECK(fp[9] == 10.0f);
    
    // test packed-stuff
    Memory::Clear(scratchSpace, sizeof(scratchSpace));
    ptr = scratchSpace;
    
    // test UByte4 packing
    const uint8_t* u8p = ptr;
    ptr = VertexWriter::Write(ptr, VertexFormat::UByte4, 0.0f, 128.5f, -1.0f, 255.5f);
    CHECK(u8p[0] == 0);
    CHECK(u8p[1] == 128);
    CHECK(u8p[2] == 0);
    CHECK(u8p[3] == 255);
    u8p = ptr;
    ptr = VertexWriter::Write(ptr, VertexFormat::UByte4, 64.0f, -64.0f, 2000.0f, -2000.0f);
    CHECK(u8p[0] == 64);
    CHECK(u8p[1] == 0);
    CHECK(u8p[2] == 255);
    CHECK(u8p[3] == 0);
    
    // test UByte4N packing
    u8p = ptr;
    ptr = VertexWriter::Write(ptr, VertexFormat::UByte4N, -1.0f, +1.0f, 0.5f, -32123.0f);
    CHECK(u8p[0] == 0);
    CHECK(u8p[1] == 255);
    CHECK(u8p[2] == 128);
    CHECK(u8p[3] == 0);
    
    // test Byte4N packing
    const int8_t* i8p = (int8_t*) ptr;
    ptr = VertexWriter::Write(ptr, VertexFormat::Byte4, -64.0f, 0.0f, +64.0f, 127.5f);
    CHECK(i8p[0] == -64);
    CHECK(i8p[1] == 0);
    CHECK(i8p[2] == 64);
    CHECK(i8p[3] == 127);
    i8p = (int8_t*) ptr;
    ptr = VertexWriter::Write(ptr, VertexFormat::Byte4, -200.0f, +200.0f, -1.0f, +1.0f);
    CHECK(i8p[0] == -128);
    CHECK(i8p[1] == 127);
    CHECK(i8p[2] == -1);
    CHECK(i8p[3] == 1);
    
    // test Byte4N packing
    i8p = (int8_t*) ptr;
    VertexWriter::Write(ptr, VertexFormat::Byte4N, -1.0f, +1.0f, -0.5f, +0.5f);
    CHECK(i8p[0] == -127);
    CHECK(i8p[1] == 127);
    CHECK(i8p[2] == -64);
    CHECK(i8p[3] == 64);
    
    // test Short4 packing
    Memory::Clear(scratchSpace, sizeof(scratchSpace));
    ptr = scratchSpace;
    const int16_t* i16p = (const int16_t*) ptr;
    ptr = VertexWriter::Write(ptr, VertexFormat::Short4, -40000.0f, +40000.0f, -2000.0f, +2000.0f);
    CHECK(i16p[0] == -32768);
    CHECK(i16p[1] == 32767);
    CHECK(i16p[2] == -2000);
    CHECK(i16p[3] == 2000);
    
    // test Short4N packing
    i16p = (const int16_t*) ptr;
    ptr = VertexWriter::Write(ptr, VertexFormat::Short4N, -2.0f, +2.0f, -1.0f, +1.0f);
    CHECK(i16p[0] == -32767);
    CHECK(i16p[1] == 32767);
    CHECK(i16p[2] == -32767);
    CHECK(i16p[3] == 32767);
    i16p = (const int16_t*) ptr;
    ptr = VertexWriter::Write(ptr, VertexFormat::Short4N, -0.5f, +0.5f, 0.0f, 0.0f);
    CHECK(i16p[0] == -16384);
    CHECK(i16p[1] == 16384);
    CHECK(i16p[2] == 0);
    CHECK(i16p[3] == 0);
    
    // test Short2 packing
    i16p = (const int16_t*) ptr;
    ptr = VertexWriter::Write(ptr, VertexFormat::Short2, -40000.0f, +40000.0f);
    ptr = VertexWriter::Write(ptr, VertexFormat::Short2, -2000.0f, +2000.0f);
    CHECK(i16p[0] == -32768);
    CHECK(i16p[1] == 32767);
    CHECK(i16p[2] == -2000);
    CHECK(i16p[3] == 2000);

    // test Short2N packing
    i16p = (const int16_t*) ptr;
    ptr = VertexWriter::Write(ptr, VertexFormat::Short2N, -2.0f, +2.0f);
    ptr = VertexWriter::Write(ptr, VertexFormat::Short2N, -1.0f, +1.0f);
    ptr = VertexWriter::Write(ptr, VertexFormat::Short2N, -0.5f, +0.5f);
    VertexWriter::Write(ptr, VertexFormat::Short2N, 0.0f, 0.0f);
    CHECK(i16p[0] == -32767);
    CHECK(i16p[1] == 32767);
    CHECK(i16p[2] == -32767);
    CHECK(i16p[3] == 32767);
    CHECK(i16p[4] == -16384);
    CHECK(i16p[5] == 16384);
    CHECK(i16p[6] == 0);
    CHECK(i16p[7] == 0);
}