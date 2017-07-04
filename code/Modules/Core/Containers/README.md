## Oryol Container Classes

Oryol has it's own set of container classes which don't use exception, are
easier to debug and allow better tweaking for Oryol's requirements than the
C++ std library container classes.

### Buffer

The **Buffer** class is a simple dynamic container for an 'untyped' memory
chunk on the heap. When the Buffer object is deleted, the heap data will be
freed. A Buffer object cannot be _copied_, only _moved_. This is to prevent
accidential expensive memory allocation and duplicate data copies.

See the [Buffer Unit Test](../UnitTests/BufferTest.cc) for
usage examples code.

### StaticArray&lt;TYPE&gt;

A StaticArray is a a constant-size C-style array with bounds-checking
(similar to the new std::array in C++11). Use this in places where
you would use a C array, but want to have bounds-checking performed.
However keep in mind that the bounds-checks are only
performed in debug mode, not in (optimized) release mode!

For StaticArray code samples, see the [Unit Test](../UnitTests/StaticArrayTest.cc). 

### Array&lt;TYPE&gt;

The **Array** class is a dynamic, linear container similar to 
**std::vector**, but doesn't use exceptions for error reporting,
has more control over allocation behaviour, and is easier to debug
since it doesn't go quite as crazy with template metaprogramming.

See the [Array Header File](Array.h) and 
[Array Unit Test](../UnitTests/ArrayTest.cc) for more
information and code samples.

### Map&lt;KEYTYPE,VALUETYPE&gt;

The **Map** class is Oryol's version of std::map, with one important
difference: trying to access an element with operator\[\] creates
a fatal error if the element doesn't exists (instead of adding
a new element like std::map).

Check out the [Map Header File](Map.h) and [Unit Test](../UnitTests/MapTest.cc)
for more information and code samples.

### ArrayMap&lt;KEYTYPE,VALUETYPE&gt;

The **ArrayMap** class combines features of the Array and Map class.
It keeps value-items in a separate Array in the order they were added,
but also allows a fast key-lookup by maintaining a separate Map
which maps keys to array indices. The downsides are:

- value and key items live in 2 separate heap memory chunks
- Erase() operations are fairly slow

Use an ArrayMap if:

- you need to preserve the original order in which items were added
- or if the value-type is expensive to move, since adding new elements
will never involve moving value objects around

For more info, see the [ArrayMap Header File](ArrayMap.h), and for
code samples see the [ArrayMap Unit Test](../UnitTests/ArrayMapTest.cc).

### Queue&lt;TYPE&gt;

This is a simple FIFO queue on top of
a dynamic array. See the [Header File](Queue.h) and
[Unit Test](../UnitTests/QueueTest.cc) for more 
information.

### Set&lt;TYPE&gt;

This is a dynamic, sorted array which only allows adding
unique elements. trying to add an identical item
twice results in a fatal runtime error.

See the [Header File](Set.h) and [Unit Test](../UnitTests/Set.cc) for more information.

### InlineArray&lt;TYPE,CAPACITY&gt;

The InlineArray class is similar to the Array class
but doesn't allocate memory from the heap. The max capacity is fixed
and must be provided as a template argument. A fatal runtime error
will be thrown when attempting to add new items to a full array.

### Slice&lt;TYPE&gt;

A Slice is an array without its own data, instead it
provides an array-like interface to a chunk of non-owned
memory. The same concept also exists under different names,
like array-views or ranges.

> NOTE: Since Slices do not own the memory they are pointing to, they are
potentially dangerous to use. The underlying memory may have been moved or
freed (similar to how iterators are affected by iterator-invalidation).
Either make sure that the items referenced by Slices are 'pinned' into place,
or use Slices only as a short-lived, transient reference.

