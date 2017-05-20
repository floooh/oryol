@vs vs
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

@fs fs
in vec4 nrm;
out vec4 fragColor;
void main() {
    fragColor = nrm * 0.5 + 0.5;
}
@end

@program Shader vs fs
