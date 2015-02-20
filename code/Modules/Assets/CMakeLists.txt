fips_begin_module(Assets)
    fips_files(
        AssetsSetup.h
        Assets.h Assets.cc
        AssetId.cc AssetId.h
    )
    fips_dir(Core)
    fips_files(
        assetLoaderRegistry.cc assetLoaderRegistry.h
        assetCreator.h
        AssetLoader.cc AssetLoader.h
    )
    fips_dir(Gfx)
    fips_files(
        assetCreator_Gfx.cc
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
        MeshBuilderTest.cc
        ShapeBuilderTest.cc
        VertexWriterTest.cc
    )
    fips_deps(Assets)
fips_end_unittest()


