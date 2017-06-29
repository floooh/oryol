## HttpFS Module

The HttpFS module implements a filesystem plugin to load data from
web servers.

The HTTP filesystem needs to be registered with the IO module before
it can be used, the best way to do this is when the IO module is
setup. It also makes sense to define one or several path assigns.

```cpp
#include "IO/IO.h"
#include "HttpFS/HTTPFileSystem.h"
...

AppState::Code
MyApp::OnInit() {
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("http", HttpFileSystem::Creator());
    ioSetup.Assigns.Add("data", "http://floooh.github.com/oryol/data");
    IO::Setup(ioSetup);
    ...
}
```

A few things to note:

- IOSetup.FileSystems takes a filesystem **Creator** object, not a filesystem object, this is because (depending on platform) multiple filesystem objects may be created (one per IO thread)
- the URL scheme "http" is usually used with the HTTPFileSystem, but you can choose any scheme you want
- on the HTML5 platform, the host address part of an URL is discarded, data will always be loaded from the same location where the main page is hosted, this is because of cross-origin restrictions

After the HTTPFileSystem has been setup, data can be loaded as usual, refer to the [IO module documentation](../IO/README.md) for more details.