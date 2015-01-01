fips_begin_app(Paclone windowed)
    fips_files(
        Paclone.cc Paclone.jpg
        canvas.cc canvas.h
        draw.cc draw.h
        func.cc func.h
        game.cc game.h
        state.cc state.h
    )
    fips_dir(data)
    fips_files(sprites.py sprites.png)
    fips_dir(shaders)
    fips_files(shaders.py shaders.shd)
    fips_deps(Core Gfx Input Dbg)
    oryol_add_web_sample(Paclone "Simple Pacman clone (work in progress!" "emscripten,pnacl,android" Paclone.jpg "General/Paclone/Paclone.cc")
fips_end_app()
