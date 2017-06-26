#version 300 es
precision highp float;
precision highp sampler2D;
in vec2 texCoord;
out float bluredOcclusion;
uniform sampler2D occlusionBuffer;
uniform vec2 direction;

float blur5(sampler2D image, vec2 uv, vec2 resolution) {
    float color = 0.0;
    vec2 off1 = vec2(1.3333333333333333) * direction;
    color += texture(image, uv).r * 0.29411764705882354;
    color += texture(image, uv + (off1 / resolution)).r * 0.35294117647058826;
    color += texture(image, uv - (off1 / resolution)).r * 0.35294117647058826;
    return color;
}
float blur9(sampler2D image, vec2 uv, vec2 resolution) {
    float color = 0.0;
    vec2 off1 = vec2(1.3846153846) * direction;
    vec2 off2 = vec2(3.2307692308) * direction;
    color += texture(image, uv).r * 0.2270270270;
    color += texture(image, uv + (off1 / resolution)).r * 0.3162162162;
    color += texture(image, uv - (off1 / resolution)).r * 0.3162162162;
    color += texture(image, uv + (off2 / resolution)).r * 0.0702702703;
    color += texture(image, uv - (off2 / resolution)).r * 0.0702702703;
    return color;
}
float blur13(sampler2D image, vec2 uv, vec2 resolution) {
    float color = 0.0;
    vec2 off1 = vec2(1.411764705882353) * direction;
    vec2 off2 = vec2(3.2941176470588234) * direction;
    vec2 off3 = vec2(5.176470588235294) * direction;
    color += texture(image, uv).r * 0.1964825501511404;
    color += texture(image, uv + (off1 / resolution)).r * 0.2969069646728344;
    color += texture(image, uv - (off1 / resolution)).r * 0.2969069646728344;
    color += texture(image, uv + (off2 / resolution)).r * 0.09447039785044732;
    color += texture(image, uv - (off2 / resolution)).r * 0.09447039785044732;
    color += texture(image, uv + (off3 / resolution)).r * 0.010381362401148057;
    color += texture(image, uv - (off3 / resolution)).r * 0.010381362401148057;
    return color;
}
vec4 cubic(float v){
    vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
    vec4 s = n * n * n;
    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;
    return vec4(x, y, z, w) * (1.0/6.0);
}

vec4 textureBicubic(sampler2D sampler, vec2 texCoords){
    vec2 texSize = vec2(textureSize(sampler, 0));
    vec2 invTexSize = 1.0 / texSize;
   
    texCoords = texCoords * texSize - 0.5;

   
    vec2 fxy = fract(texCoords);
    texCoords -= fxy;

    vec4 xcubic = cubic(fxy.x);
    vec4 ycubic = cubic(fxy.y);

    vec4 c = texCoords.xxyy + vec2(-0.5, +1.5).xyxy;
    
    vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
    vec4 offset = c + vec4(xcubic.yw, ycubic.yw) / s;
    
    offset *= invTexSize.xxyy;
    
    vec4 sample0 = texture(sampler, offset.xz);
    vec4 sample1 = texture(sampler, offset.yz);
    vec4 sample2 = texture(sampler, offset.xw);
    vec4 sample3 = texture(sampler, offset.yw);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return mix(
       mix(sample3, sample2, sx), mix(sample1, sample0, sx)
    , sy);
}
void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(occlusionBuffer, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x) * 0.5, float(y) * 0.5) * texelSize;
            result += texture(occlusionBuffer, texCoord + offset).r;
        }
    }
    bluredOcclusion = result / (4.0 * 4.0);
//    bluredOcclusion = textureBicubic(occlusionBuffer, texCoord).r;
//    bluredOcclusion = blur13(occlusionBuffer, texCoord, vec2(textureSize(occlusionBuffer, 0)));
}
