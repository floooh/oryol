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
    T* GetUnsafe() const noexcept {
        return p;
    };

    /// default constructor
    constexpr Ptr() noexcept : p(nullptr) {
        // empty
    };
    /// nullptr constructor
    constexpr Ptr(std::nullptr_t) noexcept : p(nullptr) {
        // empty
    };
    /// construct from compatible raw pointer
    template<class U> Ptr(U* rhs) noexcept {
        set(static_cast<T*>(rhs));
    };
    /// copy-construct from Ptr<TYPE>
    Ptr(const Ptr<T>& rhs) noexcept {
        set(rhs.p);
    };
    /// copy-construct from Ptr<OTHER>
    template<class U> Ptr(const Ptr<U>& rhs) noexcept {
        set(static_cast<T*>(rhs.GetUnsafe()));
    };
    /// move constructor from Ptr<TYPE>
    Ptr(Ptr<T>&& rhs) noexcept {
        p = rhs.p;
        rhs.p = nullptr;
    };
    /// move constructor from compatible type
    template<class U> Ptr(Ptr<U>&& rhs) noexcept {
        p = static_cast<T*>(rhs.GetUnsafe());
        rhs.p = nullptr;
    };
    
    /// destructor
    ~Ptr() {
        del();
    };
    
    /// assign from compatible raw pointer
    template<class U> void operator=(U* rhs) noexcept {
        if (static_cast<T*>(rhs) != p) {
            del();
            set(static_cast<T*>(rhs));
        }
    };
    /// assign from compatible Ptr<>
    template<class U> void operator=(const Ptr<U>& rhs) noexcept {
        const T* rhs_p = static_cast<T*>(rhs.GetUnsafe());
        if (rhs_p != p) {
            del();
            set(static_cast<T*>(rhs_p));
        }
    };
    /// move-assign from comptabile Ptr<>
    template<class U> void operator=(Ptr<U>&& rhs) noexcept {
        T* rhs_p = static_cast<T*>(rhs.GetUnsafe());
        if (rhs_p != p) {
            del();
            p = rhs_p;
            rhs.p = nullptr;
        }
    };
    /// assign nullptr (equivalent with .invalidate())
    void operator=(std::nullptr_t) noexcept {
        del();
    };
    
    /// operator==
    template<class U> bool operator==(const Ptr<U>& rhs) const noexcept {
        return p == rhs.GetUnsafe();
    };
    /// operator!=
    template<class U> bool operator!=(const Ptr<U>& rhs) const noexcept {
        return p != rhs.GetUnsafe();
    };
    /// operator<
    template<class U> bool operator<(const Ptr<U>& rhs) const noexcept {
        return p < rhs.GetUnsafe();
    };
    /// operator>
    template<class U> bool operator>(const Ptr<U>& rhs) const noexcept {
        return p > rhs.GetUnsafe();
    };
    /// operator<=
    template<class U> bool operator<=(const Ptr<U>& rhs) const noexcept {
        return p <= rhs.GetUnsafe();
    };
    /// operator>=
    template<class U> bool operator>=(const Ptr<U>& rhs) const noexcept {
        return p >= rhs.GetUnsafe();
    };
    /// test if invalid (contains nullptr)
    bool operator==(std::nullptr_t) const noexcept {
        return nullptr == p;
    };
    /// test if valid (contains valid ptr)
    bool operator!=(std::nullptr_t) const noexcept {
        return nullptr != p;
    };

    /// cast to bool
    explicit operator bool() const noexcept {
        return nullptr != p;
    };
    /// cast to compatible type
    template<class U, class=typename std::enable_if<std::is_convertible<U*,T*>::value>::type> operator U() const noexcept {
        return static_cast<Ptr<U>>(*this);
    };
    /// operator*
    T& operator*() const noexcept {
        o_assert(nullptr != p);
        return *p;
    };
    /// operator->
    T* operator->() const noexcept {
        o_assert(nullptr != p);
        return p;
    };
    
    /// return true if valid (equivalent with operator bool())
    bool IsValid() const noexcept {
        return nullptr != p;
    };
    /// invalidate the ptr (equivalent with assigning nullptr)
    void Invalidate() noexcept {
        del();
    };

    /// get (assert that returned pointer is not nullptr)
    T* Get() const noexcept {
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
