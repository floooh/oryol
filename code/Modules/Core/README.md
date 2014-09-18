## Core Module

The Core module provides basic functionality which every Oryol app and Oryol module depends on:

* a unified application model for all target platforms
* lifetime management for heap-allocated objects
* a central logging facility
* a per-thread run-loop 
* selected alternative classes and concepts to the C++ std library tailored to game applications (containers, strings, low-level memory management)

### The Oryol Application Model

Oryol needs to run on platforms where applications are forbidden to 'own the game loop',
or block the application's main thread for more then a few dozen milliseconds. Instead of 
having a main() function with an 'infinite loop' these platforms require to register
a frame-callback which is called whenever a new frame needs to be rendered.

There are also platforms which have restrictions which operating system services are 
available from threads, and some platforms don't even have a main function.

In fact, most platforms except Windows and game consoles have at least some of these limitations.

Oryol's App class hides all these dirty details from the programmer by implementing
a simple application state model.

Creating a new Oryol application starts with deriving from the App class and overriding
the virtual state-callback methods which are called once per frame as long as this specific
state is active. The return value of a state callback method is the next state the application
should switch to. This way an application can do initialization in small chunks 
spread over several frames.

A minimal Oryol application looks like this:

```cpp
#include "Core/App.h"

using namespace Oryol;

class MyApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();    
};
OryolMain(MyApp);

//---: 1 frame initialization
AppState::Code
MyApp::OnInit() {
    // do initialization work
    ...

    // returning AppState::Running switches the App to the Running state
    return AppState::Running;
}

//---: per-frame callback
AppState::Code
MyApp::OnRunning() {
    // do per-frame work (e.g. 3D rendering)
    ...

    // either stay on Running state, or go into Cleanup state
    return done ? AppState::Cleanup : AppState::Running;
}

//---: 1 frame cleanup
AppState::Code
MyApp::OnCleanup() {
    // do cleanup-work, calling parent class OnCleanup is currently necessary
    return App::OnCleanup();
}
```

> NOTE: the current model of implementing per-frame callbacks through virtual
> methods may change in the future


### Class Annotation Macros

When looking through Oryol header files you may wonder about macros inside
class declarations like this:

```cpp
class MyClass : public BaseClass {
    OryolClassDecl(MyClass);
    OryolClassCreator(MyClass);
public:
    /// default constructor
    MyClass();
    ...
};
```

*OryolClassDecl* and *OryolClassCreator*  are 'class annotation macros' which add some 
Oryol-specific standard functionality to a class. 


### Object Lifetime Management

> NOTE: in general, allocating objects on the heap should be avoided if possible,
> instead Oryol's philosophy is to pre-allocate fixed sized memory blocks and
> object pools and keep dynamic allocations to a minimum

Oryol applications should try to keep heap-allocated objects to a minimum, but if
needed, Oryol's lifetime-managed heap objects using ref-counting and smart pointers
should be used instead of raw new and delete calls.

There's a few things to keep in mind when using ref-counted objects in Oryol:

1. to use ref-counting, a class must have a public *addRef()*, *release()* and *virtual destroy()* 
methods (for convenience, these are provided by the *RefCounted* base class)
2. the destructor must be virtual
3. a class annoation macro must be used to add creation methods to refcounted class
4. the Ptr<> class must be used as smart-pointer class

With this in place, creating an object then looks like this:

```cpp
Ptr<MyClass> myObj = MyClass::Create();
```

The *Create()* method is added by the OryolClassDecl() macro in the class declaration:

```cpp
class MyClass : public RefCounted {
    OryolClassDecl(MyClass);
public:
    ...
};
```

Non-default constructors can be invoked throught the Create method as well, and 
of course you can also use the new C++11 auto keyword:

```cpp
auto myObj = MyClass::Create(arg1, arg2, arg3);
```

> TODO: write about why std::shared_ptr and std::make_shared is not used.


### Object Pools

Object creation can be optimized with object pools which prevent dynamic memory
allocation per object (but still calls the constructor and destructor). This should
only be used for small objects which need to be very frequently created and destroyed
(which is something that should be avoided in a game engine). To object pools for
a class, use the OryolClassPoolAllocDecl annotation macro:

```cpp
class MyPoolClass : public MyClass {
    OryolClassPoolAllocDecl(MyClass);
public:
    ...  
};
```

The pool allocator will allocate objects in chunks of 256, up to 256 chunks, and will never
free claimed memory, so use this wisely.


### Deferred Object Creation

Sometimes the information of how to create an object must be handed around without actually
creating the object immediately. An object which knows how to create another object is
called a creator, and a class can be annotated to provide a static Creator() function
which returns a creator object:

```cpp
class MyClass : public RefCounted {
    OryolClassDecl(MyClass);
    OryolClassCreator(MyClass);
public:
    ... 
};
```

A creator is then created like this:

```cpp
// without constructor arguments:
auto creator = MyClass::Creator();

// with construction arguments
auto creator = MyClass::Creator(arg1, arg2, arg3);
```

A creator is actually a std::function object, so to create an object just invoke operator():

```cpp
// create object from previously created creator
Ptr<MyClass> myObj = creator();

// or more C++11 style:
auto myObj = creator(); 
```

### Logging

Oryol contains a central logging class **Log** with static logging methods. All Oryol text output goes through
this class. An application can attach custom logger objects derived from Logger to hook into the log messages.

Use the following methods for text output instead of printf:

```cpp
using namespace Oryol::Core;

// normal text output
Log::Info("Hello World!");
// warning-level output
Log::Warn("A printf style log message %d...\n", blarghl);
// error-level output
Log::Error("...\n");
// debug-level output
Log::Dbg("Some spammy debug-only message\n");

// set the log level so that only warnings and errors will be displayed
Log::SetLogLevel(Log::Warn);
```

The Log class can be called safely from any thread.

### Asserts

Instead of assert(), use Oryol specialized o_assert() macros, the standard form is 
also active when the code is compiled in release mode (with optimizations), the _dbg()
form is only active in debug mode:

```cpp
// standard assert, first form also active in release mode, second not:
o_assert(x != 0);
o_assert_dbg(x != 0);

// assert with additional text message:
o_assert(x != 0, "Message");
o_assert_dbg(x != 0, "Message");

// short-cut range check assert (tests that val >= 0 and val < max)
o_assert_range(val, max);
o_assert_range_dbg(val, max);
```

### The RunLoop

The main thread, and each thread created by Oryol has a thread-local RunLoop object. An 
application (or Oryol modules) can attach std::function objects to the run loop so that
this function is automatically called once per frame.

Here's an example using C++11 lambdas:

```cpp
Core::RunLoop()->Add([] {
    Core::Log("Hello!\n");
});
```

In a proper Oryol App, this should now print 'Hello!' to stdout 60 times per second.

> NOTE: there's currently no control over the order of how RunLoop callbacks are executed,
> and it is not defined whether the callback is called before or after the App's
> per-frame callback. These details will very likely change in the future.

