@vs vs
uniform vsParams {
    mat4 mvp;
};
in vec4 in_pos;
in vec4 in_normal;
out vec4 nrm;

void main() {
    gl_Position = mvp * in_pos;
    nrm = in_normal;
}
@end

@fs fs
in vec4 nrm;
out vec4 fragColor;
void main() {
    fragColor = vec4(nrm.xyz * 0.5 + 0.5, 1.0);
}
@end

@program Shader vs fs
