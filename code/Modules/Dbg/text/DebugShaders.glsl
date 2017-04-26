//------------------------------------------------------------------------------
//  Debug module shaders
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  text vertex shader 
//
@vs vsDbgText
uniform vsParams {
    vec2 glyphSize;
};

in vec4 position;
in vec4 color0;
out vec2 uv;
out vec4 color;

void main() {
    vec2 screenPos = position.xy * glyphSize;
    screenPos.x -= 1.0;
    screenPos.y = 1.0 - screenPos.y;
    gl_Position = vec4(screenPos, 0.5, 1.0);
    uv = position.zw * vec2(8.0 / 1024.0, 1.0);  // one glyph is 8 pixels wide
    color = color0;
}
@end

//------------------------------------------------------------------------------
//  text fragment shader
//
@fs fsDbgText
uniform sampler2D tex;
in vec2 uv;
in vec4 color;
out vec4 fragColor;
void main() {
    fragColor = texture(tex, uv).xxxx * color;
}
@end

@program DbgTextShader vsDbgText fsDbgText
