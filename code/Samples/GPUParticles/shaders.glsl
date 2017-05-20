//------------------------------------------------------------------------------
//  Fragment shader util functions
//
@block FSUtil
// get particleId from current fragment coord
float getParticleId(float bufferWidth, vec2 fragCoord) {
    vec2 xy = floor(fragCoord);
    return (xy.y * bufferWidth * 0.5) + (xy.x * 0.5);
}

// check if current fragment is a position or velocity
bool atPosition(vec2 fragCoord) {
    // this should be either 0.0 or 1.0, but avoid an exact comparison
    return mod(floor(fragCoord.x), 2.0) < 0.5;
}

// obtain uv coords for particle position and velocity
struct particleStateUvs {
    vec2 posUv;
    vec2 velUv;
};
particleStateUvs getParticleStateUvs(bool atPos, vec2 bufDims, vec2 fragCoord) {
    particleStateUvs result;
    if (atPos) {
        result.posUv = fragCoord / bufDims;
        result.velUv = (fragCoord + vec2(1.0, 0.0)) / bufDims;
    }
    else {
        result.posUv = (fragCoord + vec2(-1.0, 0.0)) / bufDims;
        result.velUv = fragCoord / bufDims;
    }
    return result;
}
@end

//------------------------------------------------------------------------------
//  Vertex shader util functions
//
@block VSUtil
// compute particle position uv coord from particleId
vec2 posUvFromParticleId(float particleId, vec2 bufferDims) {
    float numParticlesAlongX = bufferDims.x * 0.5;
    float f = particleId / numParticlesAlongX;
    float fragCoordX = fract(f) * numParticlesAlongX * 2.0;
    float fragCoordY = floor(f);
    vec2 posUV = vec2(fragCoordX, fragCoordY) / bufferDims;
    return posUV;
}
@end

//------------------------------------------------------------------------------
//
// The simplex noise function is used for getting pseudo-random particle
// velocity vectors.
//
// Description : Array and textureless GLSL 2D simplex noise function.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
//
@block Noise
vec3 mod289(vec3 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
    return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
{
    const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
    0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
    -0.577350269189626,  // -1.0 + 2.0 * C.x
    0.024390243902439); // 1.0 / 41.0
    // First corner
    vec2 i  = floor(v + dot(v, C.yy) );
    vec2 x0 = v -   i + dot(i, C.xx);
    
    // Other corners
    vec2 i1;
    //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
    //i1.y = 1.0 - i1.x;
    i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    // x0 = x0 - 0.0 + 0.0 * C.xx ;
    // x1 = x0 - i1 + 1.0 * C.xx ;
    // x2 = x0 - 1.0 + 2.0 * C.xx ;
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    
    // Permutations
    i = mod289(i); // Avoid truncation effects in permutation
    vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
    + i.x + vec3(0.0, i1.x, 1.0 ));
    
    vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
    m = m*m ;
    m = m*m ;
    
    // Gradients: 41 points uniformly over a line, mapped onto a diamond.
    // The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)
    
    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;
    
    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt( a0*a0 + h*h );
    m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
    
    // Compute final noise value at P
    vec3 g;
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130.0 * dot(m, g);
}
@end

//------------------------------------------------------------------------------
//  A generic fullscreen-quad vertex shader.
//
@vs fsqVS
in vec4 position;
void main() {
    gl_Position = position;
}
@end

//------------------------------------------------------------------------------
//  Fullscreen-quad fragment shader to initialize a particle state texture
//  with particle positions and velocity.
//
//  ParticleBufferLayout:
//
//  | p0.pos | p0.vec | p1.pos | p1.vec | p2.pos | p2.vec |
//  | p3.pos | p3.vec | ...    | ...
@fs initFS
@include Noise
@include FSUtil
uniform fsParams {
    vec2 bufDims;
};
out vec4 fragColor;
void main() {
    if (atPosition(gl_FragCoord.xy)) {
        // position starts always at 0
        fragColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
    else {
        // select a pseudo random velocity vector, skewed along positive Y
        float particleId = getParticleId(bufDims.x, gl_FragCoord.xy);
        vec3 rnd = vec3(snoise(vec2(particleId, 0.0)),
                        snoise(vec2(0.0, particleId * 0.1)),
                        snoise(vec2(particleId * 0.1, particleId * 0.01)));
        rnd *= 0.5;
        rnd.y += 2.0; // skew velocity upwards
        fragColor = vec4(rnd, 0.0);
    }
}
@end

@program InitShader fsqVS initFS

//------------------------------------------------------------------------------
//  Fullscreen-quad shader to update particle positions and velocity
//  via ping-ponging.
//
@fs updateFS
@include FSUtil
uniform fsParams {
    vec2 bufDims;
    float numParticles;
};
uniform sampler2D prevState;
out vec4 fragColor;

void main() {
    const float frameTime = 1.0 / 60.0;

    // get particleId and check if this is actually an active particle
    float particleId = getParticleId(bufDims.x, gl_FragCoord.xy);
    if (particleId >= numParticles) {
        discard;
    }

    // current fragment a position or velocity?
    bool atPos = atPosition(gl_FragCoord.xy);

    // load previous position and velocity
    particleStateUvs uvs = getParticleStateUvs(atPos, bufDims.xy, gl_FragCoord.xy);
    vec4 pos = texture(prevState, uvs.posUv);
    vec4 vel = texture(prevState, uvs.velUv);
    if (atPos) {
        // update position
        pos += vel * frameTime;
        fragColor = pos;
    }
    else {
        // update velocity
        vec4 vel = texture(prevState, uvs.velUv);
        vel.y -= 1.0 * frameTime;
        
        // check for collision
        if ((pos.y < -1.0) && (vel.y < 0.0)) {
            vel.y = -vel.y;
            vel *= 0.8;
        }
        fragColor = vel;
    }
}
@end

@program UpdateShader fsqVS updateFS

//------------------------------------------------------------------------------
//  This is the actual particle drawing shader. The vertex shader gets the
//  current particleId from the instance0 vertex attribute (this is generated
//  using instanced rendering, and unfortunately the ANGLE_instanced_array
//  extension cannot provide an auto-generated gl_InstanceID. With this
//  particleID, the particle's position is fetched from the particle state
//  texture.
//
@vs drawVS
@include VSUtil

uniform vsParams {
    mat4 mvp;
    vec2 bufDims;
};
uniform sampler2D particleTex;

in vec4 position;
in vec4 color0;
in float instance0;
out vec4 color;
void main() {
    float particleId = instance0;
    vec2 posUv = posUvFromParticleId(particleId, bufDims);
    vec4 particlePos = vec4(texture(particleTex, posUv).xyz, 0.0);

    gl_Position = mvp * (position + particlePos);
    color = color0;
}
@end


@fs drawFS
in vec4 color;
out vec4 fragColor;
void main() {
    fragColor = color;
}
@end

@program DrawShader drawVS drawFS
