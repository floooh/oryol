### Oryol Unit Testing ###

#### Configure and build with unit test support

Oryol integrates the UnitTest++ framework (http://unittest-cpp.sourceforge.net/). Unit testing
is turned off by default and must be enabled through cmake:

* start cmake-gui and press Configure
* check the ORYOL_UNITTESTS checkbox 
* press Configure again and verify that ORYOL_EXCEPTIONS and ORYOL_UNITTESTS_RUN_AFTER_BUILD are also checked
* press Generate to generate the build files

The generated build files now contains unit test projects (in an IDE these are grouped under "unittests"). The projects are configured to automatically run after building (unless the
ORYOL_UNITTESTS_RUN_AFTER_BUILD option is turned off), thus you can simply edit and build
the unit test code as usual. If a specific test fails, UnitTest++ will emit error messages
on the IDEs output console which usually can be double-clicked and jump directly to the
right source code line.

#### Writing unit tests ####

Each Oryol module should come with a set of unit tests:

* in the module's source directory, create a subdirectory called 'UnitTests'
* add a new unit test in the module's CMakeLists.txt file:

```cmake
    oryol_begin_unittest([ModuleName])
        oryol_sources(UnitTests)
        oryol_deps([additional dependencies])
    oryol_end_unittest()
```


* populate the 'unittests' directory with C++ files with the actual tests. Each C++ file can contain several tests, and there can be any number of files in the 'unittests' directory
* a minimal successful test looks like this:

```cpp
    //------------------------------------------------------------------------------
    //  [name].cc
    //  A short description
    //------------------------------------------------------------------------------
    #include "Pre.h"
    #include "UnitTest++/src/unittest++.h"

    TEST(MyTest) {
        CHECK(true);
    }
```

* configure and generate in cmake-gui again, the test has now been integrated into the build
