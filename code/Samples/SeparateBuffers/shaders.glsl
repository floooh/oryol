@vs vs
uniform params {
    mat4 mvp;
};

in vec4 in_pos;
in vec3 in_color;
out vec3 color;

void main() {
    gl_Position = mvp * in_pos;
    color = in_color;
}
@end

@fs fs 
in vec3 color;
out vec4 fragColor;
void main() {
    fragColor = vec4(color, 1.0);
}
@end

@program Shader vs fs
