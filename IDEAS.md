### Design Basics ###

- [x] nested namespaces: oryol::[module_name]
- [x] explicitely NOT a framework for asset tools, only a game runtime (even only a 3d runtime in the beginning)
- [x] use the C++11 stdc++ lib, but not boost
- [x] all basic types are defined in the oryol namespace (oryol::int32, oryol::float32, ...)
- [x] ~~define our own ptr<> type, or use shared_ptr<> ? at least a typedef because shared_ptr is unwieldy~~ we just use shared_ptr, unique_ptr and weak_ptr
- [x] use cmake as meta-build-system
- [x] use UnitTest++ and use unit-testing right from the beginning
- [x] modules can depend on other modules, dependencies are defined both for compile time (in cmake files) and at runtime 
(modules attach other modules)
- [x] optionally supports platforms without threading (emscripten)
- [ ] standardized oryol::app class, setup runtime environment by adding required modules to the app (and modules add 
the modules dependent on them)
- [x] ~~custom RTTI system, but without static initialisers~~ we'll just the the standard C++ rtti system, but sparingly
- [ ] usually one header file per module

### Memory Management ###

- [ ] use object pools and placement new/delete for all oryol::core::base derived classes
- [ ] ~~separate classes into thread-local (must be created / destroyed in same thread) and global (can be destroyed in 
other thread then creation)...?~~ not sure if this is worth the trouble...

