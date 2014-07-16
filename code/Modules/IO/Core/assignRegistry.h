#pragma once
//------------------------------------------------------------------------------
/*
    private class, do not use
 
    Central registry for assign definitions. Assigns are
    path aliases (google for AmigaOS assign).
*/
#include "Core/RefCounted.h"
#include "Core/Singleton.h"
#include "Core/Containers/Map.h"
#include "Core/String/String.h"
#include "Core/Threading/RWLock.h"

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
    void SetAssign(const Core::String& assign, const Core::String& path);
    /// check if an assign exists
    bool HasAssign(const Core::String& assign) const;
    /// lookup an assign (return empty string if not exists)
    Core::String LookupAssign(const Core::String& assign) const;
    /// resolve assigns in the provided string
    Core::String ResolveAssigns(const Core::String& str) const;
    
private:
    /// setup the standard assigns
    void setStandardAssigns();
    
    mutable Core::RWLock rwLock;
    Core::Map<Core::String, Core::String> assigns;
};
    
} // namespace IO
} // namespace Oryol