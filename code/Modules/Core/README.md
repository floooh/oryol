## Core Module

TODO: general overview of the Core module.

### Basic Data Types and Constants:

Core/Types.h defines the following basic datatypes in the Oryol namespace:

```
/// signed 8, 16, 32 and 64-bit integers:
int8, int16, int32, int64

/// unsigned 8, 16, 32 and 64-bit integers:
uint8, uint16, uint32, uint64

/// 32- and 64-bit floats:
float32, float64

/// unsigned 8-bit character:
uchar

/// integers with as many bits as a pointers (allows to cast between pointers and integers)
intptr, uintptr
```

The following constants are defined (all resolve to -1):

```
InvalidIndex: used where an index is expected, but no valid index can be provided

EndOfString: used in string functions as "end of string" index

EndOfStream: used in IO streams as "end of stream" position
```




