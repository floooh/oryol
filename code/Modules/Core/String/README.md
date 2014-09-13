## String Handling in Oryol

By default Oryol strings are UTF-8, and only converted to and from wide-strings as needed (for instance when 
talking to APIs which don't support UTF-8).

Instead of having an all-in-one string class like std::string, Oryol offers several specialized string-related classes. 
The biggest difference to std::string is, that all Oryol string objects are immutable. String objects can be created, 
copied or moved, but not manipulated in-place. 

To manipulate string data, use the **StringBuilder** class.

To convert between UTF-8 and wide-string data, or to convert string data to and from simple data types, use the 
**StringConverter** class.

#### String Types

There are 3 basic string types in Oryol:

- **String**: immutable UTF-8 string
- **StringAtom**: application-wide unique, immutable UTF-8 strings, useful as keys
- **WideString**: immutable wchar_t string

Each of those string classes is useful in different ways:

The **String** class is the closest equivalent to std::string, with the exception that it is strictly immutable. 
Copying one String object to another doesn't duplicate the string data, instead only a pointer to the original 
data is copied and a reference count is incremented. The length of the string is cached internally, so 
String::Length() is very fast. **String** objects usually contain UTF-8 strings (however, a few functions 
are currently missing, for instance for counting the characters in an UTF-8 string, or locating the start of the 
next or previous UTF-8 character). Comparing **String** objects involves calling std::strcmp(), with a shortcut 
if the contained string-data pointer is identical (in this case it is guaranteed that the 2 strings are identical).

**StringAtom** is also an immutable 8-bit string, but is guaranteed to be unique in the whole application. This 
makes comparing StringAtoms extremely fast, since it is always a simple pointer comparison (with some caveats if 
the StringAtoms have been created in different threads, but this is a very unlikely case). StringAtoms are especially 
useful as keys in a Map<>. StringAtoms are relatively slow to create, but extremely fast to copy (and compare). 
Creation is still usually faster then creating a String object from raw string data though.

**WideString** is the least used string class, it contains an UTF-16 (on Windows) or UTF-32 (everywhere else) 
string. Wide strings are usually only used when talking to APIs which require this.

