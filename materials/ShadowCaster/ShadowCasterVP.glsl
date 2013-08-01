#version 120

/* This file implements standard programs for depth shadow mapping. 
   These particular ones are suitable for additive lighting models, and
   include 3 techniques to reduce depth fighting on self-shadowed surfaces,
   constant bias, gradient (slope-scale) bias, and a fuzzy shadow map comparison*/

uniform mat4 worldViewProj;
uniform vec4 texelOffsets;
uniform vec4 depthRange;

// Shadow caster vertex program.
void main()
{
	//Get the inputs (mimic Direct3D names)
	vec4 position	= gl_Vertex;
	
	//Perform the calculations (as similar as possible to Direct3D)
	vec4 outPos;
	vec4 outDepth = vec4(0.0, 0.0, 0.0, 0.0);
	position.w = 1.0;
	
    outPos = worldViewProj * position; //Correct way around?
	
	outPos.xy += texelOffsets.zw * outPos.w;

    outDepth.x = outPos.z;
    outDepth.y = outPos.w;
	
	//Set the outputs (mimic Direct3D names)
	gl_Position = outPos;
	gl_TexCoord[0] = outDepth;
}
