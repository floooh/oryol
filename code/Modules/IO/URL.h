#pragma once
//------------------------------------------------------------------------------
/**
    @class IO::URL
    
    Oryol URL class. Use URLUtil and URLParts class if you
    want to split an URL into its parts, or build an URL from parts.
*/
#include "Core/Types.h"
#include "Core/String/StringAtom.h"
#include "IO/URLParts.h"

namespace Oryol {
namespace IO {

class URL
{
public:
    /// default constructor
    URL();
    /// copy constructor
    URL(const URL& rhs);
    /// move constructor
    URL(URL&& rhs);
    /// construct from raw string
    URL(const char* rhs);
    /// construct from StringAtom
    URL(const String::StringAtom& rhs);
    /// construct from std::string
    URL(const std::string& rhs);
    
    /// assignment from other URL
    void operator=(const URL& rhs);
    /// move-assign from other URL
    void operator=(URL&& rhs);
    /// assign from raw string
    void operator=(const char* rhs);
    /// assignment from StringAtom
    void operator=(const String::StringAtom& rhs);
    /// assignment from std::string
    void operator=(const std::string& rhs);
    
    /// equality
    bool operator==(const URL& rhs);
    /// inequality
    bool operator!=(const URL& rhs);

    /// get the URL string
    const String::StringAtom& Get() const;

    /// split an URL into parts
    URLParts Split() const;
    /// build an URL from parts
    static URL Build(const URLParts& parts);
    
private:
    String::StringAtom content;
};

//------------------------------------------------------------------------------
inline
URL::URL() {
    // empty
}

//------------------------------------------------------------------------------
inline
URL::URL(const URL& rhs) :
content(rhs.content) {

    // empty
}

//------------------------------------------------------------------------------
inline
URL::URL(URL&& rhs) {
    this->content = rhs.content;
    rhs.content.Clear();
}

//------------------------------------------------------------------------------
inline
URL::URL(const char* rhs) :
content(rhs) {
    // empty
}

//------------------------------------------------------------------------------
inline
URL::URL(const String::StringAtom& rhs) :
content(rhs) {
    // empty
}

//------------------------------------------------------------------------------
inline
URL::URL(const std::string& rhs) :
content(rhs) {
    // empty
}

//------------------------------------------------------------------------------
inline void
URL::operator=(const URL& rhs) {
    this->content = rhs.content;
}

//------------------------------------------------------------------------------
inline void
URL::operator=(URL&& rhs) {
    this->content = rhs.content;
    rhs.content.Clear();
}

//------------------------------------------------------------------------------
inline void
URL::operator=(const char* rhs) {
    this->content = rhs;
}

//------------------------------------------------------------------------------
inline void
URL::operator=(const String::StringAtom& rhs) {
    this->content = rhs;
}

//------------------------------------------------------------------------------
inline void
URL::operator=(const std::string& rhs) {
    this->content = rhs;
}

//------------------------------------------------------------------------------
inline bool
URL::operator==(const URL& rhs) {
    return this->content == rhs.content;
}

//------------------------------------------------------------------------------
inline bool
URL::operator!=(const URL& rhs) {
    return this->content != rhs.content;
}

//------------------------------------------------------------------------------
inline const String::StringAtom&
URL::Get() const {
    return this->content;
}
    
} // namespace IO
} // namespace Oryol