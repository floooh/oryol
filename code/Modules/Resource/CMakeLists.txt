#-------------------------------------------------------------------------------
#   oryol Resource module
#-------------------------------------------------------------------------------
fips_begin_module(Resource)
    fips_files(
        Id.h
        Locator.cc Locator.h
        ResourcePool.h
        ResourceRegistry.cc ResourceRegistry.h
        ResourceState.cc ResourceState.h
        loaderFactory.h
        resourceBase.h
        resourceSlot.h
        simpleFactory.h
    )
    fips_deps(Core)
fips_end_module()

fips_begin_unittest(Resource)
    fips_dir(UnitTests)
    fips_files(
        IdTest.cc
        LocatorTest.cc
        ResourceRegistryTest.cc
        StateTest.cc
    )
    fips_deps(Resource Core)
fips_end_unittest()
