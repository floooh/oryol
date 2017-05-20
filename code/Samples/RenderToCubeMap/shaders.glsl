//------------------------------------------------------------------------------
//  Lighting helper functions.
//
@block lighting

vec3 light(vec3 baseColor, vec3 eyeVec, vec3 normal, vec3 lightVec) {

    // ambient intensity
    float ambient = 0.25;

    // diffuse
    float n_dot_l = max(dot(normal, lightVec), 0.0);
    float diff = n_dot_l + ambient;

    // specular
    float specPower = 16.0;
    vec3 r = reflect(-lightVec, normal);
    float r_dot_v = max(dot(r, eyeVec), 0.0);
    float spec = pow(r_dot_v, specPower) * n_dot_l;

    return baseColor * (diff+ambient) + vec3(spec,spec,spec);
}

vec3 gamma(vec3 c) {
    float p = 1.0/2.2;
    return pow(c, vec3(p,p,p));
}
@end


//------------------------------------------------------------------------------
//  Render simple colored shapes
//
@vs shapeVS
uniform vsParams {
    mat4 mvp;
    mat4 model;
    vec4 shapeColor;
    vec3 lightDir;
    vec3 eyePos;
};

in vec4 position;
in vec3 normal;
out vec3 worldPosition;
out vec3 worldNormal;
out vec3 worldEyePos;
out vec3 worldLightDir;
out vec4 color;

void main() {
    gl_Position = mvp * position;
    worldPosition = vec4(model * position).xyz;
    worldNormal = vec4(model * vec4(normal, 0.0)).xyz;
    worldEyePos = eyePos;
    worldLightDir = lightDir;
    color = shapeColor;
}
@end

@fs shapeFS
@include lighting
in vec3 worldNormal;
in vec3 worldPosition;
in vec3 worldEyePos;
in vec3 worldLightDir;
in vec4 color;
out vec4 fragColor;

void main() {
    vec3 nrm = normalize(worldNormal);
    vec3 eyeVec = normalize(worldEyePos - worldPosition);
    vec3 lightDir = normalize(worldLightDir);
    fragColor = vec4(light(color.xyz, eyeVec, nrm, lightDir), 1.0);
}
@end

@fs shapeFSGamma
@include lighting

in vec3 worldNormal;
in vec3 worldPosition;
in vec3 worldEyePos;
in vec3 worldLightDir;
in vec4 color;
out vec4 fragColor;

void main() {
    vec3 eyeVec = normalize(worldEyePos - worldPosition);
    fragColor = vec4(gamma(light(color.xyz, eyeVec, worldNormal, worldLightDir)), 1.0);
}
@end

@program ShapeShader shapeVS shapeFS
@program ShapeShaderWithGamma shapeVS shapeFSGamma

//------------------------------------------------------------------------------
//  Render shape with reflection/refraction.
//
@vs sphereVS
uniform vsParams {
    mat4 mvp;
    mat4 model;
    vec3 lightDir;
    vec3 eyePos;
};

in vec4 position;
in vec3 normal;
out vec3 worldPosition;
out vec3 worldNormal;
out vec3 worldEyePos;
out vec3 worldLightDir;

void main() {
    gl_Position = mvp * position;
    worldPosition = vec4(model * position).xyz;
    worldNormal = vec4(model * vec4(normal, 0.0)).xyz;
    worldEyePos = eyePos;
    worldLightDir = lightDir;
}
@end

@fs sphereFS
@include lighting

// Adapted from http://developer.download.nvidia.com/SDK/9.5/Samples/DEMOS/Direct3D9/src/HLSL_FresnelReflection/docs/FresnelReflection.pdf
float fresnel(vec3 V, vec3 N, float R0)
{
    float cosAngle = 1.0-max(dot(V, N), 0.0);
    float result = cosAngle * cosAngle;
    result = result * result;
    result = result * cosAngle;
    result = clamp(result * (1.0 - R0) + R0, 0.0, 1.0);
    return result;
}

uniform samplerCube tex;

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 worldEyePos;
in vec3 worldLightDir;
out vec4 fragColor;

void main() {
    vec3 eyeVec = normalize(worldEyePos.xyz - worldPosition.xyz);
    vec3 nrm = normalize(worldNormal.xyz);

    // reflection, refraction and fresnel
    vec3 refl = reflect(-eyeVec, nrm);
    vec3 refr = refract(-eyeVec, nrm, 0.7);
    vec4 reflectColor = texture(tex, refl);
    vec4 refractColor;
    if (length(refr) > 0.0) {
        refractColor = texture(tex, refr);
    }
    else {
        refractColor = reflectColor;
    }
    float fr = fresnel(eyeVec, nrm, 0.4);
    vec3 c = vec3(1.5, 1.2, 1.2) * fr * mix(refractColor.xyz, reflectColor.xyz, fr);
    c = light(c, eyeVec, nrm, normalize(worldLightDir));
    fragColor = vec4(gamma(c), 1.0);
}
@end

@program SphereShader sphereVS sphereFS


