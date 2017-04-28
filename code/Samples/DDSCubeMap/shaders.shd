//------------------------------------------------------------------------------
//  DDSCubeMap sample shaders.
//------------------------------------------------------------------------------
@vs vs
uniform vsParams {
    mat4 mvp;
};
in vec4 position;
in vec3 normal;
out vec3 nrm;

void main() {
    gl_Position = mvp * position;
    nrm = normal;
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
