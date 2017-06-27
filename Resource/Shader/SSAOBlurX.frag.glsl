#version 300 es
precision highp float;
precision highp sampler2D;
in vec2 texCoord;
out float bluredOcclusion;
uniform sampler2D occlusionBuffer;
void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(occlusionBuffer, 0));
    float totalAO = 0.0;
    //5 offsets for 10 pixel sampling!
    float offset[5] = float[](-2.0f, -1.0f, 0.0f, 1.0f, 2.0f);
    //int[5] weight = [1, 4, 6, 4, 1]; //sum = 16
    float weightInverse[5] = float[](0.0625f, 0.25f, 0.375, 0.25f, 0.0625f);
    for(int i = 0; i < 5; i++) {
        totalAO += texture(occlusionBuffer, vec2(texCoord.x + offset[i] * texelSize.x, texCoord.y)).r * weightInverse[i];
    }
    bluredOcclusion = totalAO;
}
