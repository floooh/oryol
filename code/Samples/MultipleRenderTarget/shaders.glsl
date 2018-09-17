
//------------------------------------------------------------------------------
@vs offscreenVS
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

@fs offscreenFS
in vec4 nrm;
layout(location=0) out vec4 fragColor0;
layout(location=1) out vec4 fragColor1;
layout(location=2) out vec4 fragColor2;
void main() {
    vec3 c = abs(nrm.xyz) * vec3(0.5,0.5,0.5) + vec3(0.5,0.5,0.5);
    fragColor0 = vec4(c.x, 0.0, 0.0, 1.0);
    fragColor1 = vec4(0.0, c.y, 0.0, 1.0);
    fragColor2 = vec4(0.0, 0.0, c.z, 1.0);
}
@end

@program OffscreenShader offscreenVS offscreenFS

//------------------------------------------------------------------------------
@vs quadVS
in vec2 in_pos;
out vec2 uv;
void main() {
    gl_Position = vec4(in_pos*2.0-1.0, 0.5f, 1.0f);
    uv = in_pos;
}
@end

@fs quadFS
uniform sampler2D tex;
in vec2 uv;
out vec4 fragColor;
void main() {
    fragColor = texture(tex, uv);
}
@end

@program QuadShader quadVS quadFS

//------------------------------------------------------------------------------
@vs displayVS
uniform vsParams {
    mat4 mvp;
    vec2 offsets;
};

in vec4 in_pos;
in vec4 in_normal;
in vec2 in_uv;
out vec2 uvRed;
out vec2 uvGreen;
out vec2 uvBlue;

void main() {
    gl_Position = mvp * in_pos;
    uvRed = in_uv + vec2(offsets.x, 0.0);
    uvGreen = in_uv + vec2(0.0, offsets.y);
    uvBlue = in_uv;
}
@end

@fs displayFS
uniform sampler2D redTex;
uniform sampler2D greenTex;
uniform sampler2D blueTex;
in vec2 uvRed;
in vec2 uvGreen;
in vec2 uvBlue;
out vec4 fragColor;

void main() {
    vec4 red = texture(redTex, uvRed);
    vec4 green = texture(greenTex, uvGreen);
    vec4 blue = texture(blueTex, uvBlue);
    vec4 c = vec4(red.xyz + green.xyz + blue.xyz, 1.0);
    fragColor = c;
}
@end

@program DisplayShader displayVS displayFS

