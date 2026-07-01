#include <metal_stdlib>
using namespace metal;

struct VertexIn {
    float4 position [[attribute(0)]];
    float4 color    [[attribute(1)]];
};

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

struct CameraData { 
    float4x4 matrix;
};

vertex VertexOut design_physics_vertex(
    VertexIn in [[stage_in]], 
    constant CameraData& cam [[buffer(1)]]
) {
    VertexOut out;
    
    float4 worldPos = float4(in.position.xyz, 1.0);
    out.position = cam.matrix * worldPos; 
    out.color = in.color;
    
    return out;
}

fragment float4 design_physics_fragment(VertexOut in [[stage_in]]) {
    return in.color;
}