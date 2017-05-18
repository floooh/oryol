@vs vs
uniform params {
    mat4 mvp;
};

in vec4 position;
in vec3 color0;
out vec3 color;

void main() {
    gl_Position = mvp * position;
    color = color0;
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
