//------------------------------------------------------------------------------
//  DDSTextureLoading sample shaders.
//------------------------------------------------------------------------------

@vs vs
uniform vsParams {
    mat4 mvp;
};
in vec4 in_pos;
in vec2 in_uv;
out vec2 uv;

void main() {
    gl_Position = mvp * in_pos;
    uv = in_uv;
}
@end

@fs fs
uniform sampler2D tex;
in vec2 uv;
out vec4 fragColor;
void main() {
    fragColor = vec4(texture(tex, uv).xyz, 1.0);
}
@end

@program Shader vs fs
