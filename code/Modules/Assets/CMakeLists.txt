fips_begin_module(Assets)
    fips_vs_warning_level(3)
    fips_dir(Gfx)
    fips_files(
        MeshBuilder.cc MeshBuilder.h
        ShapeBuilder.cc ShapeBuilder.h
        VertexWriter.cc VertexWriter.h
        TextureLoader.cc TextureLoader.h
    )
fips_end_module()
