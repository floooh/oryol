//------------------------------------------------------------------------------
//  Shaders for the InfiniteSpheres sample
//------------------------------------------------------------------------------

@vs vs
uniform vsParams {
    mat4 mvp;
};

in vec4 position;
in vec4 normal;
in vec2 texcoord0;
out vec4 nrm;
out vec2 uv;
void main() {
    gl_Position = mvp * position;
    nrm = normal;
    uv = texcoord0;
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
