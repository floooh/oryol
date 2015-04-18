#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Class.h
    @brief Oryol class annotation macros
*/
#include "Core/Types.h"

/// declare an Oryol class with pool allocator (located inside class declaration)
#define OryolClassPoolAllocDecl(TYPE) \
private:\
static Oryol::_priv::poolAllocator<TYPE> allocator;\
protected:\
virtual void destroy() {\
    TYPE::allocator.Destroy(this);\
};\
public:\
template<typename... ARGS> static Oryol::Ptr<TYPE> Create(ARGS&&... args) {\
    return Oryol::Ptr<TYPE>(TYPE::allocator.Create(std::forward<ARGS>(args)...));\
};\

/// implementation-side macro for Oryol class with pool allocator (located in .cc source file)
#define OryolClassPoolAllocImpl(TYPE) \
Oryol::_priv::poolAllocator<TYPE> TYPE::allocator;

/// implementation-side macro for template classes with pool allocator (located in .cc source file)
#define OryolTemplClassPoolAllocImpl(TEMPLATE_TYPE, CLASS_TYPE) \
template<class TEMPLATE_TYPE> Oryol::_priv::poolAllocator<CLASS_TYPE<TEMPLATE_TYPE>> CLASS_TYPE<TEMPLATE_TYPE>::allocator;

/// declare an Oryol class without pool allocator (located inside class declaration)
#define OryolClassDecl(TYPE) \
protected:\
virtual void destroy() {\
    Oryol::Memory::Delete(this);\
};\
public:\
template<typename... ARGS> static Oryol::Ptr<TYPE> Create(ARGS&&... args) {\
    return Oryol::Ptr<TYPE>(Oryol::Memory::New<TYPE>(std::forward<ARGS>(args)...));\
};

/// implementation-side macro for Oryol class without pool allocator (located in .cc source file)
#define OryolClassImpl(TYPE)

/// add simple RTTI system to a class, inspired by turbobadger's RTTI system
namespace Oryol {
    typedef void* TypeId;
}
#define OryolBaseTypeDecl(CLASS)\
protected:\
    template<class T> static TypeId getTypeId() { static char _type_id; return &_type_id; };\
    virtual bool isOfType(const TypeId t) const { return t == getTypeId<CLASS>(); };\
public:\
    template<class T> bool IsA() const { return this->isOfType(getTypeId<T>()); };\
    template<class T> T* DynamicCast() const { return (T*) (this->isOfType(getTypeId<T>()) ? this : nullptr); };\

#define OryolTypeDecl(CLASS, BASECLASS)\
protected:\
    virtual bool isOfType(const TypeId t) const { return t == getTypeId<CLASS>() ? true : BASECLASS::isOfType(t); };\

