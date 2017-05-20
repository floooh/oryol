//------------------------------------------------------------------------------
//  CreationTest.cc
//  Test object creation.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Log.h"
#include "Core/Core.h"
#include "Core/Types.h"
#include "Core/RefCounted.h"
#include "Core/Ptr.h"
#include "Core/Containers/Array.h"

#include <thread>
#include <array>
#include <vector>
#include <memory>

using namespace std;
using namespace Oryol;

// define a custom class
class TestClass : public RefCounted {
    OryolClassDecl(TestClass);
public:
    TestClass() : val(0) {
//        Log::Info("constructor called!\n");
    };
    TestClass(int v) : val(v) {
//        Log::Info("constructor with '%d' called!\n", this->val);
    }
    virtual ~TestClass() {
//        Log::Info("destructor called!\n");
    };
    void Set(int i) { this->val = i; };
    int Get() const { return this->val; };
private:
    int val;
};

TEST(CreateShared) {

    auto ptr0 = TestClass::Create();
    CHECK(bool(ptr0));
    CHECK(ptr0.isValid());
    CHECK(ptr0->GetRefCount() == 1);
    
    // assign from some pointer type
    auto ptr1 = ptr0;
    CHECK(ptr0 == ptr1);
    CHECK(ptr0 && ptr1);
    CHECK(ptr0->GetRefCount() == 2);
    CHECK(ptr1->GetRefCount() == 2);
    
    // non-default constructor
    auto ptr2 = TestClass::Create(128);
    CHECK(ptr2->Get() == 128);
    
    // invalidation
    ptr0.invalidate();
    CHECK(!bool(ptr0));
    CHECK(bool(ptr1));
    CHECK(ptr1->GetRefCount() == 1);
    
    // move semantics
    ptr0 = std::move(ptr1);
    CHECK(ptr0.isValid());
    CHECK(!ptr1.isValid());
    CHECK(ptr0->GetRefCount() == 1);
}

TEST(CreatePtrBenchmark) {

    for (int i = 0; i < 3; i++) {
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        
        const int maxLiveObjects = 65535;
        const int numObjects = 1000000;
        const int numOuterLoop = numObjects / maxLiveObjects;
        for (int j = 0; j < numOuterLoop; j++) {
            Array<Ptr<TestClass>> objs;
            objs.Reserve(maxLiveObjects);
            for (int k = 0; k < maxLiveObjects; k++) {
                objs.Add(TestClass::Create());
            }
        }
        end = chrono::system_clock::now();
        chrono::duration<double> dur = end - start;
        Log::Info("run %d: %dx Ptr<TestClass> created: %f sec\n", i, numOuterLoop * maxLiveObjects, dur.count());
    }

    for (int i = 0; i < 3; i++) {
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        
        const int numObjects = 1000000;
        std::vector<shared_ptr<TestClass>> objs;
        objs.reserve(numObjects);
        for (int j = 0; j < numObjects; j++) {
            objs.emplace_back(std::make_shared<TestClass>());
        }
        objs.clear();   // important: also measure destruction
        
        end = chrono::system_clock::now();
        chrono::duration<double> dur = end - start;
        Log::Info("run %d: %dx shared_ptr<TestClass> created: %f sec\n", i, numObjects, dur.count());
    }
}

#if ORYOL_HAS_THREADS
const int numInner = 65535 / 8;
const int numOuter = 1000000 / numInner;

void threadFunc() {
    Oryol::Core::EnterThread();
    hash<thread::id> hashfn;
    
    Log::Info("CreateMultiThreaded: thread '%lu' entered!\n", hashfn(this_thread::get_id()));
    
    for (int i = 0; i < numOuter; i++) {
        Array<Ptr<TestClass>> pointers;
        pointers.Reserve(numInner);
        for (int j = 0 ; j < numInner; j++) {
            pointers.Add(TestClass::Create());
            o_assert(1 == pointers.Back()->GetRefCount());
        }
    }

    Log::Info("CreateMultiThreaded: thread '%lu' leaving!\n", hashfn(this_thread::get_id()));
    
    Oryol::Core::LeaveThread();
}

TEST(CreateMultiThreaded) {
    
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    
    // create a few threads
    const int numThreads = 8;
    std::array<std::thread, numThreads> threads;
    for (int i = 0; i < numThreads; i++) {
        threads[i] = std::thread(threadFunc);
    }
    for (int i = 0; i < numThreads; i++) {
        threads[i].join();
    }
    
    end = chrono::system_clock::now();
    chrono::duration<double> dur = end - start;
    Log::Info("%d objects created in %d threads: %f sec\n", numOuter * numInner * numThreads, numThreads, dur.count());
}
#endif
