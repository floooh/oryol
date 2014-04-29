#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Creator
    @brief deferred creation of objects with constructor args

    A creator is an object which can create objects with constructor args
    at a later time. 
    
    Usage:
    
    Suppose there's a method argument like this:
    
    void SetFileSystem(CreatorRef<FileSystem> fs);
    
    This means that the SetFileSystem method expects a creator object
    which can create a FileSystem object (or objects of derived classes).
    
    For instance to pass a creator for a default-constructed FileSystem
    object:
    
    bla->SetFileSystem(Creator<FileSystem>());
    
    If FileSystem has a constructor with values, you could also call:
    
    bla->SetFileSystem(Creator<FileSystem>(1, "bla"));
    
    If you want to pass a creator for a derived class HttpFileSystem:
    
    bla->SetFileSystem(Creator<HttpFileSystem, FileSystem>());
    
    The first template parameter is the class of the actually created object,
    and the second template parameter the type of the Creator's return value.
    If the first template parameter is not a derived class of the second
    template parameter, a static_assert will trigger, so this is completely
    type-safe.
    
    Again, if HttpFileSystem accepts constructor parameters, these can be
    passed as well:
    
    bla->SetFileSystem(Creator<HttpFileSystem, FileSystem>(1, "bla"));
*/
#include "Core/Ptr.h"
#include "Core/RefCounted.h"

namespace Oryol {
namespace Core {
    
template<class TYPE> class creatorBase : public RefCounted {
public:
    /// constructor
    creatorBase() { };
    /// create object
    virtual Ptr<TYPE> New() const = 0;
};

template<class TYPE> class creator : public creatorBase<TYPE> {
public:
    /// constructor
    creator() { };
    /// create object
    virtual Ptr<TYPE> New() const {
        return TYPE::Create();
    };
};

template<class TYPE, typename V0> class creator_v0 : public creatorBase<TYPE> {
public:
    /// constructor
    creator_v0(V0 v0_) : v0(v0_) { };
    /// create object
    virtual Ptr<TYPE> New() const {
        return TYPE::Create(this->v0);
    };
private:
    V0 v0;
};

template<class TYPE, typename V0, typename V1> class creator_v0_v1 : public creatorBase<TYPE> {
public:
    /// constructor
    creator_v0_v1(V0 v0_, V1 v1_) : v0(v0_), v1(v1_) { };
    /// create object
    virtual Ptr<TYPE> New() const {
        return TYPE::Create(this->v0, this->v1);
    };
private:
    V0 v0;
    V1 v1;
};

template<class TYPE, typename V0, typename V1, typename V2> class creator_v0_v1_v2 : public creatorBase<TYPE> {
public:
    /// constructor
    creator_v0_v1_v2(V0 v0_, V1 v1_, V2 v2_) : v0(v0_), v1(v1_), v2(v2_) { };
    /// create object
    virtual Ptr<TYPE> New() const {
        return TYPE::Create(this->v0, this->v1, this->v2);
    };
private:
    V0 v0;
    V1 v1;
    V2 v2;
};

template<typename TYPE> using CreatorRef = Ptr<creatorBase<TYPE>>;

template<class TYPE, class BASETYPE=TYPE> inline Ptr<creatorBase<BASETYPE>> Creator() {
    static_assert(std::is_base_of<BASETYPE, TYPE>::value, "BASETYPE must be base class of TYPE!");
    return Ptr<creatorBase<BASETYPE>>((creatorBase<BASETYPE>*)new creator<TYPE>());
}
template<class TYPE, class BASETYPE=TYPE, typename V0> inline Ptr<creatorBase<BASETYPE>> Creator(V0 v0) {
    static_assert(std::is_base_of<BASETYPE, TYPE>::value, "BASETYPE must be base class of TYPE!");
    return Ptr<creatorBase<BASETYPE>>((creatorBase<BASETYPE>*)new creator_v0<TYPE,V0>(v0));
}
template<class TYPE, class BASETYPE=TYPE, typename V0, typename V1> inline Ptr<creatorBase<BASETYPE>> Creator(V0 v0, V1 v1) {
    static_assert(std::is_base_of<BASETYPE, TYPE>::value, "BASETYPE must be base class of TYPE!");
    return Ptr<creatorBase<BASETYPE>>((creatorBase<BASETYPE>*)new creator_v0_v1<TYPE,V0,V1>(v0, v1));
}
template<class TYPE, class BASETYPE=TYPE, typename V0, typename V1, typename V2> inline Ptr<creatorBase<BASETYPE>> Creator(V0 v0, V1 v1, V2 v2) {
    static_assert(std::is_base_of<BASETYPE, TYPE>::value, "BASETYPE must be base class of TYPE!");
    return Ptr<creatorBase<BASETYPE>>((creatorBase<BASETYPE>*)new creator_v0_v1_v2<TYPE,V0,V1,V2>(v0, v1, v2));
}

} // namespace Core
} // namespace Oryol