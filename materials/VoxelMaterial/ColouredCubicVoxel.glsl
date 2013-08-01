#version 120

uniform vec4 lightPositionArray[8];
uniform vec4 lightDiffuseColourArray[8];
uniform vec4 lightAttenuationArray[8];
uniform vec4 lightEmissiveColour;
uniform vec4 cameraPosition;
	
uniform mat4 texViewProj;

uniform sampler2D colorMap;
uniform sampler2D normalMap /*: register(s1)*/;
uniform sampler3D noiseMap /*: register(s2)*/;
uniform sampler2D shadowMap /*: register(s3)*/;

vec4 computeShadowIntensity(mat4 texViewProj, sampler2D shadowMap, vec4 inWorldPosition)
{
	//Shadowing
	vec4 shadowUV = texViewProj * inWorldPosition;
	
	float bias = 0.000001;		
	float shadowIntensity = 0.0;
	float sample = 0.0;	
	float texelSize = 1.0 / 256.0; //Doesn't actually have to match shadow size - controls amount of bluring.
	
	sample = texture2D(shadowMap, shadowUV.xy).x;
	shadowIntensity += (sample + bias > shadowUV.z) ? 1.0 : 0.0;
	
	sample = texture2D(shadowMap, shadowUV.xy + vec2(texelSize, texelSize)).x;
	shadowIntensity += (sample + bias > shadowUV.z) ? 1.0 : 0.0;
	
	sample = texture2D(shadowMap, shadowUV.xy + vec2(-texelSize, texelSize)).x;
	shadowIntensity += (sample + bias > shadowUV.z) ? 1.0 : 0.0;
	
	sample = texture2D(shadowMap, shadowUV.xy + vec2(texelSize, -texelSize)).x;
	shadowIntensity += (sample + bias > shadowUV.z) ? 1.0 : 0.0;
	
	sample = texture2D(shadowMap, shadowUV.xy + vec2(-texelSize, -texelSize)).x;
	shadowIntensity += (sample + bias > shadowUV.z) ? 1.0 : 0.0;

	shadowIntensity /= 5.0;
	
	return vec4(shadowIntensity, shadowIntensity, shadowIntensity, shadowIntensity);
}

void buildCoordinateSystem(vec3 worldPosition, out vec2 texCoords, out vec3 tangent, out vec3 binormal, out vec3 normal)
{
	//Compute normals from derivitive instrutions
	normal = cross(dFdy(worldPosition), dFdx(worldPosition));
	normal = normalize(normal);
	
	//Need to initialise to something, otherwise 45
	//degree angles (fireballs) never get texcoords.
	texCoords = vec2(0.5, 0.5);
	
	//Avoid GLSL warnings
	tangent = vec3(0.0, 0.0, 0.0);
	binormal = vec3(0.0, 0.0, 0.0);
	
	//For some reason, in OpenGL the computed normals face the other way.
	//#if OPENGL
		//normal *= -1.0;
	//#endif
	
	//Small offset, because our cubes are centered on integer position rather than 
	vec3 worldPositionOffset = worldPosition + vec3(0.5, 0.5, 0.5);
	
	//Maybe we can improve on this with a x1x1 cubemap lookup texture, to convert
	//normal to face index? Then a look up table from face index to other details?
	if(normal.x > 0.9)
	{
		texCoords = vec2(-worldPositionOffset.z, worldPositionOffset.y);
		tangent = vec3(0.0, 0.0,-1.0);
		binormal = vec3(0.0, 1.0,0.0);
	}
	else if(normal.y > 0.9)
	{
		texCoords = vec2(worldPositionOffset.x, -worldPositionOffset.z);
		tangent = vec3(1.0, 0.0, 0.0);
		binormal = vec3(0.0, 0.0, -1.0);
	}
	else if(normal.z > 0.9)
	{
		texCoords = vec2(worldPositionOffset.x, worldPositionOffset.y);
		tangent = vec3(1.0, 0.0, 0.0);
		binormal = vec3(0.0, 1.0, 0.0);
	}	
	else if(normal.x < -0.9)
	{
		texCoords = vec2(worldPositionOffset.z, worldPositionOffset.y);
		tangent = vec3(0.0, 0.0,1.0);
		binormal = vec3(0.0, 1.0,0.0);
	}	
	else if(normal.y < -0.9)
	{
		texCoords = vec2(worldPositionOffset.x, worldPositionOffset.z);
		tangent = vec3(1.0, 0.0, 0.0);
		binormal = vec3(0.0, 0.0, 1.0);
	}
	else if(normal.z < -0.9)
	{
		texCoords = vec2(-worldPositionOffset.x, worldPositionOffset.y);
		tangent = vec3(-1.0, 0.0, 0.0);
		binormal = vec3(0.0, 1.0, 0.0);
	}
}

vec4 glsl_lit(float NdotL, float NdotH, float m)
{
  float specular = (NdotL > 0) ? pow(max(0.0, NdotH), m) : 0.0;
  return vec4(1.0, max(0.0, NdotL), specular, 1.0);
}

vec4 computeSingleDynamicLight(vec4 lightPosition, vec4 lightColour, vec4 inWorldPosition, vec3 perturbedWorldNormal, vec4 cameraPosition)
{
	//Material properties (not light)
	float Kd = 0.3;
	float shininess = 128.0;
	float Ks = 0.5;

	vec3 lightDir = normalize(lightPosition.xyz -  (inWorldPosition.xyz * lightPosition.w));
	vec3 V = normalize(cameraPosition.xyz - inWorldPosition.xyz);
	vec3 H = normalize(lightDir + V);
	
	vec4 litValue = glsl_lit(dot(lightDir, perturbedWorldNormal), dot(perturbedWorldNormal, H), shininess);	
	vec4 diffuseLightColour = Kd * lightColour * litValue.y;
	vec4 specularLightColour = Ks * lightColour * litValue.z;
	
	return diffuseLightColour + specularLightColour;
}

vec4 computeDirectionalLight(vec4 lightPosition, vec4 lightColour, vec4 inWorldPosition, vec3 perturbedWorldNormal, vec4 cameraPosition, vec4 attenuation)
{
	vec4 diffuseLightColour = computeSingleDynamicLight(lightPosition, lightColour, inWorldPosition, perturbedWorldNormal, cameraPosition);
	
	//We apply attenuation to the directional light, even though it is set to one. Otherwise Cg appears to 
	//optimise away this element of the attenuation array which then causes compile errors with the Ogre material.
	diffuseLightColour *= attenuation.z;
	return diffuseLightColour;
}

vec4 computePointLight(vec4 lightPosition, vec4 lightColour, vec4 inWorldPosition, vec3 perturbedWorldNormal, vec4 cameraPosition, vec4 attenuation)
{
	vec4 diffuseLightColour = computeSingleDynamicLight(lightPosition, lightColour, inWorldPosition, perturbedWorldNormal, cameraPosition);
	
	//Apply attentuation to point light.
	float pointLightDistance = length(inWorldPosition.xyz - lightPosition.xyz);
	//diffuseLightColour *= (1.0 / (pointLightDistance * pointLightDistance * attenuation.z)); // FIXME - For some reason attenuation.z doesn't work?
	diffuseLightColour *= (1.0 / (pointLightDistance * pointLightDistance * 0.001 /* attenuation.z*/));
	
	return diffuseLightColour;
}

vec4 computeDynamicLight(vec4 inWorldPosition, vec4 cameraPosition, vec3 perturbedWorldNormal, vec4 lightPositionArray[8], vec4 lightDiffuseColourArray[8], vec4 lightAttenuationArray[8], mat4 texViewProj, sampler2D shadowMap)
{
	
	vec4 dynamicLightColour = vec4(0.0,0.0,0.0,0.0);
	
	vec4 diffuseLightColour = computeDirectionalLight(lightPositionArray[0], lightDiffuseColourArray[0], inWorldPosition, perturbedWorldNormal, cameraPosition, lightAttenuationArray[0]);
	vec4 shadowIntensity = computeShadowIntensity(texViewProj, shadowMap, inWorldPosition);

	dynamicLightColour += diffuseLightColour * shadowIntensity;
	
	inWorldPosition.xyz = floor(inWorldPosition.xyz/* + vec3(0.5, 0.5, 0.5)*/);
	
	for(int lightIndex = 1; lightIndex < 8; lightIndex++)
	{		
		vec4 diffuseLightColour = computePointLight(lightPositionArray[lightIndex], lightDiffuseColourArray[lightIndex], inWorldPosition, perturbedWorldNormal, cameraPosition, lightAttenuationArray[lightIndex]);		
		dynamicLightColour += diffuseLightColour;
	}
	
	return dynamicLightColour;
}

void main(void)
{
	vec4 inLocalPosition = gl_TexCoord[0];
	vec4 inWorldPosition = gl_TexCoord[1];
	vec4 colour = gl_Color;
	
	vec2 texCoords;	
	vec3 tangent;
	vec3 binormal;
	vec3 worldNormal;

	buildCoordinateSystem(inLocalPosition.xyz, texCoords, tangent, binormal, worldNormal);
	
	mat3 rotation = mat3(tangent, binormal, worldNormal);
	
	rotation = transpose(rotation); //Inversion?
	
	vec4 perturbedNormalAndHeight = texture2D(normalMap, texCoords);
	perturbedNormalAndHeight.xyz -= 0.5; //Move range from 0.0 - 1.0 to -0.5 - 0.5
	perturbedNormalAndHeight.xyz = normalize(perturbedNormalAndHeight.xyz); //Moves to range -1.0 - 1.0. 
	
	//perturbedNormalAndHeight.xyz = mul(rotation, perturbedNormalAndHeight.xyz);
	perturbedNormalAndHeight.xyz = rotation * perturbedNormalAndHeight.xyz;
	
	//Apply some noise. Tiny offset required as geometry lies exactly on texel boundaries. 32.0f is size of texture.
	float noiseVal = texture3D(noiseMap, ((inLocalPosition.xyz + vec3(0.5,0.5,0.5)) - (worldNormal.xyz * 0.1)) / 64.0).a;
	noiseVal -= 0.5; //So some are positive and some negative.
	float noiseScaleFactor = 0.3;
	noiseVal *= noiseScaleFactor;
	colour += vec4(noiseVal, noiseVal, noiseVal, 0.0);
	
	vec4 dynamicLightColour = computeDynamicLight(inWorldPosition, cameraPosition, perturbedNormalAndHeight.xyz, lightPositionArray, lightDiffuseColourArray, lightAttenuationArray, texViewProj, shadowMap);
	
	//vec3 finalAmbientColour = computeAmbientLightColour(inWorldPosition, ambientMap);
	vec3 finalAmbientColour = vec3(0.25, 0.25, 0.25);
	finalAmbientColour *= perturbedNormalAndHeight.a;
	
	vec4 attenuatedLightEmissiveColour = lightEmissiveColour * perturbedNormalAndHeight.a;
	
	vec3 finalLight = (dynamicLightColour.xyz) + finalAmbientColour + attenuatedLightEmissiveColour.xyz;
	
	finalLight = clamp(finalLight, 0.0, 1.0);
	
	//Apply lighting to the base colour
	colour.xyz *= finalLight;
	
	//height = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	vec4 result = colour;
	
    gl_FragColor = result;
}