## Core Module

TODO: general overview of the Core module.

### Basic Data Types and Constants:

Core/Types.h defines the following basic datatypes in the Oryol namespace:

* *int8, int16, int32, int64*: signed 8, 16, 32 and 64-bit integers
* *uint8, uint16, uint32, uint64*: unsigned 8, 16, 32 and 64-bit integers
* *float32, float64*: 32- and 64-bit floating point numbers
* *intptr, uintptr*: an integer with as many bits as a pointer (32 or 64 bits)
* *uchar*: an unsigned 8-bit character

Use the standard wchar_t for wide-characters, but be aware that this is 2 bytes on Windows, and 4 bytes everwhere else.

The following constants are defined (all resolve to -1):

* *InvalidIndex*: used where an index is expected, but no valid index can be provided
* *EndOfString*: used in string functions as "end of string" index
* *EndOfStream*: used in IO streams as "end of stream" position

### String Handling

By default Oryol strings are UTF-8, and only converted to and from wide-strings as needed (for instance when talking to APIs which don't support UTF-8).

Instead of having an all-in-one string class like std::string, Oryol offers several specialized string-related classes. The biggest difference to std::string is, that all Oryol string objects are immutable. String objects can be created, copied or moved, but not manipulated in-place. 

To manipulate string data, use the *Core::StringBuilder* class.

To convert between UTF-8 and wide-string data, or to convert string data to and from simple data types, use the *Core::StringConverter* class.

#### String Types

There are 3 basic string types in Oryol:

- *Core::String*: immutable UTF-8 string
- *Core::StringAtom*: application-wide unique, immutable UTF-8 strings, useful as keys
- *Core::WideString*: immutable wchar_t string
- 
Each of those string classes is useful in different ways:

The *Core::String* class is the closest equivalent to std::string, with the exception that it is strictly immutable. Copying one Core::String object to another doesn't duplicate the string data, instead only a pointer to the original data is copied and a reference count is incremented. The length of the string is cached internally, so Core::String::Length() is very fast. *Core::String* objects usually contain UTF-8 strings (however, a few functions are currently missing, for instance for counting the characters in an UTF-8 string, or locating the start of the next or previous UTF-8 character). Comparing *Core::String* objects involves calling std::strcmp(), with a shortcut if the contained string-data pointer is identical (in this case it is guaranteed that the 2 strings are identical).

*Core::StringAtom* is also an immutable 8-bit string, but is guaranteed to be unique in the whole application. This makes comparing StringAtoms extremely fast, since it is always a simple pointer comparison (with some caveats if the StringAtoms have been created in different threads, but this is a very unlikely case). StringAtoms are especially useful as keys in a Core::Map<>. StringAtoms are relatively slow to create, but extremely fast to copy (and compare). Creation is still usually faster then creating a String object from raw string data though.

*Core::WideString* is the least used string class, it contains an UTF-16 (on Windows) or UTF-32 (everywhere else) string. Wide strings are usually only used when talking to APIs which require this.

### Logging and Asserts

Oryol contains a central logging class *Core::Log* with static logging methods. All Oryol text output goes through
this class. An application can attach custom logger objects derived from Core::Logger to hook into the log messages.

Use the following methods for text output instead of printf:

```cpp
using namespace Oryol::Core;

// normal text output
Log::Info("Hello World!");
// warning-level output
Log::Warn("Earth will be destroyed in %d seconds...\n", secsToDestruction);
// error-level output
Log::Error("Something terrible has happend...\n");
// debug-level output
Log::Dbg("Some spammy debug-only message\n");

// set the log level so that only warnings and errors will be displayed
Log::SetLogLevel(Log::Warn);
```

The Log class is supposed to be thread-safe.

### The RunLoop

(TODO)

### Ref-counting and smart-pointers

(TODO)

### Containers 

(TODO)

