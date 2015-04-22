#-------------------------------------------------------------------------------
#   oryol ImGui integration module
#-------------------------------------------------------------------------------
fips_begin_module(IMUI)
    fips_files(
        IMUI.h IMUI.cc
        IMUISetup.h
        imguiWrapper.h imguiWrapper.cc
    )
    fips_generate(FROM IMUIShaders.shd TYPE Shader)
    fips_deps(Gfx Input imgui)
fips_end_module()
