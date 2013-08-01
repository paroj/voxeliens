#version 120

uniform mat4 viewProj;
uniform mat4 world;

void main()																					
{				
	//Get the inputs (mimic Direct3D names)
	vec4 inPosition	= gl_Vertex;
	vec4 inColour = gl_Color;
	
	//Perform the calculations (as similar as possible to Direct3D)
	vec4 outLocalPosition = inPosition;
	vec4 outWorldPosition = world * outLocalPosition;
	vec4 outClipPosition = viewProj * outWorldPosition;
	vec4 outColour = inColour;
	
	//Set the outputs (mimic Direct3D names)
	gl_Position = outClipPosition;
	gl_TexCoord[0] = outLocalPosition;
	gl_TexCoord[1] = outWorldPosition;
	gl_FrontColor = outColour;															
}
