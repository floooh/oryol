//------------------------------------------------------------------------------
//  Vertex and fragment shaders for rendering into the offscreen render target
//
@vs offscreenVS
uniform vsParams {
    mat4 mvp;
};

in vec4 position;
in vec4 normal;
out vec4 nrm;

void main() {
    gl_Position = mvp * position;
    nrm = normal;
}
@end

@fs offscreenFS
in vec4 nrm;
out vec4 fragColor;
void main() {
    fragColor = vec4(nrm.xyz * 0.5 + 0.5, 1.0);
}
@end

@program OffscreenShader offscreenVS offscreenFS

//------------------------------------------------------------------------------
//  Vertex and fragment shaders for rendering into the frame buffer.
//
@vs displayVS
uniform vsParams {
    mat4 mvp;
};

in vec4 position;
in vec4 normal;
in vec2 texcoord0;
out vec2 uv;
out vec4 nrm;

void main() {
    gl_Position = mvp * position;
    uv = texcoord0;
    nrm = normalize(mvp * normal);
}
@end

@fs displayFS
uniform sampler2D tex;
in vec2 uv;
in vec4 nrm;
out vec4 fragColor;
void main() {
    vec4 c = texture(tex, uv * vec2(20.0, 10.0));
    float l = clamp(dot(nrm.xyz, normalize(vec3(1.0, 1.0, -1.0))), 0.0, 1.0) * 2.0;
    fragColor = vec4(c.xyz * (l + 0.25), 1.0);
}
@end

@program DisplayShader displayVS displayFS

