#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::RunLoop
    @brief universal run-loop object for on-frame callbacks
    
    A runloop object manages a priority-sorted array of callback 
    functions which are called per-frame. By default, each thread
    has a RunLoop object which can be configured through the CoreFacade
    singleton. Runloops can be nested by adding the Run() function
    of one runloop to another runloop. NOTE that priority values are
    inverted, lower values are called first).
    
    Examples for constructing callbacks:
    
    1. from C function myFunc():

        Callback("name", pri, std::function<void()>(&myFunc);
    2. from an object's method (careful, object must not go out-of-scope
       as long as the callback is added to the RunLoop!

        MyClass myObj;<br>
        Callback("name", pri, std::function<void()>(&MyClass::MyMethod, &myObj));
*/
#include "Core/RefCounted.h"
#include "Core/String/StringAtom.h"
#include "Core/Containers/Map.h"

namespace Oryol {
namespace Core {

class RunLoop : public RefCounted {
    OryolClassDecl(RunLoop);
public:
    /// callback object
    class Callback {
    public:
        /// default constructor
        Callback() : pri(0), valid(false) { };
        /// constructor with name, priority and function
        Callback(const StringAtom& name_, int32 pri_, std::function<void()> func_) : name(name_), pri(pri_), func(func_), valid(false) { };
        /// get name
        const StringAtom& Name() const { return this->name; };
        /// get priority
        int32 Priority() const { return this->pri; };
        /// get function pointer
        const std::function<void()>& Func() const { return this->func; };
        /// set valid flag
        void SetValid(bool b) { this->valid = b; };
        /// get valid flag
        bool IsValid() const { return this->valid; };
    private:
        StringAtom name;
        int32 pri;
        std::function<void()> func;
        bool valid;
    };

    /// constructor
    RunLoop();
    /// destructor
    virtual ~RunLoop();
    
    /// run one frame
    void Run();
    
    /// add a callback to the run loop, higher priorities run earlier, slow!
    void Add(const Callback& callback);
    /// remove a callback, slow!
    void Remove(const StringAtom& name);
    /// test if a callback has been attached, slow!
    bool HasCallback(const StringAtom& name) const;
    /// get the callbacks map, key is priority, value is Callback object
    const Map<int32, Callback>& Callbacks() const;
    
private:
    /// find callback index by name
    int32 FindCallback(const StringAtom& name) const;
    /// add new callbacks that have been added (called at beginning of Run())
    void AddCallbacks();
    /// remove callbacks that have been removed (called at end of Run())
    void RemoveCallbacks();
    
    Map<int32, Callback> callbacks;
    Map<StringAtom, Callback> toAdd;
    Set<StringAtom> toRemove;
};
    
} // namespace Core
} // namespace Oryol
