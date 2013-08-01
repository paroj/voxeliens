float4 floatToRGBA(float input)
{	
	//Store the input in each component
	float4 inputVec = float4(input, input, input, input);
	
	//Convert each component to a value in the range 0-15
	float4 result = floor(inputVec / float4(4096.0f, 256.0f, 16.0f, 1.0f));	
	float4 shiftedResult = float4(0.0f, result.rgb) * 16.0f;	
	result -= shiftedResult;
	
	//Convert to range 0-1
	result /= 15.0f;
	
	//return the result	
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Advanced Version
////////////////////////////////////////////////////////////////////////////////
void PolyVoxVPAdvanced(
	float4 inPosition	: POSITION,

	out float4 outClipPosition		: POSITION,
	out float4 outLocalPosition	: TEXCOORD0,
	out float4 outWorldPosition	: TEXCOORD1,
	out float4 outVertexColour : COLOR,

	uniform float4x4 world,
	uniform float4x4 viewProj
	)
{	
	//Thermite3D has packed the material ID into the vertex's 'w' component.
	//Obviously this will mess up any transformations we perform so before going
	//to world and clip space we back up this material and reset the 'w' to 1.0.
	float material = inPosition.w;
	inPosition.w = 1.0;
	
	outLocalPosition = inPosition;
	
	//Compute the world space position
	outWorldPosition = mul(world, outLocalPosition);	

	//Compute the clip space position
	outClipPosition = mul(viewProj, outWorldPosition);
	
	//Get the voxel colour from the colour map
	//float u = (material / 256.0f) + (1.0f / 512.0f);
	//outVertexColour = tex1D(colorMap, u);
	
	outVertexColour = floatToRGBA(material);
}
