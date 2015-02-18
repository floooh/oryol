fips_begin_module(Assets)
    fips_files(
        AssetsSetup.h
        Assets.h Assets.cc
        AssetId.cc AssetId.h
    )
    fips_dir(Core)
    fips_files(
        assetRegistry.cc assetRegistry.h
        assetLoaderRegistry.cc assetLoaderRegistry.h
        assetCreator.h
        assetCreator_Gfx.cc
        AssetLoader.cc AssetLoader.h
    )
    fips_dir(Util)
    fips_files(
        MeshBuilder.cc MeshBuilder.h
        ShapeBuilder.cc ShapeBuilder.h
        VertexWriter.cc VertexWriter.h
    )
fips_end_module()

fips_begin_unittest(Assets)
    fips_dir(UnitTests)
    fips_files(
        assetRegistryTest.cc
        MeshBuilderTest.cc
        ShapeBuilderTest.cc
        VertexWriterTest.cc
    )
    fips_deps(Assets)
fips_end_unittest()


