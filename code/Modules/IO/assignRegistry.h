#pragma once
//------------------------------------------------------------------------------
/**
    @class IO::assignRegistry
    
    Central registry for assign definitions. Assigns are
    path aliases (google for AmigaOS assign).
*/
#include "Core/RefCounted.h"
#include <mutex>
#include <map>

namespace Oryol {
namespace IO {

class assignRegistry : public Core::RefCounted {
    OryolGlobalSingletonDecl(assignRegistry);
public:
    /// constructor
    assignRegistry();
    /// destructor
    virtual ~assignRegistry();
    
    /// add or replace an assign definition
    void SetAssign(const std::string& assign, const std::string& path);
    /// check if an assign exists
    bool HasAssign(const std::string& assign) const;
    /// lookup an assign (return empty string if not exists)
    std::string LookupAssign(const std::string& assign) const;
    /// resolve assigns in the provided string
    std::string ResolveAssigns(const std::string& str) const;
    
private:
    /// setup the standard assigns
    void setStandardAssigns();

    mutable std::mutex mtx;
    std::map<std::string, std::string> assigns;
};
    
} // namespace IO
} // namespace Oryol