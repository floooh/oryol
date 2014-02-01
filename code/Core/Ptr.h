#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Core::Ptr
    @brief Oryol's smart pointer class

    The Oryol smart pointer class is used together with the RefCounted
    base class to implement automatic object life-time management.

    @see RefCounted
*/
#include "Core/Types.h"
#include "Core/Macros.h"

namespace Oryol {
namespace Core {

template<class T> class Ptr {
public:
    /// unsafe get, may return nullptr
    T* GetUnsafe() const {
        return p;
    };

    /// default constructor
    Ptr() : p(nullptr) {
        // empty
    };
    /// nullptr constructor
    Ptr(std::nullptr_t) : p(nullptr) {
        // empty
    };
    /// construct from compatible raw pointer
    template<class U> Ptr(U* rhs) {
        set(static_cast<T*>(rhs));
    };
    /// copy-construct from Ptr<TYPE>
    Ptr(const Ptr<T>& rhs) {
        set(rhs.p);
    };
    /// copy-construct from Ptr<OTHER>
    template<class U> Ptr(const Ptr<U>& rhs) {
        set(static_cast<T*>(rhs.GetUnsafe()));
    };
    /// move constructor from Ptr<TYPE>
    Ptr(Ptr<T>&& rhs) {
        p = rhs.p;
        rhs.p = nullptr;
    };
    /// move constructor from compatible type
    template<class U> Ptr(Ptr<U>&& rhs) {
        p = static_cast<T*>(rhs.GetUnsafe());
        rhs.p = nullptr;
    };
    
    /// destructor
    ~Ptr() {
        del();
    };
    
    /// copy-assign from Ptr<TYPE>
    void operator=(const Ptr<T>& rhs) {
        if (rhs.p != p) {
            del();
            set(rhs.p);
        }
    };
    /// copy-assign from compatible raw pointer
    template<class U> void operator=(U* rhs) {
        if (static_cast<T*>(rhs) != p) {
            del();
            set(static_cast<T*>(rhs));
        }
    };
    /// copy-assign from compatible Ptr<>
    template<class U> void operator=(const Ptr<U>& rhs) {
        const T* rhs_p = static_cast<T*>(rhs.GetUnsafe());
        if (rhs_p != p) {
            del();
            set(static_cast<T*>(rhs_p));
        }
    };
    /// move-assign from comptabile Ptr<>
    template<class U> void operator=(Ptr<U>&& rhs) {
        T* rhs_p = static_cast<T*>(rhs.GetUnsafe());
        if (rhs_p != p) {
            del();
            p = rhs_p;
            rhs.p = nullptr;
        }
    };
    /// assign nullptr (equivalent with .invalidate())
    void operator=(std::nullptr_t) {
        del();
    };
    
    /// operator==
    template<class U> bool operator==(const Ptr<U>& rhs) const {
        return p == rhs.GetUnsafe();
    };
    /// operator!=
    template<class U> bool operator!=(const Ptr<U>& rhs) const {
        return p != rhs.GetUnsafe();
    };
    /// operator<
    template<class U> bool operator<(const Ptr<U>& rhs) const {
        return p < rhs.GetUnsafe();
    };
    /// operator>
    template<class U> bool operator>(const Ptr<U>& rhs) const {
        return p > rhs.GetUnsafe();
    };
    /// operator<=
    template<class U> bool operator<=(const Ptr<U>& rhs) const {
        return p <= rhs.GetUnsafe();
    };
    /// operator>=
    template<class U> bool operator>=(const Ptr<U>& rhs) const {
        return p >= rhs.GetUnsafe();
    };
    /// test if invalid (contains nullptr)
    bool operator==(std::nullptr_t) const {
        return nullptr == p;
    };
    /// test if valid (contains valid ptr)
    bool operator!=(std::nullptr_t) const {
        return nullptr != p;
    };

    /// cast to bool
    explicit operator bool() const {
        return nullptr != p;
    };
    /// cast to compatible type
    template<class U, class=typename std::enable_if<std::is_convertible<U*,T*>::value>::type> operator U() const {
        return static_cast<Ptr<U>>(*this);
    };
    /// operator*
    T& operator*() const {
        o_assert(nullptr != p);
        return *p;
    };
    /// operator->
    T* operator->() const {
        o_assert(nullptr != p);
        return p;
    };
    
    /// return true if valid (equivalent with operator bool())
    bool IsValid() const {
        return nullptr != p;
    };
    /// invalidate the ptr (equivalent with assigning nullptr)
    void Invalidate() {
        del();
    };

    /// get (assert that returned pointer is not nullptr)
    T* Get() const {
        o_assert(nullptr != p);
        return p;
    };

private:
    /// delete content
    void del() {
        if (nullptr != p) {
            p->release();
            p = nullptr;
        }
    };
    /// set a new pointer
    void set(T* rhs) {
        p = rhs;
        if (nullptr != p) {
            p->addRef();
        }
    };

    T* p;
};

} // namespace core
} // namespace oryol
