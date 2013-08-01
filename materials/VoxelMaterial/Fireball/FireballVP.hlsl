float noiseFromPosition(float3 position)
{
	float3 noiseScaleFactors1 = float3(0.83, 1.15, 1.1956); //Higher numbers gives higher frequesncy
	float3 noiseScaleFactors2 = float3(0.53523, 0.473, 0.623); //Higher numbers gives higher frequesncy
	
	float3 resultVec = float3(0.0,0.0,0.0);
	resultVec += sin(position * noiseScaleFactors1); //Components now -1 to 1
	resultVec += sin(position * noiseScaleFactors2); //Components now -2 to 2
	
	resultVec += float3(2.0, 2.0, 2.0); //Components now 0 to 4
	resultVec *= float3(0.25, 0.25, 0.25); //Components now 0 to 1
	float result = resultVec.x + resultVec.y + resultVec.z;
	result /= 3.0;
	
	return result;
}

void FireballVP(
	float4 inLocalPosition	: POSITION,
	float4 inNormal	: NORMAL,

	out float4 outClipPosition		: POSITION,
	out float4 outLocalPosition	: TEXCOORD0,
	out float4 outWorldPosition	: TEXCOORD1,
	out float4 outVertexColour : COLOR,
	
	//uniform sampler3D noiseMap : TEXUNIT0,
	
	//uniform sampler1D permutationMap : TEXUNIT0,
	//uniform sampler1D gradientMap : TEXUNIT1,
	uniform sampler2D colorMap : register(s0),
	//uniform sampler3D noiseMap : TEXUNIT1,

	uniform float4x4 world,
	uniform float4x4 viewProj
	)
{	
	//Is this valid? Shouldn't the w component of a normal already be zero?
	inNormal.w = 0.0;
	
	//Some constants
	float noiseFrequency = 3.0;
	float scaleFactor = 2.0;	
	
	//Compute the properties before offseting
	float4 worldPosition = mul(world, inLocalPosition);
	float4 worldOrigin = mul(world, float4(0.0,0.0,0.0,1.0)); //Could probably just translate here.
	float distance = length(worldPosition.xyz - worldOrigin.xyz);	
	
	//Compute a noise value. We have to use the world space position here, because the local
	//space position will be the same for different fireballs and so we'll see repetition.
	float noiseSample = noiseFromPosition(worldPosition.xyz * noiseFrequency);

	//Compute the offset properties	
	float offset = noiseSample * distance * scaleFactor;	
	float4 offsetWorldPosition = worldPosition + (inNormal * offset);
	float offsetDistance = length(offsetWorldPosition.xyz - worldOrigin.xyz);
	
	//Sample the colour gradient texture.
	float4 texCoord = float4(offsetDistance / 5.0, 0.5f, 0.0f, 0.5f); //w is lod - not sure what it should be.
	texCoord.x = clamp(texCoord.x, 0.0, 0.99); //Setting sampler mode to clamp doesn't seem to work. Maybe because it's a vertex texture?
	outVertexColour = tex2Dlod(colorMap, texCoord);
	
	//Fade off the fireball as it grows.
	float fadeStart = 2.0;
	float fadeEnd = 2.5;
	float proportion = clamp(distance, fadeStart, fadeEnd);	
	proportion = (proportion - fadeStart) / (fadeEnd - fadeStart);
	outVertexColour.a = 1.0 - proportion;
	
	//Round the positions for the cubic look.
	offsetWorldPosition.xyz = round(offsetWorldPosition.xyz);	
	offsetWorldPosition += float4(0.5, 0.5, 0.5, 0.0);
	
	//Final positions.
	outWorldPosition = offsetWorldPosition;
	outClipPosition  = mul(viewProj, outWorldPosition);	
	
	//Within our fragment program we generate texture coordinates according to local space positions so that
	//moving meshes do not have the textures scroll across them. But the local space position on our fireball
	//are all on a sphere (not cubic) and even if we went back to local space at the end we would be performing
	//some scaling (the world matrix has a scale component as that's how we grow the fireball). But in the case
	//of the fireball we can acytually use the world position for texture coordinates, so we sneakily set the 
	//local position to be the same as this.
	outLocalPosition = offsetWorldPosition;
}
