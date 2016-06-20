//------------------------------------------------------------------------------
//  ThreadLocalData.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "ThreadLocalData.h"
#include "Core/Memory/Memory.h"
#include "Core/Assertion.h"

#if ORYOL_THREADLOCAL_PTHREAD
namespace Oryol {
    
pthread_key_t ThreadLocalData::key = 0;
std::atomic<int> ThreadLocalData::curSlot{0};
    
//------------------------------------------------------------------------------
void
ThreadLocalData::SetupOnce() {
    if (0 == key) {
        pthread_key_create(&key, 0);
        o_assert_dbg(0 != key);
    }
}

//------------------------------------------------------------------------------
void**
ThreadLocalData::PointerTable() {
    o_assert_dbg(0 != key);
    
    // get thread-local value (null if none assigned yet)
    void** table = (void**) pthread_getspecific(key);
    if (0 == table) {
        // not assigned yet, allocate thread-specific table and
        // associate with key
        int tableSize = MaxNumSlots * sizeof(void*);
        table = (void**) Memory::Alloc(tableSize);
        Memory::Clear(table, tableSize);
        pthread_setspecific(key, table);
    }
    return table;
}

//------------------------------------------------------------------------------
int
ThreadLocalData::Alloc() {
    o_assert_dbg(curSlot < (MaxNumSlots - 1));
    return curSlot++;
}
    
//------------------------------------------------------------------------------
void
ThreadLocalData::Set(int slotIndex, void* ptr) {
    o_assert_range_dbg(slotIndex, MaxNumSlots);
    void** table = PointerTable();
    table[slotIndex] = ptr;
}

} // namespace Oryol
#else
// silence 'has no symbols' warning
ORYOL_UNUSED int oryol_thread_local_dummy = 0;
#endif
