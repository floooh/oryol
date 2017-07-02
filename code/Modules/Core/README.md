## Core Module

**Updated: 30-Apr-2016**

The Core module provides core functionality useful for most Oryol application:

* a unified application model across all target platforms
* text logging
* time measurement
* memory managament functions
* macros for attaching realtime profilers
* per-thread run-loops
* lifetime management for heap-allocated objects
* an optional per-class RTTI system
* custom assert macros with callstacks
* container classes that replace the std library containers
* string and UTF conversion helper classes

### The Oryol Application Model

Oryol needs to run on all types of esoteric host platforms where applications may be forbidden to 
'own the game loop', block the main thread for more then a few dozen milliseconds, specific functionality
is only available from the main (or UI-) thread, don't have shared-memory-threading, don't
even have a main() function or where an app can be suspended and restored at will by 
the operating system.

To hide all these platform-specific details, Oryol offers a convenient application
class *Oryol::App* which implements a simple *application-state-model*. During the lifetime
of an Oryol application, the app goes through different states, controlled by a derived 
application subclass. While a state is active, an associated per-frame method is called
which implements a frame's worth of work for the current application state. The only
hard rule is that these callback methods must not block for more then a few dozen milliseconds
(the usual frame budget is 16ms or 32ms).

Creating a new Oryol application starts with deriving an App subclass and overriding
the virtual state-callback methods which are called once per frame as long as the associated state is 
active. The return value of a state callback method is the next state the application
should switch to. 

A minimal Oryol application looks like this:

```cpp
#include "Core/Main.h"

using namespace Oryol;

class MyApp : public App {
public:
    AppState::Code OnInit();
    AppState::Code OnRunning();
    AppState::Code OnCleanup();    
};
// this declares MyApp as the Oryol main class
OryolMain(MyApp);

//---: 1 frame initialization
AppState::Code MyApp::OnInit() {
    // do initialization work
    ...

    // returning AppState::Running switches the App to the Running state
    return AppState::Running;
}

//---: per-frame callback
AppState::Code MyApp::OnRunning() {
    // do per-frame work (e.g. 3D rendering)
    ...

    // either stay on Running state, or go into Cleanup state
    return done ? AppState::Cleanup : AppState::Running;
}

//---: 1 frame cleanup
AppState::Code MyApp::OnCleanup() {
    // do cleanup-work, calling parent class OnCleanup is currently necessary
    return App::OnCleanup();
}
```

> NOTE: the current model of implementing per-frame callbacks through virtual
> methods may change in the future

### Logging

Oryol contains a central logging class **Log** with static logging methods. All Oryol text output goes through
this class. An application can attach custom logger objects derived from Logger to hook into the log messages.

Use the following methods for text output instead of printf:

```cpp
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

Instead of assert(), use Oryol's specialized o\_assert() macros, the standard form is 
also active when the code is compiled in release mode (with optimizations), the \_dbg()
form is only active in debug mode:

```cpp
// standard assert, first form also active in release mode, second not:
o_assert(x != 0);
o_assert_dbg(x != 0);

// assert with additional text message:
o_assert2(x != 0, "Message");
o_assert2_dbg(x != 0, "Message");

// short-cut range check assert (tests that val >= 0 and val < max)
o_assert_range(val, max);
o_assert_range_dbg(val, max);
```

On some platforms, o\_assert() will log callstack dumps for better post-mortem
debugging.

### RunLoops

The main thread, and each thread created by Oryol has two thread-local run-loop
lists, one executed before the App's on-frame method, one after. An application
(or Oryol modules) can attach std::function objects to the run loop so that
this function is automatically called once per frame.

Here's an example using C++11 lambdas:

```cpp
Core::PreRunLoop()->Add([] {
    Core::Log("Hello!\n");
});
```

In a proper Oryol App, this should now print 'Hello!' to stdout 60 times per second.

> NOTE: there's currently no control over the order of how RunLoop callbacks are executed in relation to each other.

### Accessing Command Line Arguments

On some platforms, a global object _OryolArgs_ provides access to command line arguments:

```cpp
#include "Core/Main.h"
...
AppState::Code MyApp::OnInit() {

    if (OryolArgs.HasArg("--version")) {
        Log::Info("version: 1.0.0\n");
    }
    const int DefaultLogLevel = 3;
    int logLevel = OryolArgs.GetInt("--log", DefaultLogLevel);
    String filename = OryolArgs.GetString("--file");
    if (filename.IsValid()) {
        ...
    }
    ...
}

```

### Time Measurement

The Core module offers 3 classes that deal with time measurement:

- **Clock**: a high-resolution time source
- **TimePoint**: a point in time
- **Duration**: a time duration

Sample code:

```cpp
#include "Core/Time/Clock.h"
...

    // return current time (no relation to wall-clock time,
    // it is only useful to compute time differences)
    TimePoint t0 = Clock::Now();
    ...
    TimePoint t1 = Clock::Now();
    
    // compute the time difference between t1 and t0
    Duration d0 = t1 - t0;

    // the method Clock::Since() is a shortcut of the above
    Duration d1 = Clock::Since(t0);

    // the Duration class has methods to return the time span 
    // as seconds, milli- and microseconds
    double sec = d0.AsSeconds();
    double ms  = d0.AsMilliSeconds();
    double us  = d0.AsMicroSeconds();

    // the method Clock::LapTime() can be used to measure
    // frame duration, it needs a TimePoint variable that persists between
    // frames to store the current frame time in
    static TimePoint tp;
    Duration frameDuration = Clock::LapTime(tp);
...

```

### String Handling

See the [Core Module String documentation](String/README.md) for detailed
information about string handling in the Oryol Core module. 

### Memory Management

The header [Core/Memory/Memory.h](Memory/Memory.h) contains static 
helper functions for memory management.

Currently these functions use the std library function (like
std::malloc, std:free, etc). At a later time it will be possible to
override these functions with your own implementation.

### Containers

See the [Core Module Containers documentation](Containers/README.md) for
detailed information about the container classes in the Oryol Core module.

### Things you should NOT use

There are a couple of C++ features which are black-listed on Oryol for various reasons:

- *C++ exceptions*: These are disabled by default, but can be enabled with a
  cmake option. Don't use them as they are fairly expensive on emscripten.
- *std containers (std::vector, std::map, ...)*: Oryol comes with its own set
  of containers classes which offer more fine-control over their behaviour and
  use asserts instead of exceptions.
- *C++ style input/output (e.g. std::cout << "hello")*: This causes a lot of
  code to be pulled into the executable, which is especially bad in emscripten
  where client-size matters.

## Heap Objects, Lifetime Management, RTTI...

> NOTE: Heap-allocated objects play a minor role in Oryol, you will hardly
ever use heap objects in typical Oryol code, thus all the following information
isn't required knowledge when working with Oryol, feel free to stop
reading here :)

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

1. classes which use ref-counting should be subclasses of RefCounted (or provide a specific set of methods to manage the refcount, but that's an advanced topic)
2. a class annotation macro must be used to add creation methods to refcounted class
3. the Ptr<> class must be used as smart-pointer class

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

Non-default constructors can be invoked through the Create method as well, and
of course you can also use the new C++11 auto keyword:

```cpp
auto myObj = MyClass::Create(arg1, arg2, arg3);
```

> NOTE: Always keep in mind that there should be a good reason to use heap-allocated, 
> ref-counted objects instead of stack-allocated or class-embedded objects. Always consider
> stack-allocated objects and class-embedded objects first!

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

### RTTI

Runtime Type Information in Oryol is optional and per-class. The standard
C++ RTTI language feature is not used and by default disabled (it can be enabled with
the cmake option 'FIPS\_RTTI').

RTTI is usually only necessary for safe downcasting from a parent class pointer
to a subclass pointer and thus is rarely needed. If RTTI is needed,
classes have to be annotated with the **OryolTypeDecl** and **OryolBaseTypeDecl**
macros. The latter is needed in thr root class of an RTTI-enabled class
hierarchy, and the former in derived classes, e.g.:

```cpp
// a base class in an RTTI-enabled class hierarchy:
class A : public RefCounted {
    OryolClassDecl(A);
    OryolBaseTypeDecl(A);
    ...
};

// an RTTI-enabled subclass of A
class B : public A {
    OryolClassDecl(B);
    OryolTypeDecl(B, A);
public:
    ...
};

// another RTTI-enabled subclass of A
class C : public A {
    OryolClassDecl(C);
    OryolTypeDecl(C, A);
public:
    ...
```

You can test whether an object is type-compatible (same class or parent class)
with the **IsA()** template method:

```cpp
auto a = A::Create();
auto b = B::Create();
auto c = C::Create();

// this is true since B is a subclass of A:
if (b->IsA<A>()) {
    // yep
}

// this is also true since C is a subclass of A:
if (c->IsA<A>()) {
    // yep
}

// this is false since B and C are not related:
if (b->IsA<C>()) {
    // nope
}

// this is false since A is not derived from B
if (a->IsA<B>()) {
    // nope
}

// testing for the actual class is also true of course
if (a->IsA<A>()) {
    // yep
}
```

For safe downcasting, use the **DynamicCast()** method, currently this
behaves like a standard *dynamic\_cast()* by returning a nullptr if the
cast is not valid (NOTE: this may change in the future, and a fatal
error may be thrown on an incompatible downcast):

```cpp
// assign Ptr<B> to a Ptr<A> and safely downcast back to a Ptr<B>
Ptr<A> ab = b;                      // this is an implicit static cast checked at compile-time
Ptr<B> bb = ab->DynamicCast<B>();   // performs a runtime check
o_assert(bb);                       // this checks out

// assign Ptr<B> to a Ptr<A>, and try to cast to Ptr<C>, which fails
Ptr<A> ab = b;
Ptr<C> cb = ab->DynamicCast<C>();   
o_assert(cb);                       // this will fail
```

Oryol's RTTI system is small and fast, there's one byte of static storage added
per RTTI-enabled class, and an RTTI check only involves a pointer-comparisions.
