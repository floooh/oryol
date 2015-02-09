fips_begin_module(Asset)
    fips_dir(Util)
    fips_files(
        MeshBuilder.cc MeshBuilder.h
        ShapeBuilder.cc ShapeBuilder.h
        VertexWriter.cc VertexWriter.h
    )
    fips_deps(IO Gfx Resource)
fips_end_module()

fips_begin_unittest(Asset)
    fips_dir(UnitTests)
    fips_files(
        MeshBuilderTest.cc
        ShapeBuilderTest.cc
        VertexWriterTest.cc
    )
    fips_deps(Asset)
fips_end_unittest()


