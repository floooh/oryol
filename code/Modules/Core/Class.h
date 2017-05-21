#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Class.h
    @brief Oryol class annotation macros
*/
#include "Core/Memory/Memory.h"

/// declare an Oryol class without pool allocator (located inside class declaration)
#define OryolBaseClassDecl(TYPE) \
protected:\
virtual void destroy() {\
    Oryol::Memory::Delete(this);\
};\
public:\
template<typename... ARGS> static Oryol::Ptr<TYPE> Create(ARGS&&... args) {\
    return Oryol::Ptr<TYPE>(Oryol::Memory::New<TYPE>(std::forward<ARGS>(args)...));\
};

/// declare an Oryol class without pool allocator (located inside class declaration)
#define OryolClassDecl(TYPE) \
protected:\
virtual void destroy() override {\
    Oryol::Memory::Delete(this);\
};\
public:\
template<typename... ARGS> static Oryol::Ptr<TYPE> Create(ARGS&&... args) {\
    return Oryol::Ptr<TYPE>(Oryol::Memory::New<TYPE>(std::forward<ARGS>(args)...));\
};

/// add simple RTTI system to a class, inspired by turbobadger's RTTI system
namespace Oryol {
    typedef void* TypeId;
}
#define OryolBaseTypeDecl(CLASS)\
protected:\
    template<class T> static Oryol::TypeId getTypeId() { static char _type_id; return &_type_id; };\
    virtual bool isOfType(const Oryol::TypeId t) const { return t == getTypeId<CLASS>(); };\
public:\
    template<class T> bool IsA() const { return this->isOfType(getTypeId<T>()); };\
    template<class T> Oryol::Ptr<T> DynamicCast() const { return (T*) (this->isOfType(getTypeId<T>()) ? this : nullptr); };\

#define OryolTypeDecl(CLASS, BASECLASS)\
protected:\
    virtual bool isOfType(const Oryol::TypeId t) const override { return t == getTypeId<CLASS>() ? true : BASECLASS::isOfType(t); };\

