## Oryol Container Classes

Oryol has it's own set of container classes which don't use exception
and allow finer control over memory allocation behaviour than the 
C++ std library container classes. 

### Buffer

The **Buffer** class is a simple dynamic container for an 'untyped' memory
chunk on the heap. When the Buffer object is deleted, the heap data will be
freed. A Buffer object cannot be _copied_, only _moved_. This is to prevent
accidential expensive memory allocation and duplicate data copies.

There's a number of different ways to fill a buffer
with data:

```cpp
#include "Core/Containers/Buffer.h"
using namespace Oryol;
...

// if we get a pointer to some existing data in
// memory and its size, and want to copy that 
// data into a buffer:
Buffer buf;
buffer.Add(ptr, numBytes);

// it's also possible to write the buffer data
// 'inplace' by adding uninitialized bytes to
// the buffer, and write to the returned pointer:
const int bytesToRead = 128;
uint8_t* ptr = buffer.Add(bytesToRead);
fread(ptr, 1, bytesToRead, fp);
```

A buffer cannot be copied, but moved:

```cpp
// setup a new buffer object with data
Buffer buf0;
buffer.Add(ptr, numBytes);

// copying a Buffer object is illegal, so the following
// will not compile:
Buffer buf1(buf0);
Buffer buf2;
buf2 = buf0;

// ...but a Buffer object can be moved, the original Buffer
// will be "emptied", and move its content into the target:
Buffer buf1(std::move(buf0));
Buffer buf2;
buf2 = std::move(buf0);
```

Access the data in a buffer with the Buffer::Data() and
Buffer::Size() methods:

```cpp
Buffer buf;
buf.Add(data, numBytes);

// get pointer to contained data (read/write)
uint8* ptr = buf.Data();
// get pointer to contained data (read-only)
const uint8* const_ptr = buf.Data();
// get byte-size of contained data
int byteSize = buf.Size();
```

If you need to add data to a Buffer in little pieces, definitely
call **Buffer::Reserve()** first to prevent frequent
reallocations:

```cpp
Buffer buf;
// pre-allocate 64 bytes 
buf.Reserve(64);
// buffer size is still 0 because there's no data in it...
o_assert(0 == buf.Size());
// ... but the capacity is 128
o_assert(128 == buf.Capacity());
// add 16, 32 and another 16 bytes... 
buf.Add(ptr0, 16);
buf.Add(ptr1, 32);
buf.Add(ptr2, 16);
```
Without the call to Reserve(), 3 reallocations would happen in this
code sample (first allocation to 16 bytes, then to 48 bytes, and
finally to 64 bytes). But with the call to Reserve(64) enough capacity
was allocated for holding all the data that was added.

You can **clear** a buffer with the **Buffer::Clear** method, this will
not free any memory, only reset its size to 0:

```cpp
Buffer buf;
// add 64 bytes of data
buf.Add(ptr, 64);
// size and capacity is now both 64:
o_assert((buf.Size() == 64) && (buf.Capacity() == 64));
// clearing the buffer only resets the size but does not free memory:
buf.Clear();
o_assert((buf.Size() == 0) && (buf.Capacity() == 64));
``` 

You can **remove** data from a buffer with the **Buffer::Remove()** method,
this takes a byte-offset, and a byte-size:

```cpp
Buffer buf;
buf.Add(ptr, 128);
// remove 64 bytes from the middle of the data:
int bytesRemoved = buf.Remove(32, 64);
o_assert(bytesRemoved == 32);
o_assert(buf.Size() == 64);
```

### StaticArray&lt;TYPE&gt;

A StaticArray is a a constant-size C-style array with bounds-checking
(similar to the new std::array in C++11). Use this in places where
you would use a C array, but want to have secure bounds-checked
accesses. However keep in mind that the bounds-checks are only
performed in debug mode, not in (optimized) release mode!

```cpp
#include "Core/Containers/StaticArray.h"
using namespace Oryol;
...

// an static array of 5 integers
StaticArray<int, 5> intArray;
intArray[0] = 1;
intArray[2] = 123;
intArray[7] = 1234; // this will assert at runtime
o_assert(intArray.Size() == 5); 

// you can fill the whole array with a value:
intArray.Fill(10);

// .begin/.end works:
for (int val : intArray) {
    ...
}
```

### Array&lt;TYPE&gt;

The **Array** class is a dynamic, linear container similar to 
**std::vector**, but doesn't use exceptions for error reporting,
has more control over allocation behaviour, and is easier to debug
since it doesn't go crazy with template metaprogramming.

Arrays can be initialized in various ways:

```cpp
// a default-initialized array will not (pre-)allocate memory
Array<int> arr0;
// create a new array as copy of another array
Array<int> arr1(arr0);
// ...or by moving from another array
Array<int> arr2(std::move(arr0));
// ...or initialize content via C++11 initializer lists:
Array<int> arr3({ 1, 2, 3, 4, 5});

// you can also copy or move into existing arrays, this
// will replace any previous content:
Array<int> arr4;
arr4 = arr0;
arr4 = std::move(arr0);
```

New elements can be added to the back with **Add()** or in the
middle with **Insert()**:

```cpp
Array<float> arr0;
// add new items to the array:
arr0.Add(1.0f);
arr0.Add(3.0f);
// add a new item between the 2 existing elements at index 1:
arr0.Insert(1, 2.0f);
```

Adding or inserting items may reallocate memory, keep in mind that
**iterator invalidation rules** apply, just like with std::vector, if you
have a pointer (or reference) to an element in the array, and add elements
to the array the pointer (or reference) may now point to an invalid memory
location. So don't keep pointers or references into a dynamic array around
while adding or removing items!

You can also add elements with C++11 move and emplace semantics:

```cpp
// add a new object via std::move:
Array<MyMoveableClass> arr;
MyMoveableClass obj;
arr.Add(std::move(obj));

// directly emplace an object:
// MyClass has a constructor MyClass(int, int, int):
Array<MyClass> arr;
arr.Add(1, 2, 3);
// this is the same as:
add.Add(MyClass(1, 2, 3));
```

When adding more then a handful elements to an Array you should always
consider calling **Array::Reserve()** first to prevent excessive
reallocation:

```cpp
Array<int> arr;
// pre-allocate capacity for 128 items:
arr.Reserve(128);
// add 128 items, no additional allocations will happen:
for (int i = 0; i < 128; i++) {
    arr.Add(i);
}
```

You can **remove** elements in various ways:

```cpp
// remove an element from the front of the array, this
// will not move any objects around since the array
// is internally 'double-ended':
auto firstItem = arr.PopFront();
auto lastItem = arr.PopBack();

// erase the element at array index 3, this will
// move items to fill the hole, either from the front or
// from the back (whatever would be less move operations),
// this is slow, but the overall ordering of the items is preserved
// (if the items were sorted before the Erase, they will also remain
// sorted after the Erase)
arr.Erase(3);

// If the order of items in the array is not imported, it is faster
// to use EraseSwap(), this will fill the whole by swapping in
// the first or last item instead of moving N items around:
arr.EraseSwap(3);

// You can specifically choose to swap-in the front or back element too:
arr.EraseSwapFront(3);
arr.EraseSwapBack(3);
```

In addition to the Array::Reserve() method (which is usually the best
choice), there are some more methods to control memory allocation:

```cpp
// it is possible to set the minimal and maximal number of
// items the array should grow by, which cap the default
// growth value of half the current capacity:
Array<int> arr;

// grow at least by 512 items, but at most by 1024:
arr.SetAllocStrategy(512, 1024);

// you can trim the array capacity to its size, so
// that no memory is 'wasted', but this always involves
// a re-allocation, so it is rarely useful:
arr.Trim();

// Size() returns the number of valid items in the array:
int size = arr.Size();

// Capacity() is the overall allocated space, if Size()
// grows bigger then capacity, a reallocation will happen:
int cap = arr.Capacity();

// Spare() is the number of items that can be added before
// a reallocation happens (so spare = capacity - size):
int spare = arr.Spare();
o_assert(spare == cap - size);
```

The Array class has the usual .begin/.end methods which
makes it compatible with the new C++ ranged for loops
and most std algorithms (such as std::sort()):

```cpp
Array<MyClass> arr;
...
for (const auto& item : arr) {
    ...
}
...
// sort array content:
std::sort(arr.begin(), arr.end());

// or using a C++ lambda in reverse order:
std::sort(arr.begin(), arr.end(), [](int a, int b){
    return b < a;
});
```

### Map&lt;TYPE&gt;

(TODO)

### ArrayMap&ltTYPE&gt;

(TODO)

### Queue&lt;TYPE&gt;

(TODO)

### Set&lt;TYPE&gt;

(TODO)
