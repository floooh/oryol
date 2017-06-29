## LocalFS Module

The LocalFS module implements a filesystem plugin to load data from
the local disc via the C POSIX file functions (fopen, fread, etc).

Before it can be used, the LocalFS filesystem must be registered
with the IO module, the best time to do this is during setup:

```cpp
#include "IO/IO.h"
#include "LocalFS/LocalFileSystem.h"
...

AppState::Code
MyApp::OnInit() {
    IOSetup ioSetup;
    ioSetup.FileSystems.Add("file", LocalFileSystem::Creator());
    IO::Setup(ioSetup);
    ...
}
```

The LocalFileSystem initializes two path assigns:

- **root:** this is the directory where the executable is located
- **cwd:** this is the current working directory (aquired with the getcwd() function)

After setup, data can be loaded as usual, refer to the [IO module documentation](../IO/README.md) for more details.