fips_begin_app(ImGuiDemo windowed)
    fips_files(
        ImGuiDemo.cc
    )
    fips_deps(Gfx HTTP Dbg IMUI)
    oryol_add_web_sample(ImGuiDemo "ImGui UI integration" "emscripten,pnacl" none "UI/ImGuiDemo/ImGuiDemo.cc")
fips_end_app()
