//------------------------------------------------------------------------------
//  Annotated GLSL shaders for Fullscreen sample.
//  This implements distance-field ray-marching, see here for details:
//  http://www.iquilezles.org/default.html
//------------------------------------------------------------------------------

@block VSFuncs
// compute eye position
vec3 eyePos(float time, vec3 center) {
    return center + vec3(sin(time * 0.1) * 5.0, 2.0, cos(time * 0.1) * 5.0);
}

// lookat function
struct lookatResult {
    vec3 fwdVec; 
    vec3 rightVec;
    vec3 upVec;
};
lookatResult lookat(vec3 eye, vec3 center,vec3 up) {
    lookatResult res;
    res.fwdVec = normalize(center - eye);
    res.rightVec = normalize(cross(res.fwdVec, up));
    res.upVec = cross(res.rightVec, res.fwdVec);
    return res;
}
@end

@vs vs
@include VSFuncs
uniform params {
    float time;
};

in vec4 position;
in vec2 texcoord0;
out vec2 uv;
out vec3 eye;
out vec3 up;
out vec3 right;
out vec3 fwd;
out vec4 sinTime;

void main() {
    gl_Position = position;
    uv = texcoord0 * 2.0 - 1.0;
    vec3 center = vec3(0.0, 0.0, 0.0);
    eye = eyePos(time, center);
    lookatResult lookatVecs = lookat(eye, center, vec3(0.0, 1.0, 0.0));
    up = lookatVecs.upVec;
    fwd = lookatVecs.fwdVec;
    right = lookatVecs.rightVec;
    sinTime.x = sin(time);
    sinTime.y = sin(time * 0.5);
    sinTime.z = sin(time * 0.25);
    sinTime.w = sin(time * 0.125);
}
@end

@block FSFuncs
// smooth-minimum, see here: http://iquilezles.org/www/articles/smin/smin.htm
float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a)/k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

// sphere
float sdSphere(vec3 p, float r) {
    return length(p) - r;
}
// unsigned box
/*
float udBox(vec3 p, vec3 b) {
    return length(max(abs(p) - b, 0.0));
}
*/
// signed box
float sdBox(vec3 p, vec3 b) {
    vec3 d = abs(p) - b;
    return min( max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}

// union
/*
float opU(float d1, float d2) {
    return min(d1, d2);
}
*/

// smooth union
float opSU(float d1, float d2, float k) {
    return smin(d1, d2, k);
}

// subtraction
float opS(float d1, float d2) {
    return max(-d1, d2);
}

// intersection
/*
float opI(float d1, float d2) {
    return max(d1, d2);
}
*/

//------------------------------------------------------------------------------
//  distance function for the whole scene
//
float dScene(vec4 sinTime, vec3 p) {
    vec3 boxPos = p;
    vec3 spherePos0 = p + vec3(-sinTime.x, 0.0, 0.0) * 2.0;

    // infinite spheres
    float c = 0.25;
    vec3 q = mod(p, c) - c*0.5;
    return opS(sdSphere(q, abs(sinTime.y) * 0.16), opSU(sdSphere(spherePos0, 1.5), sdBox(boxPos, vec3(1.0,1.0,1.0)), 0.2));
}

//------------------------------------------------------------------------------
//  normal estimation function
//
vec3 normal(vec4 sinTime, vec3 p) {
    const float eps = 0.01;
    float x = dScene(sinTime, p + vec3(eps, 0, 0)) - dScene(sinTime, p - vec3(eps, 0, 0));
    float y = dScene(sinTime, p + vec3(0, eps, 0)) - dScene(sinTime, p - vec3(0, eps, 0));
    float z = dScene(sinTime, p + vec3(0, 0, eps)) - dScene(sinTime, p - vec3(0, 0, eps));
    return normalize(vec3(x, y, z));
}
@end

//------------------------------------------------------------------------------
//  The fragment shader where the magic happens.
//
@fs fs
@include FSFuncs

in vec2 uv;
in vec3 eye;
in vec3 up;
in vec3 right;
in vec3 fwd;
in vec4 sinTime;
out vec4 fragColor;

void main() {
    const float epsilon = 0.001;
    const float focalLength = 2.0;
    
    vec3 rayOrigin = eye + fwd * focalLength + right * uv.x + up * uv.y;
    vec3 rayDirection = normalize(rayOrigin - eye);
    
    vec4 color = vec4(0.5, 0.5, 0.5, 1.0);
    
    float t = 0.0;
    const int maxSteps = 64;
    for (int i = 0; i < maxSteps; i++) {
        vec3 p = rayOrigin + rayDirection * t;
        float d = dScene(sinTime, p);
        if (d < epsilon) {
            vec3 nrm = normal(sinTime, p) * 0.5 + 0.5;
            color = vec4(nrm, 1);
            break;
        }
        t += d;
    }

    fragColor = color;
}
@end

//------------------------------------------------------------------------------
@program Shader vs fs
