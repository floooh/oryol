//------------------------------------------------------------------------------
//  test.shd
//------------------------------------------------------------------------------

/*
Bla Bla Bla
*/

@code_block TransformUtil
vec4 myTransform(mat4 mvp, vec4 pos)
{
    return mvp * pos;
}
@end

@code_block FSUtil
vec4 myColor(vec4 nrm, vec4 texColor)
{
    return ((nrm * 0.5) + 0.5) * 0.75 + texColor * texColor * texColor;
}
@end

@vs MyVertexShader
@use_code_block TransformUtil
uniform vsParams {
    mat4 mvp;
};
in vec4 position;
in vec4 normal;
in vec2 texcoord0;
out vec4 nrm;
out vec2 uv;
void main() {
    gl_Position = myTransform(mvp, position);
    nrm = normal;
    uv  = texcoord0;
}
@end

@fs MyFragmentShader
@use_code_block FSUtil
uniform sampler2D tex;
in vec4 nrm;
in vec2 uv;
out vec4 fragColor;
void main() {
    vec4 texColor = texture(tex, uv * vec2(5.0, 3.0));
    fragColor = myColor(nrm, texColor);
}
@end

@program MyShader MyVertexShader MyFragmentShader

