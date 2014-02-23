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

