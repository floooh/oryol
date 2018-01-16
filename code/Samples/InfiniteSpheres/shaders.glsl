//------------------------------------------------------------------------------
//  Shaders for the InfiniteSpheres sample
//------------------------------------------------------------------------------

@vs vs
uniform vsParams {
    mat4 mvp;
};

in vec4 in_pos;
in vec4 in_normal;
in vec2 in_uv;
out vec4 nrm;
out vec2 uv;
void main() {
    gl_Position = mvp * in_pos;
    nrm = in_normal;
    uv = in_uv;
}
@end

@fs fs
uniform sampler2D tex;
in vec4 nrm;
in vec2 uv;
out vec4 fragColor;
void main() {
    vec4 texColor = texture(tex, uv * vec2(5.0, 3.0));
    fragColor = ((nrm * 0.5) + 0.5) * 0.75 + texColor * texColor * texColor * texColor;
}
@end

@program Shader vs fs
