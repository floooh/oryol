//------------------------------------------------------------------------------
//  DDSCubeMap sample shaders.
//------------------------------------------------------------------------------
@vs vs
uniform vsParams {
    mat4 mvp;
};
in vec4 in_pos;
in vec3 in_normal;
out vec3 nrm;

void main() {
    gl_Position = mvp * in_pos;
    nrm = in_normal;
}
@end

@fs fs
uniform samplerCube tex;
in vec3 nrm;

void main() {
    gl_FragColor = texture(tex, nrm);
}
@end

@program Shader vs fs
