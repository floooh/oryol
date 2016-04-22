//------------------------------------------------------------------------------
//  Args.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Args.h"
#include "Core/String/StringBuilder.h"
#include "Core/String/StringConverter.h"

namespace Oryol {
    
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
    for (int i = 0; i < argc; i++) {
        this->args.Add(argv[i]);
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
    int index = this->args.FindIndexLinear(arg);
    if ((InvalidIndex != index) && ((index + 1) < this->args.Size())) {
        return this->args[index + 1];
    }
    else {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
int
Args::GetInt(const String& arg, int defaultValue) const {
    int index = this->args.FindIndexLinear(arg);
    if ((InvalidIndex != index) && ((index + 1) < this->args.Size())) {
        return StringConverter::FromString<int>(this->args[index + 1]);
    }
    else {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
float
Args::GetFloat(const String& arg, float defaultValue) const {
    int index = this->args.FindIndexLinear(arg);
    if ((InvalidIndex != index) && ((index + 1) < this->args.Size())) {
        return StringConverter::FromString<float>(this->args[index + 1]);
    }
    else {
        return defaultValue;
    }
}

//------------------------------------------------------------------------------
const Array<String>&
Args::GetArgs() const {
    return this->args;
}

} // namespace Oryol
