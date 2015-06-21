fips_begin_app(Paclone windowed)
    fips_vs_warning_level(3)
    fips_files(
        Paclone.cc Paclone.jpg
        canvas.cc canvas.h
        draw.cc draw.h
        func.cc func.h
        game.cc game.h
        state.cc state.h
        sound.cc sound.h
    )
    fips_generate(FROM sprites.py)
    oryol_shader(shaders.shd)
    fips_deps(Core Gfx Input Dbg Sound Assets)
    oryol_add_web_sample(Paclone "Simple Pacman clone (work in progress!" "emscripten,pnacl,android" Paclone.jpg "Paclone/Paclone.cc")
fips_end_app()
