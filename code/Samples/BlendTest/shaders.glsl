//------------------------------------------------------------------------------
//  background shader
//
@vs backgroundVS
in vec4 position;
in vec2 texcoord0;
out vec2 uv0;

void main() {
    gl_Position = position;
    uv0 = texcoord0;
}
@end

@fs backgroundFS
in vec2 uv0;
void main() {
    vec2 xy = fract(uv0 * 32.0) * 0.5 + 0.25;
    float f = xy.x * xy.y;
    gl_FragColor = vec4(f, f, f, 1.0);
}
@end

@program BGShader backgroundVS backgroundFS

//------------------------------------------------------------------------------
//  triangle shader
//
@vs triangleVS
uniform params {
    vec4 translate;
};
in vec4 position;
in vec4 color0;
out vec4 color;

void main() {
    gl_Position = position + translate;
    color = color0;
}
@end

@fs triangleFS
in vec4 color;
out vec4 fragColor;
void main() {
    fragColor = color;
}
@end

@program TriShader triangleVS triangleFS
