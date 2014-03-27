### Design Basics ###

- OK: nested namespaces: oryol::[module_name]
- OK: explicitly NOT a framework for asset tools, only a game runtime (even only a 3d runtime in the beginning)
- OK: use the C++11 stdc++ lib, but not boost (update: except for containers, write custom containers instead)
- OK: all basic types are defined in the oryol namespace (oryol::int32, oryol::float32, ...)
- OK: ~~define our own ptr<> type, or use shared_ptr<> (update: use our own smart pointer and refcounted base class)
- OK: use cmake as meta-build-system
- OK: use UnitTest++ and use unit-testing right from the beginning
- OK: modules can depend on other modules, dependencies are defined both for compile time (in cmake files) and at runtime 
(modules attach other modules) update: not sure about the automatic runtime resolution of dependencies yet...
- TODO: optionally supports platforms without threading (emscripten)
- NO: ~~custom RTTI system, but without static initialisers~~ we'll just use the standard C++ rtti system, but sparingly, update: the standard RTTI system is almost useless (only useful for dynamic_cast, which is only useful when working around design errors... not sure yet whether I want to have the overhead of a custom RTTI system again...)
- UNDECIDED: usually one header file per module

### Memory Management ###

- OK: use object pools and placement new/delete for all Oryol::Core::RefCounted derived classes
- PROBABLY NOT ~~separate classes into thread-local (must be created / destroyed in same thread) and global (can be destroyed in other thread then creation)...?~~ not sure if this is worth the trouble... UPDATE: the custom poolAllocator is lockless and fast enough, so allocating an object in one thread and freeing it in another is not an issue

