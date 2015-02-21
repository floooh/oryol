fips_begin_module(Assets)
    fips_dir(Gfx)
    fips_files(
        MeshBuilder.cc MeshBuilder.h
        ShapeBuilder.cc ShapeBuilder.h
        VertexWriter.cc VertexWriter.h
    )
    fips_deps(Gfx)
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


