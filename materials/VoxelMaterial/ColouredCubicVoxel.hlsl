float4 computeShadowIntensity(float4x4 texViewProj, sampler2D shadowMap, float4 inWorldPosition)
{
	//Shadowing
	float4 shadowUV = mul(texViewProj, inWorldPosition);
	
	float bias = 0.000001;		
	float shadowIntensity = 0.0;
	float sample = 0.0f;	
	float texelSize = 1 / 256.0f; //Doesn't actually have to match shadow size - controls amount of bluring.
	
	sample = tex2D(shadowMap, shadowUV.xy).x;
	shadowIntensity += (sample + bias > shadowUV.z) ? 1.0f : 0.0f;
	
	sample = tex2D(shadowMap, shadowUV.xy + float2(texelSize, texelSize)).x;
	shadowIntensity += (sample + bias > shadowUV.z) ? 1.0f : 0.0f;
	
	sample = tex2D(shadowMap, shadowUV.xy + float2(-texelSize, texelSize)).x;
	shadowIntensity += (sample + bias > shadowUV.z) ? 1.0f : 0.0f;
	
	sample = tex2D(shadowMap, shadowUV.xy + float2(texelSize, -texelSize)).x;
	shadowIntensity += (sample + bias > shadowUV.z) ? 1.0f : 0.0f;
	
	sample = tex2D(shadowMap, shadowUV.xy + float2(-texelSize, -texelSize)).x;
	shadowIntensity += (sample + bias > shadowUV.z) ? 1.0f : 0.0f;

	shadowIntensity /= 5.0f;
	
	return shadowIntensity;
}

void buildCoordinateSystem(float3 worldPosition, out float2 texCoords, out float3 tangent, out float3 binormal, out float3 normal)
{
	//Compute normals from derivitive instrutions
	normal = cross(ddy(worldPosition), ddx(worldPosition));
	normal = normalize(normal);
	
	//Need to initialise to something, otherwise 45
	//degree angles (fireballs) never get texcoords.
	texCoords = float2(0.5, 0.5);
	
	//For some reason, in OpenGL the computed normals face the other way.
	#if OPENGL
		normal *= -1.0f;
	#endif
	
	//Small offset, because our cubes are centered on integer position rather than 
	float3 worldPositionOffset = worldPosition + float3(0.5, 0.5, 0.5);
	
	//Maybe we can improve on this with a x1x1 cubemap lookup texture, to convert
	//normal to face index? Then a look up table from face index to other details?
	if(normal.x > 0.9)
	{
		texCoords = float2(-worldPositionOffset.z, worldPositionOffset.y);
		tangent = float3(0.0, 0.0,-1.0);
		binormal = float3(0.0, 1.0,0.0);
	}
	else if(normal.y > 0.9)
	{
		texCoords = float2(worldPositionOffset.x, -worldPositionOffset.z);
		tangent = float3(1.0, 0.0, 0.0);
		binormal = float3(0.0, 0.0, -1.0);
	}
	else if(normal.z > 0.9)
	{
		texCoords = float2(worldPositionOffset.x, worldPositionOffset.y);
		tangent = float3(1.0, 0.0, 0.0);
		binormal = float3(0.0, 1.0, 0.0);
	}	
	else if(normal.x < -0.9)
	{
		texCoords = float2(worldPositionOffset.z, worldPositionOffset.y);
		tangent = float3(0.0, 0.0,1.0);
		binormal = float3(0.0, 1.0,0.0);
	}	
	else if(normal.y < -0.9)
	{
		texCoords = float2(worldPositionOffset.x, worldPositionOffset.z);
		tangent = float3(1.0, 0.0, 0.0);
		binormal = float3(0.0, 0.0, 1.0);
	}
	else if(normal.z < -0.9)
	{
		texCoords = float2(-worldPositionOffset.x, worldPositionOffset.y);
		tangent = float3(-1.0, 0.0, 0.0);
		binormal = float3(0.0, 1.0, 0.0);
	}
}

float4 computeSingleDynamicLight(float4 lightPosition, float4 lightColour, float4 inWorldPosition, float3 perturbedWorldNormal, float4 cameraPosition)
{
	//Material properties (not light)
	float Kd = 0.3;
	float shininess = 128;
	float Ks = 0.5;

	float3 lightDir = normalize(lightPosition.xyz -  (inWorldPosition.xyz * lightPosition.w));
	float3 V = normalize(cameraPosition.xyz - inWorldPosition.xyz);
	float3 H = normalize(lightDir + V);
	
	float4 litValue = lit(dot(lightDir, perturbedWorldNormal), dot(perturbedWorldNormal, H), shininess);	
	float4 diffuseLightColour = Kd * lightColour * litValue.y;
	float4 specularLightColour = Ks * lightColour * litValue.z;
	
	return diffuseLightColour + specularLightColour;
}

float4 computeDirectionalLight(float4 lightPosition, float4 lightColour, float4 inWorldPosition, float3 perturbedWorldNormal, float4 cameraPosition, float4 attenuation)
{
	float4 diffuseLightColour = computeSingleDynamicLight(lightPosition, lightColour, inWorldPosition, perturbedWorldNormal, cameraPosition);
	
	//We apply attenuation to the directional light, even though it is set to one. Otherwise Cg appears to 
	//optimise away this element of the attenuation array which then causes compile errors with the Ogre material.
	diffuseLightColour *= attenuation.z;
	return diffuseLightColour;
}

float4 computePointLight(float4 lightPosition, float4 lightColour, float4 inWorldPosition, float3 perturbedWorldNormal, float4 cameraPosition, float4 attenuation)
{
	float4 diffuseLightColour = computeSingleDynamicLight(lightPosition, lightColour, inWorldPosition, perturbedWorldNormal, cameraPosition);
	
	//Apply attentuation to point light.
	float pointLightDistance = length(inWorldPosition.xyz - lightPosition).r;
	diffuseLightColour *= (1.0f / (pointLightDistance * pointLightDistance * attenuation.z));
	
	return diffuseLightColour;
}

float4 computeDynamicLight(float4 inWorldPosition, float4 cameraPosition, float3 perturbedWorldNormal, float4 lightPositionArray[8], float4 lightDiffuseColourArray[8], float4 lightAttenuationArray[8], float4x4 texViewProj, sampler2D shadowMap)
{
	
	float4 dynamicLightColour = float4(0.0,0.0,0.0,0.0);
	
	float4 diffuseLightColour = computeDirectionalLight(lightPositionArray[0], lightDiffuseColourArray[0], inWorldPosition, perturbedWorldNormal, cameraPosition, lightAttenuationArray[0]);
	float4 shadowIntensity = computeShadowIntensity(texViewProj, shadowMap, inWorldPosition);

	dynamicLightColour += diffuseLightColour * shadowIntensity;
	
	inWorldPosition.xyz = floor(inWorldPosition.xyz/* + float3(0.5, 0.5, 0.5)*/);
	
	for(int lightIndex = 1; lightIndex < 8; lightIndex++)
	{		
		float4 diffuseLightColour = computePointLight(lightPositionArray[lightIndex], lightDiffuseColourArray[lightIndex], inWorldPosition, perturbedWorldNormal, cameraPosition, lightAttenuationArray[lightIndex]);		
		dynamicLightColour += diffuseLightColour;
	}
	
	return dynamicLightColour;
}

void ColouredCubicVoxelFPAdvanced(
	float4 inLocalPosition	: TEXCOORD0,
	float4 inWorldPosition	: TEXCOORD1,
	float4 inClipPosition : VPOS, //Requires PS 3.0?
	float4 colour : COLOR,
	
	uniform float4 lightPositionArray[8],
	uniform float4 lightDiffuseColourArray[8],
	uniform float4 lightAttenuationArray[8],
	uniform float4 lightEmissiveColour,
	uniform float4 cameraPosition,
	
	uniform float4x4 texViewProj,

	uniform sampler2D normalMap : register(s1),
	uniform sampler3D noiseMap : register(s2),
    uniform sampler2D shadowMap : register(s3),
	
	out float4 result		: COLOR)
{	
	float2 texCoords;	
	float3 tangent;
	float3 binormal;
	float3 worldNormal;

	buildCoordinateSystem(inLocalPosition.xyz, texCoords, tangent, binormal, worldNormal);

	float3x3 rotation = float3x3(tangent, binormal, worldNormal);
	
	rotation = transpose(rotation); //Inversion?
	
	float4 perturbedNormalAndHeight = tex2D(normalMap, texCoords);
	perturbedNormalAndHeight.xyz -= 0.5f; //Move range from 0.0 - 1.0 to -0.5 - 0.5
	perturbedNormalAndHeight.xyz = normalize(perturbedNormalAndHeight.xyz); //Moves to range -1.0 - 1.0. 
	
	perturbedNormalAndHeight.xyz = mul(rotation, perturbedNormalAndHeight.xyz);
	
	
	//Apply some noise. Tiny offset required as geometry lies exactly on texel boundaries. 32.0f is size of texture.
	float noiseVal = tex3D(noiseMap, ((inLocalPosition.xyz + float3(0.5f,0.5f,0.5f)) - (worldNormal.xyz * 0.1f)) / 64.0f).a;
	noiseVal -= 0.5f; //So some are positive and some negative.
	float noiseScaleFactor = 0.3;
	noiseVal *= noiseScaleFactor;
	colour += float4(noiseVal, noiseVal, noiseVal, 0.0f);
	
	float4 dynamicLightColour = computeDynamicLight(inWorldPosition, cameraPosition, perturbedNormalAndHeight.xyz, lightPositionArray, lightDiffuseColourArray, lightAttenuationArray, texViewProj, shadowMap);
	
	
	//float3 finalAmbientColour = computeAmbientLightColour(inWorldPosition, ambientMap);
	float3 finalAmbientColour = float3(0.25f, 0.25f, 0.25f);
	finalAmbientColour *= perturbedNormalAndHeight.a;
	
	lightEmissiveColour.xyz *= perturbedNormalAndHeight.a;
	
	float3 finalLight = (dynamicLightColour.xyz) + finalAmbientColour + lightEmissiveColour.xyz;
	
	finalLight = saturate(finalLight);
	
	//Apply lighting to the base colour
	colour.xyz *= finalLight;
	
	//height = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	result = colour;
}
