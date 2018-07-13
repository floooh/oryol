## IO Module

The Oryol IO module allows to load data asynchronously from web servers, 
the local host filesystem, or other places if extended by custom 'pluggable
filesystems'.

### Design Philosophy

The IO module focuses on games that load data asynchronously. It does not
support general filesystem operations that might be required by tools or
other application types.

This means:

- reading data is more important than writing data
- directory operations are not supported
- the concepts of a current working directory and relative paths
  are not supported
- there is no concept of file attributes (e.g. read/write flags,
  ownership or timestamps)

In general, the IO module uses HTTP as a 'design blueprint' instead of
traditional file access APIs (like the POSIX file functions).

### IO Concepts

There are 3 basic concepts to understand when working with the
IO module:

- **assigns** as abstract filesystem locations
- **URLs** instead of paths
- **pluggable filesystems** which do the actual work

#### Assigns (aka Path Aliases)

A path **assign** is an old AmigaOS concept which simplifies working with 
filesystem paths. Technically they are simply a short string at the start
of a path which are replaced (or 'resolved') at runtime with actual directory 
names.

Here's a simple example to explain the concept:

Let's say a game has all its textures in a special asset subdirectory called
_assets/textures_. The game would define an assign called _tex:_ pointing to
that directory, and texture paths would be defined like this:

```
"tex:wood.dds"
"tex:brick.dds"
...
```

Before those textures are actually loaded, the _tex:_ assign is replaced with
an absolute directory location (which may differ depending on host platform and
where the user has installed the game), for instance:

```
tex:wood.dds => C:/Program Files (x86)/My Game/assets/textures/wood.dds
tex:brick.dds => C:/Program Files (x86)/My Game/assets/textures/brick.dds
```

Assigns are resolved recursively, this is useful to remove any hardcoded 
absolute paths from an application. For instance, the LocalFS filesystem -
which wraps access to the local hard disc - defines a _root:_ assign
pointing to the directory where the application executable is located. The
above _tex:_ assign would then point to _root:assets/textures_ instead
of some hardcoded absolute path, and the resolve happens in 2 steps:

```
tex:wood.dds => root:assets/textures/wood.dds => C:/Program.../assets/textures/wood.dds
```

#### URLs

After the assign-resolve operation all paths in Oryol are full URLs. This makes
it easier to load data from other places than the local hard disc, like web
servers, communicating with HTTP REST services or implementing more
exotic ways to load data.

An Oryol URL consists of the following parts (most of them optional):
```
scheme://user:password@host:port/path#fragment?query
```

A typical HTTP URL could look like this:
```
http://floooh.github.com/oryol/data/lok_dxt1.dds
```

A typical host filesystem URL on a Windows system would look like this:
```
file:///C:/Program Files/Bla/blub/blob.txt
```

And the same on a UNIX-like system (e.g. OSX):
```
file:////Users/floh/Bla/blub/blob.txt
```

Keep in mind that high-level code shouldn't directly work with paths in URL
format, but instead use assign prefixes which wrap the underlying URL.

#### Pluggable Filesystem Implementations

The Oryol IO module itself doesn't contain any code that talks
to host-platform IO-APIs, instead it only forwards IO requests to pluggable 
filesystem wrappers which are derived from the IO::FileSystem class.

Pluggable filesystems are associated with an URL scheme either
at startup or later through the IO::RegisterFileSystem() function. At the
time of writing, Oryol comes with 2 standard filesystem implementations:

* **HTTPFileSystem**: this is implemented in the HTTP module and is used to
  load data from web servers, it is usually associated with the **http:**
  URL scheme
* **LocalFileSystem**: this is implemented in the LocalFS module and loads data
  through POSIX file functions, it is usually associated with the **file:**
  URL scheme

### Working with the IO module

#### Initialization and Shutdown

Before attempting to load data, the IO module must be initialized by 
calling the IO::Setup() function. IO::Setup() takes an IOSetup object
as argument which allows to define the initial set of filesystems
and assigns.

The following code associates the HTTP filesystem with the 'http' 
scheme and defines an assign 'data:' which points to a web server location:

```cpp
#include "IO/IO.h"
#include "HTTP/HTTPFileSystem.h"
...
IOSetup ioSetup;
ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
ioSetup.Assigns.Add("data:", "http://floooh.github.com/oryol/data/");
IO::Setup(ioSetup);
```

Note that you don't provide a HTTPFileSystem _object_, but instead
a _creator object_. This is necessary because the HTTPFileSystem
may be instantiated multiple times in separate IO threads (where and
how often a FileSystem object is instantianed is an implementation detail
which differs between platforms).

> NOTE: There's one important caveat on HTML5:
> both platforms are limited by the 'same-origin policy', which basically
> means that a web page must only load assets from the same domain it was
> served from. For this reason, the current implementation of HTTPFileSystem
> for those platforms ignores the URL host address. It is not possible
> to load data from other domains.

At application shutdown, call the **IO::Discard()** method, this will
cancel any pending IO requests and cleanly shutdown any IO threads.

#### Loading Data

The easiest way to load data is through the **IO::Load()** function.  This
takes an URL and at least one callback function which is called after the 
data has been successfully loaded. The callback function(s) are usually
provided as a C++11 lambda:

```cpp
IO::Load("tex:wood.dds", [](IO::LoadResult res) {
    // the file tex:wood.dds has been successfully loaded, and
    // the data and original URL is provided in the LoadResult object:
    //      res.Data - a Memory::Buffer object with the loaded data
    //      res.Url  - the original URL
    Log::Info("'%s' has been loaded!\n", res.Url.Path().AsCStr());

    // get pointer to data and size and do something with it...
    const uint8_t* ptr = res.Data.Data();
    const int size = res.Data.Size();
    ...
    
    // note that the data will vanish when this function returns,
    // if you need to keep hold of it, either copy the data or move 
    // the Memory::Buffer object to a persistent location
});
```

Sometimes it is useful to load a whole group of related files in one
swoop and only be called back when all files have been successfully
loaded. This is implemented by the **IO::LoadGroup()** function:

```cpp
    IO::LoadGroup(Array<URL>({"tex:wood.dds","tex:brick.dds","tex:metal.dds"}),
        [](Array<IO::LoadResult> results) {
            // all 3 files have been successfully loaded
            for (const auto& res : results) {
                Log::Info("'%s' has been loaded!\n", res.Url.Path().AsCStr());
                const uint8_t* ptr = res.Data.Data();
                const int size = res.Data.Size();
                ...
            }
        });
```

An application probably wants to know about loading errors. This is done by
providing a second 'failed-callback' to the **IO::Load()** or
**IO::LoadGroup()** which is called if something goes wrong:

```cpp
// asynchronously load wood.dds, with a second 'failure-callback'
IO::Load("tex:wood.dds", 
    // this is the success-callback
    [](IO::LoadResult res) {
        ...
    },
    // the failure callback, this gets the URL of the file
    // that failed loading, and an IOStatus, which maps directly
    // to HTTP status codes (e.g. IOStatus::NotFound is 404)
    [](const URL& url, IOStatus::Code ioStatus) {
        Log::Warn("Failed to load file '%s' with status '%s'!\n",
            url.Path().AsCStr(), IOStatus::ToString(ioStatus));
    });
```

In the **IO::LoadGroup()** function, the failure callback may be called
multiple times (once per file that fails to load).

### Advanced Topics

#### Switch between loading data from disc or web

One interesting use-case of assigns is to switch to a completely different
place where data is loaded from with a single line of code without touching
any of the actual loading code.

For instance to switch between loading data from the local hard disc versus a
web server based on a preprocessor define:

```cpp
IOSetup ioSetup;
ioSetup.FileSystems.Add("http", HTTPFileSystem::Creator());
ioSetup.FileSystems.Add("file", LocalFileSystem::Creator());
#if LOAD_DATA_FROM_WEB
ioSetup.Assigns.Add("data:", "http://mygame.com/data/");
#else
// root: is defined by LocalFileSystem and points to our executable's directory
ioSetup.Assigns.Add("data:", "root:data/");
#endif

// load data from 'anywhere'
IO::Load("data:wood.dds", [](IO::LoadResult res) {
    ...
});
```

#### Loading data with IO messages and polling

This is a lower-level way to asynchronously load data which sometimes
may provide more control than the callback method described above:

Call the **IO::LoadFile()** method which takes an URL as input
and immediately returns an **Ptr&lt;IOProtocol::Read&gt;** message object.
The caller needs store this object and frequently check it
for completion by calling the **Handled()** method. Once this flips
to true, the message object contains the loaded data (in case of success)
or error information (in case of failure).

After success or failure is handled, the message object should be discarded
by clearing the smart pointer.

```cpp

// need to store Ptr<Oryol::IORead> at a persistent place
class MyClass {
    ...
    Ptr<Oryol::IORead> ioRequest;
    ...
};

// start loading a file through IO::LoadFile() and store the returned message
this->ioRequest = IO::LoadFile("tex:wood.dds");

// frequently check whether the IO request has finished (e.g. once per frame)
if (this->ioRequest != nullptr && this->ioRequest->Handled) {
    // success?
    if (IOStatus::OK == this->ioRequest->Status) {
        // do something with the loaded data...
        const void* ptr = this->ioRequest->Data.Data();
        const int size  = this->ioRequest->Data.Size();
        ...
    }
    else {
        // failure... print an error message
        Log::Warn("Failed to load '%s' with '%s'!\n",
            this->ioRequest->Url.Path().AsCStr(),
            IOStatus::ToString(this->ioRequest->Status));
    }
    // don't forget to discard the IO request (we kept the
    // object in a smart pointer, so assigning a nullptr will do the 
    // right thing)
    this->ioRequest = nullptr;
}
```

#### Loading data in chunks

**TODO**: mention HTTP-style range-requests for chunk-loading large files
or data streaming.

#### Writing data

**TODO**: describe the IO::WriteFile() method

#### Implementing your own filesystem

**TODO**: implementing FileSystem subclasses and custom IO messages



