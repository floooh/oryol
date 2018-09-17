//------------------------------------------------------------------------------
//  ArrayTexture sample shaders.
//------------------------------------------------------------------------------

@vs vs
uniform vsParams {
    mat4 mvp;
    vec2 uvOffset0;
    vec2 uvOffset1;
    vec2 uvOffset2;
};

in vec4 in_pos;
in vec2 in_uv;
out vec3 uv0;
out vec3 uv1;
out vec3 uv2;

void main() {
    gl_Position = mvp * in_pos;
    uv0 = vec3(in_uv + uvOffset0, 0.0);
    uv1 = vec3(in_uv + uvOffset1, 1.0);
    uv2 = vec3(in_uv + uvOffset2, 2.0);
}
@end

//------------------------------------------------------------------------------
@fs fs
uniform sampler2DArray tex;
in vec3 uv0;
in vec3 uv1;
in vec3 uv2;
out vec4 fragColor;

void main() {
    vec4 c0 = texture(tex, uv0);
    vec4 c1 = texture(tex, uv1);
    vec4 c2 = texture(tex, uv2);
    fragColor = vec4(c0.xyz + c1.xyz + c2.xyz, 1.0);
}
@end

@program Shader vs fs
