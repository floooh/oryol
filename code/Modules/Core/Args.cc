//------------------------------------------------------------------------------
//  Args.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Args.h"
#include "Core/String/StringBuilder.h"
#include "Core/String/StringConverter.h"

namespace Oryol {
namespace Core {
    
//------------------------------------------------------------------------------
Args::Args() {
    // empty
}

//------------------------------------------------------------------------------
Args::Args(const String& str) {
    StringBuilder strBuilder(str);
    strBuilder.Tokenize(" \t\r\n", '\"', this->args);
}

//------------------------------------------------------------------------------
Args::Args(const WideString& wideStr) {
    String utf8Str = StringConverter::WideToUTF8(wideStr);
    StringBuilder strBuilder(utf8Str);
    strBuilder.Tokenize(" \t\r\n", '\"', this->args);
}

//------------------------------------------------------------------------------
Args::Args(int argc, const char** argv) {
    for (int32 i = 0; i < argc; i++) {
        this->args.AddBack(argv[i]);
    }
}

//------------------------------------------------------------------------------
bool
Args::HasArg(const String& arg) const {
    return (InvalidIndex != this->args.FindIndexLinear(arg));
}

//------------------------------------------------------------------------------
String
Args::GetString(const String& arg, const String& defaultValue) const {
    int32 index = this->args.FindIndexLinear(arg);
    if ((InvalidIndex != index) && ((index + 1) < this->args.Size())) {
        return this->args[index + 1];
    }
    else {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
int32
Args::GetInt(const String& arg, int32 defaultValue) const {
    int32 index = this->args.FindIndexLinear(arg);
    if ((InvalidIndex != index) && ((index + 1) < this->args.Size())) {
        return StringConverter::FromString<int32>(this->args[index + 1]);
    }
    else {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
float32
Args::GetFloat(const String& arg, float32 defaultValue) const {
    int32 index = this->args.FindIndexLinear(arg);
    if ((InvalidIndex != index) && ((index + 1) < this->args.Size())) {
        return StringConverter::FromString<float32>(this->args[index + 1]);
    }
    else {
        return defaultValue;
    }
}
    
} // namespace Core
} // namespace Oryol