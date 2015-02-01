#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ThreadLocalData
    @brief manage a thread-local pointer table via pthread keys
    
    ThreadLocalData and ThreadLocalPtr enable thread-local support
    on platforms where the compiler doesn't have a thread-local keyword
    (only on iOS so far).
*/
#include "Core/Types.h"
#if ORYOL_THREADLOCAL_PTHREAD
#include "Core/Assertion.h"
#include <atomic>
#include <pthread.h>

namespace Oryol {
    
class ThreadLocalData {
public:
    /// setup once from main-thread
    static void SetupOnce();
    
    /// allocate a new slot
    static int32 Alloc();
    /// associate slot index with a pointer
    static void Set(int32 slotIndex, void* ptr);
    /// get pointer associated with slot
    static void* Get(int32 slotIndex);
    
private:
    /// get pointer to thread-local pointer-table
    static void** PointerTable();
    
    static const int MaxNumSlots = 1024;
    static pthread_key_t key;
    static std::atomic<int32> curSlot;
};

//------------------------------------------------------------------------------
inline void*
ThreadLocalData::Get(int32 slotIndex) {
    o_assert_range_dbg(slotIndex, MaxNumSlots);
    void** table = PointerTable();
    return table[slotIndex];
}
    
} // namespace Oryol
#endif // ORYOL_COMPILER_HAS_THREADLOCAL
