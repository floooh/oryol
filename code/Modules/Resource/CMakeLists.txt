#-------------------------------------------------------------------------------
#   oryol Resource module
#-------------------------------------------------------------------------------
fips_begin_module(Resource)
    fips_vs_warning_level(3)
    fips_files(
        Id.h
        Locator.cc Locator.h
        ResourceLabel.h
        ResourceState.h
        ResourceLoader.cc ResourceLoader.h
        ResourcePool.h
        SetupAndData.h
        ResourceContainerBase.cc ResourceContainerBase.h
        ResourceRegistry.cc ResourceRegistry.h
        ResourceBase.h
    )
    fips_deps(Core)
fips_end_module()

fips_begin_unittest(Resource)
    fips_vs_warning_level(3)
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
