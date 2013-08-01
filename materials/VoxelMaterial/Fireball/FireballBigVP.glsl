#version 120

uniform mat4 viewProj;
uniform mat4 world;
uniform sampler2D colorMap;

float noiseFromPosition(vec3 position)
{
	vec3 noiseScaleFactors1 = vec3(0.83, 1.15, 1.1956); //Higher numbers gives higher frequesncy
	vec3 noiseScaleFactors2 = vec3(0.53523, 0.473, 0.623); //Higher numbers gives higher frequesncy
	
	vec3 resultVec = vec3(0.0,0.0,0.0);
	resultVec += sin(position * noiseScaleFactors1); //Components now -1 to 1
	resultVec += sin(position * noiseScaleFactors2); //Components now -2 to 2
	
	resultVec += vec3(2.0, 2.0, 2.0); //Components now 0 to 4
	resultVec *= vec3(0.25, 0.25, 0.25); //Components now 0 to 1
	float result = resultVec.x + resultVec.y + resultVec.z;
	result /= 3.0;
	
	return result;
}

void main()																					
{																							
	//Get the inputs (mimic Direct3D names)
	vec4 inLocalPosition	= gl_Vertex;
	vec3 inNormal = gl_Normal;
	
	//Perform the calculations (as similar as possible to Direct3D)
	//inNormal.w = 0.0;
	
	float noiseFrequency = 2.0;
	float scaleFactor = 1.0;
	
	vec4 worldPosition = world * inLocalPosition;
	vec4 worldOrigin = world * vec4(0.0,0.0,0.0,1.0); //Could probably just translate here.
	float distance = length(worldPosition.xyz - worldOrigin.xyz);
	
	float noiseSample = noiseFromPosition(worldPosition.xyz * noiseFrequency);

	//Compute the offset properties	
	float offset = noiseSample * distance * scaleFactor;	
	vec4 offsetWorldPosition = worldPosition + vec4((inNormal * offset), 0.0);
	float offsetDistance = length(offsetWorldPosition.xyz - worldOrigin.xyz);
	offsetDistance /= 2.2;
	
	vec4 texCoord = vec4(offsetDistance / 5.0, 0.5, 0.0, 0.5); //w is lod - not sure what it should be.
	texCoord.x = clamp(texCoord.x, 0.0, 0.99); //Setting sampler mode to clamp doesn't seem to work. Maybe because it's a vertex texture?
	vec4 outVertexColour = texture2D(colorMap, texCoord.xy);
	
	float fadeStart = 6.0;
	float fadeEnd = 7.5;
	float proportion = clamp(distance, fadeStart, fadeEnd);	
	proportion = (proportion - fadeStart) / (fadeEnd - fadeStart);
	outVertexColour.a = 1.0 - proportion;
	
	//Round the positions for the cubic look.
	offsetWorldPosition.xyz = floor(offsetWorldPosition.xyz + vec3(0.5, 0.5, 0.5));	
	offsetWorldPosition += vec4(0.5, 0.5, 0.5, 0.0);
	
	//Final positions.
	vec4 outWorldPosition = offsetWorldPosition;
	vec4 outClipPosition  = viewProj * outWorldPosition;	
	
	//Within our fragment program we generate texture coordinates according to local space positions so that
	//moving meshes do not have the textures scroll across them. But the local space position on our fireball
	//are all on a sphere (not cubic) and even if we went back to local space at the end we would be performing
	//some scaling (the world matrix has a scale component as that's how we grow the fireball). But in the case
	//of the fireball we can acytually use the world position for texture coordinates, so we sneakily set the 
	//local position to be the same as this.
	vec4 outLocalPosition = offsetWorldPosition;

	//Set the outputs (mimic Direct3D names)
	gl_Position = outClipPosition;
	gl_TexCoord[0] = outLocalPosition;
	gl_TexCoord[1] = outWorldPosition;
	gl_FrontColor = outVertexColour;									
}
