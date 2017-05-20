//------------------------------------------------------------------------------
//  VolumeTexture sample shaders.
//------------------------------------------------------------------------------
@vs vs
uniform vsParams {
    mat4 mvp;
    vec4 modelEyePos;
};
in vec4 position;
out vec3 eyePos;
out vec3 surfPos;
void main() {
    gl_Position = mvp * position;
    eyePos = modelEyePos.xyz + vec3(0.5, 0.5, 0.5);
    surfPos = position.xyz + vec3(0.5, 0.5, 0.5);
}
@end

//------------------------------------------------------------------------------
@fs fs
uniform sampler3D tex;
in vec3 eyePos;
in vec3 surfPos;
out vec4 fragColor;
void main() {
    vec3 ep = eyePos;
    vec3 sp = surfPos;
    vec3 dir = normalize(sp - ep);
    float t = 0.0;
    vec4 c = vec4(0.0,0.0,0.0,0.0);
    for (int i = 0; i < 64; i++) {
        vec3 p = (sp + dir * t);
        c += texture(tex, p) * 0.02;
        t += 1.0 / 64.0;
    }
    fragColor = vec4(c.xyz, 1.0);
}
@end

@program Shader vs fs
