#pragma once
//------------------------------------------------------------------------------
/*
    @class Oryol::_priv::assignRegistry
    @ingroup _priv
    @brief central registry for assign definitions
 
    Central registry for assign definitions. Assigns are
    path aliases (google for AmigaOS assign).
*/
#include "Core/Containers/Map.h"
#include "Core/String/String.h"
#include "Core/Threading/RWLock.h"

namespace Oryol {
namespace _priv {

class assignRegistry {
public:
    /// add or replace an assign definition
    void SetAssign(const String& assign, const String& path);
    /// check if an assign exists
    bool HasAssign(const String& assign) const;
    /// lookup an assign (return empty string if not exists)
    String LookupAssign(const String& assign) const;
    /// resolve assigns in the provided string
    String ResolveAssigns(const String& str) const;
    
private:
    /// setup the standard assigns
    void setStandardAssigns();
    
    mutable RWLock rwLock;
    Map<String, String> assigns;
};
    
} // namespace _priv
} // namespace Oryol