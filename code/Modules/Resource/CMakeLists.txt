#-------------------------------------------------------------------------------
#   oryol Resource module
#-------------------------------------------------------------------------------
fips_begin_module(Resource)
    fips_files(
        Id.h
        Locator.cc Locator.h
        ResourceState.cc ResourceState.h
    )
    fips_dir(Core)
    fips_files(
        ResourcePool.h
        resourceContainerBase.cc resourceContainerBase.h
        resourceRegistry.cc resourceRegistry.h
        resourceBase.h
        resourceSlot.h
    )
    fips_deps(Core)
fips_end_module()

fips_begin_unittest(Resource)
    fips_dir(UnitTests)
    fips_files(
        IdTest.cc
        LocatorTest.cc
        ResourcePoolTest.cc
        resourceRegistryTest.cc
        StateTest.cc
    )
    fips_deps(Resource Core)
fips_end_unittest()
