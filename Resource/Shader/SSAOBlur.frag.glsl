#version 300 es
precision highp float;
precision highp sampler2D;
in vec2 texCoord;
out float bluredOcclusion;
uniform sampler2D occlusionBuffer;
void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(occlusionBuffer, 0));
//    ivec2 center = ivec2(floor(gl_FragCoord.xy / 2.0));
//    vec2 bilinearWeightXY = vec2(1.0) - fract(gl_FragCoord.xy / 2.0);
//    
    float totalAO = 0.0;
//    float totalWeight = 0.0;
//    
//    vec2 centerOcclusion = texelFetch(occlusionBuffer, center, 0).xy;
//    float centerAO = centerOcclusion.x;
//    float centerDepth = centerOcclusion.y;
//    
//    for (int x = 0; x < 2; x++) {
//        for (int y = 0; y < 2; y++) {
//            ivec2 offset = ivec2(x, y);
//            vec2 occlusionSample = texelFetch(occlusionBuffer, center + offset, 0).xy;
//            
//            float aoSample = occlusionSample.x;
//            float depthSample = occlusionSample.y;
//
//            // Calculate bilinear weight
//            float bilinearWeight = (float(x)-bilinearWeightXY.x) * (float(y)-bilinearWeightXY.y);
//            // Calculate upsample weight based on how close the depth is to the main depth
//            float upSampleWeight = max(1e-5, 0.1 - abs(depthSample - centerDepth)) * 30.0;
//
//            // Apply weight and add to total sum
//            totalAO += (bilinearWeight + upSampleWeight) * aoSample;
//            totalWeight += (bilinearWeight + upSampleWeight);
//        }
//    }
//    bluredOcclusion = totalAO / totalWeight;
//    bluredOcclusion = centerAO;
    
    vec2 UPSAMPLE_OFFSETS[4] = vec2[](
        vec2(-1.0, -1.0) * texelSize,
        vec2(-1.0, 1.0) * texelSize,
        vec2(1.0, -1.0) * texelSize,
        vec2(1.0, 1.0) * texelSize
    );
}
