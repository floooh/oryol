#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Args
    @brief access command line args
    @see AppEntry
 
    The command line args are extracted by the platform-specific OryolApp()
    macro and handed to the OryolMain() function. Not all platform support
    command line arguments. Arguments will be split by space and tab, the fence
    character is ". There is no special handling for "-" or "--". There
    is no key-value-pair concept built in, the number of arguments is simply
    the number of string tokens separated by spaces.
    
    Example:
    ```
    // -name "Jack Shaftoe" -age 30 -male
    
    args.HasArg("-name") => true
    args.GetString("-name") => "Jack Shaftoe"
    args.GetInt("-age") => 30
    args.GetInt("-bla", 10) => 10 (default value since arg not present)
    args.HasArg("-male") => true
    args.HasArg("-female") => false
    ```
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Core/String/WideString.h"
#include "Core/Containers/Array.h"

namespace Oryol {
namespace Core {

class Args {
public:
    /// default constructor
    Args();
    /// construct from string
    Args(const String& str);
    /// construct from wide-string (args will be converted to UTF-8)
    Args(const WideString& wstr);
    /// construct from c-style main args
    Args(int argc, const char** argv);
    
    /// test if an argument exists
    bool HasArg(const String& arg) const;
    /// get string after 'arg', or default value if 'arg' doesn't exist
    String GetString(const String& arg, const String& defaultValue="") const;
    /// get int after 'arg', or default value if 'arg' doesn't exist
    int32 GetInt(const String& arg, int32 defaultValue=0) const;
    /// get float after 'arg', or default value if 'arg' doesn't exist
    float32 GetFloat(const String& arg, float32 defaultValue=0.0f) const;
    
private:
    Array<String> args;
};
    
} // namespace Core
} // namespace Oryol