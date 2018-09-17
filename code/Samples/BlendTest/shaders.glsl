//------------------------------------------------------------------------------
//  background shader
//
@vs backgroundVS
in vec2 in_pos;
out vec2 uv0;

void main() {
    gl_Position = vec4(in_pos*2.0-1.0, 0.5f, 1.0f);
    uv0 = in_pos;
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
in vec4 in_pos;
in vec4 in_color;
out vec4 color;

void main() {
    gl_Position = in_pos + translate;
    color = in_color;
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
