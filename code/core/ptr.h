#pragma once
//------------------------------------------------------------------------------
/**
    @class core::ptr
    
*/
#include "core/types.h"
#include "core/macros.h"

namespace oryol {
namespace core {

template<class T> class ptr {
public:
    /// unsafe get, may return nullptr
    T* get_unsafe() const noexcept {
        return p;
    };

    /// default constructor
    constexpr ptr() noexcept : p(nullptr) {
        // empty
    };
    /// nullptr constructor
    constexpr ptr(std::nullptr_t) noexcept : p(nullptr) {
        // empty
    };
    /// construct from compatible raw pointer
    template<class U> ptr(U* rhs) noexcept {
        set(static_cast<T*>(rhs));
    };
    /// copy-construct from ptr<TYPE>
    ptr(const ptr<T>& rhs) noexcept {
        set(rhs.p);
    };
    /// copy-construct from ptr<OTHER>
    template<class U> ptr(const ptr<U>& rhs) noexcept {
        set(static_cast<T*>(rhs.get_unsafe()));
    };
    /// move constructor from ptr<TYPE>
    ptr(ptr<T>&& rhs) noexcept {
        p = rhs.p;
        rhs.p = nullptr;
    };
    /// move constructor from compatible type
    template<class U> ptr(ptr<U>&& rhs) noexcept {
        p = static_cast<T*>(rhs.get_unsafe());
        rhs.p = nullptr;
    };
    
    /// destructor
    ~ptr() {
        del();
    };
    
    /// assign from compatible raw pointer
    template<class U> void operator=(U* rhs) noexcept {
        if (static_cast<T*>(rhs) != p) {
            del();
            set(static_cast<T*>(rhs));
        }
    };
    /// assign from compatible ptr<>
    template<class U> void operator=(const ptr<U>& rhs) noexcept {
        const T* rhs_p = static_cast<T*>(rhs.get_unsafe());
        if (rhs_p != p) {
            del();
            set(static_cast<T*>(rhs_p));
        }
    };
    /// move-assign from comptabile ptr<>
    template<class U> void operator=(ptr<U>&& rhs) noexcept {
        T* rhs_p = static_cast<T*>(rhs.get_unsafe());
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
    template<class U> bool operator==(const ptr<U>& rhs) const noexcept {
        return p == rhs.get_unsafe();
    };
    /// operator!=
    template<class U> bool operator!=(const ptr<U>& rhs) const noexcept {
        return p != rhs.get_unsafe();
    };
    /// operator<
    template<class U> bool operator<(const ptr<U>& rhs) const noexcept {
        return p < rhs.get_unsafe();
    };
    /// operator>
    template<class U> bool operator>(const ptr<U>& rhs) const noexcept {
        return p > rhs.get_unsafe();
    };
    /// operator<=
    template<class U> bool operator<=(const ptr<U>& rhs) const noexcept {
        return p <= rhs.get_unsafe();
    };
    /// operator>=
    template<class U> bool operator>=(const ptr<U>& rhs) const noexcept {
        return p >= rhs.get_unsafe();
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
        return static_cast<ptr<U>>(*this);
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
    bool isvalid() const noexcept {
        return nullptr != p;
    };
    /// invalidate the ptr (equivalent with assigning nullptr)
    void invalidate() noexcept {
        del();
    };

    /// get (assert that returned pointer is not nullptr)
    T* get() const noexcept {
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
            p->add_ref();
        }
    };

    T* p;
};

} // namespace core
} // namespace oryol
