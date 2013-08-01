////////////////////////////////////////////////////////////////////////////////
// Basic Version
////////////////////////////////////////////////////////////////////////////////
void MeshVP(
	float4 inPosition	: POSITION,
	float4 inColour : COLOR,
	
	out float4 outClipPosition		: POSITION,
	out float4 outLocalPosition	: TEXCOORD0,
	out float4 outWorldPosition	: TEXCOORD1,
	out float4 outColour : COLOR,
	
	uniform float4x4 world,
	uniform float4x4 viewProj
	)
{		
	outLocalPosition = inPosition;
	
	//Compute the world space position
	outWorldPosition = mul(world, outLocalPosition);	

	//Compute the clip space position
	outClipPosition = mul(viewProj, outWorldPosition);
	
	outColour = inColour;
}