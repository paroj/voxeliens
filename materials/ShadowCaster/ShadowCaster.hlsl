/* This file implements standard programs for depth shadow mapping. 
   These particular ones are suitable for additive lighting models, and
   include 3 techniques to reduce depth fighting on self-shadowed surfaces,
   constant bias, gradient (slope-scale) bias, and a fuzzy shadow map comparison*/

// Shadow caster vertex program.
void ShadowCasterVP(
    float4 position            : POSITION,
    out float4 outPos        : POSITION,
    out float2 outDepth        : TEXCOORD0,

    uniform float4x4 worldViewProj,
    uniform float4 texelOffsets,
    uniform float4 depthRange
    )
{
	position.w = 1.0f;
	
    outPos = mul(worldViewProj, position);
	
	outPos.xy += texelOffsets.zw * outPos.w;

    outDepth.x = outPos.z;
    outDepth.y = outPos.w;
}

// Shadow caster fragment program for high-precision single-channel textures    
void ShadowCasterFP(
    float2 depth            : TEXCOORD0,
    out float4 result        : COLOR)
    
{
    float finalDepth = depth.x / depth.y;
	
    // just smear across all components 
    // therefore this one needs high individual channel precision
    result = float4(finalDepth, finalDepth, finalDepth, 1);
}
