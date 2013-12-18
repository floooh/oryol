### String Atoms ###

String atoms provide read-only shared strings (each specific string only exists once in memory - per thread). Construction from a "raw string"
is relatively expensive (but not much more expensive then creating a std::string object), but copying and comparison is extremely fast, making string atoms ideal as keys in map 
and set containers.

It is important to know that the storage area behind a string atom is never released, and is guaranteed to remain
at the same memory location until the application exits. Thus it is not a good idea to handle completely random
strings (like GUIDs in string form) in long-running applications as string atoms, since memory usage would grow
infinitely. Instead the string atom system assumes that the number of different strings in an application are
finite (which they usually are).

Each thread has a thread-local string atom table which stores all string atoms which have been created in this thread.
String atoms may traverse thread boundaries, copying a "foreign" string atom into a thread-local string atom is more
expensive then copying a thread-local string atom to another thread-local atom. Likewise, comparing a "foreign"
string atom with a local string atom may involve an expensive string comparison instead of a very fast pointer
comparison (but only if their precomputed hash values collide).

#### Implementation Details ####

Strings in stringatom objects are assumed to be UTF-8.

A stringatom object only contains a single pointer to a data structure stored at a fixed location. This 'header' data
structure is shared between all string atoms referencing the same string and is only created for the
first stringatom containing that string.

The header data structure contains the following entries:

* a pointer to the (thread-local) stringatom table, this is used to check whether a string atom is from another thread
* a hash value which is computed when a string atom is created from a raw string, but is cached from then on and used for optimizing comparisons
* a pointer to the actual string data
* the string data follows the actual header, terminted by a 0-byte

These data structures (headers followed by actual string data) are stored in pre-allocated memory chunk managed by
the stringatom_buffer class. One chunk is usually several kilobytes, when this runs full, a new chunk is allocated.
Chunks are never freed.

The stringatom_buffer object is owned by a stringatom_table, which is a thread-local singleton.

The stringatom_table has a unordered_set<> (basically a bucketed hash table) with pointers to the header data
structures as keys and the precomputed hash values for hashing into the buckets.

##### What happens when creating or assigning from a raw string #####

This is implemented in the private method _stringatom::setup_from_c_string()_:

* the hash value of the input string is computed
* the unordered set in the thread-local string atom table is searched (inolves comparing the 
hash values, and on hash-collisions actual string-comparisons)
* if the string is already in the table we're done
* otherwise a new entry in the stringatom_buffer is created, this involves copying the computed hash and string data

#### What happens when comparing a string atom with another #####

This is implemented in the stringatom equality operator:

* if the 2 data pointers are identical then the string atoms live in the same thread and point to the 
same string (and we're done)
* otherwise, check if the 2 string atom table pointers in the header structure are identical, if yes, then
the string atoms are not equal
* otherwise the string atoms have been created in different threads and the strings may be identical even so their
data pointers are different, in this case:
* compare the hash values in the header structure, if they're different then the string are definitely not equal
* if the hashes are are identical, then an actual string comparison must be performed

It is expected that the majority of string atom comparisons are performed between string atoms belonging to the same
thread, which is in both cases (equality and inequality) very fast.

#### What happens when a string atom is copied into another string atom ####

This is very fast when the right-hand-side atom has been created in the local thread. The code
for assignment is in stringatom::copy():

* if the stringatom_table pointer of the right-hand-side stringatom is identical with the thread-local
stringatom_table, then only the header data pointer is copied and we're done
* otherwise the right-hand-side stringatom is from another thread, and a relatively expensive assignment from the raw string data
happens (see above)

#### What happens when a stringatom object is deleted ####

This is very fast since the actual storage of the string data and the stringatom_table entry is never freed.

### Code Samples ###

```cpp
#include "core/core.h"
#include "core/string/stringatom.h"

using namespace oryol;
using namespace string;

void main() {
  // this creates the global string atom table in the main thread (among others)
  core::setup();
  
  // create a few string atoms from raw strings
  stringatom atom0("BLA");
  stringatom atom1("BLA");
  stringatom atom2("BLUB");
  
  // atom0 and atom1 are identical, atom0 and atom2 are not
  o_assert(atom0 == atom1);
  o_assert(atom0 != atom2);
  
  // get a c string pointer from the atom
  const char* cstr = atom0.as_cstr();
  
  // get a std string from the atom
  std::string str = atom0.as_string();
  
  // cleanup behind us
  core::discard();
}
```
