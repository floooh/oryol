//------------------------------------------------------------------------------
//  WideString.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "WideString.h"
#include "Core/Memory/Memory.h"

namespace Oryol {
namespace Core {

const wchar_t* WideString::emptyString = L"";
    
//------------------------------------------------------------------------------
void
WideString::destroy() {
    o_assert(0 != this->data);
    o_assert(0 == this->data->refCount);
    this->data->~StringData();
    Memory::Free(this->data);
    this->data = 0;
}

//------------------------------------------------------------------------------
void
WideString::create(const wchar_t* ptr, int32 numChars) {
    o_assert(0 != ptr);
    if ((ptr[0] != 0) && (numChars > 0)) {
        this->data = (StringData*) Memory::Alloc(sizeof(StringData) + ((numChars + 1) * sizeof(wchar_t)));
        new(this->data) StringData();
        this->addRef();
        this->data->length = numChars;
        wchar_t* strPtr = (wchar_t*) &(this->data[1]);
        Memory::Copy(ptr, strPtr, numChars * sizeof(wchar_t));
        strPtr[numChars] = 0;
    }
    else {
        // empty string, don't bother to allocate storage for this
        this->data = nullptr;
    }
}

    
} // namespace Core
} // namespace Oryol