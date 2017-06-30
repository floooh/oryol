//------------------------------------------------------------------------------
//  QeueTest.cc
//  Test Queue class.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/Queue.h"
#include "Core/String/StringAtom.h"

using namespace Oryol;

TEST(QueueTest) {

    // test empty queue
    Queue<StringAtom> queue0;
    CHECK(queue0.GetMinGrow() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(queue0.GetMaxGrow() == ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    CHECK(queue0.Size() == 0);
    CHECK(queue0.Capacity() == 0);
    CHECK(queue0.Empty());
    CHECK(queue0.SpareEnqueue() == 0);
    CHECK(queue0.SpareDequeue() == 0);
    
    // copy-construct from empty queue
    Queue<StringAtom> queue1(queue0);
    CHECK(queue1.GetMinGrow() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(queue1.GetMaxGrow() == ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    CHECK(queue1.Size() == 0);
    CHECK(queue1.Capacity() == 0);
    CHECK(queue1.Empty());
    CHECK(queue1.SpareEnqueue() == 0);
    CHECK(queue1.SpareDequeue() == 0);
    
    // copy-assign empty queue
    Queue<StringAtom> queue2;
    queue2 = queue0;
    CHECK(queue2.GetMinGrow() == ORYOL_CONTAINER_DEFAULT_MIN_GROW);
    CHECK(queue2.GetMaxGrow() == ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    CHECK(queue2.Size() == 0);
    CHECK(queue2.Capacity() == 0);
    CHECK(queue2.Empty());
    CHECK(queue2.SpareEnqueue() == 0);
    CHECK(queue2.SpareDequeue() == 0);
    
    // enqueue some elements
    const StringAtom w0("Element0");
    const StringAtom w1("Element1");
    const StringAtom w2("Element2");
    const StringAtom w3("Element3");
    queue0.Enqueue(w0);
    queue0.Enqueue(w1);
    queue0.Enqueue(w2);
    queue0.Enqueue(w3);
    CHECK(queue0.Size() == 4);
    CHECK(queue0.SpareDequeue() == 0);
    
    // dequeue elements
    StringAtom r0(queue0.Dequeue());
    CHECK(r0 == w0);
    CHECK(r0 == "Element0");
    CHECK(queue0.Size() == 3);
    CHECK(queue0.SpareDequeue() == 1);
    CHECK(queue0.Dequeue() == "Element1");
    CHECK(queue0.Size() == 2);
    CHECK(queue0.SpareDequeue() == 2);
    StringAtom r2;
    queue0.Dequeue(r2);
    CHECK(r2 == w2);
    CHECK(r2 == "Element2");
    CHECK(queue0.Size() == 1);
    CHECK(queue0.SpareDequeue() == 3);
    StringAtom r3 = queue0.Dequeue();
    CHECK(r3 == w3);
    CHECK(r3 == "Element3");
    CHECK(queue0.Empty());
    
    queue0.Enqueue("Bla");
    CHECK(queue0.SpareDequeue() == 0);
    CHECK(queue0.Size() == 1);
    CHECK(queue0.Dequeue() == "Bla");

    // SetFixedCapacity
    // fixed capacity
    Queue<int> queue3;
    queue3.SetFixedCapacity(128);
    CHECK(queue3.Size() == 0);
    CHECK(queue3.Capacity() == 128);
    CHECK(queue3.GetMinGrow() == 0);
    CHECK(queue3.GetMaxGrow() == 0);
}
    
