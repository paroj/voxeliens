#version 120

uniform mat4 viewProj;
uniform mat4 world;

vec4 floatToRGBA(float inputVal)
{	
	//Store the input in each component
	vec4 inputVec = vec4(inputVal, inputVal, inputVal, inputVal);
	
	//Convert each component to a value in the range 0-15
	vec4 result = floor(inputVec / vec4(4096.0, 256.0, 16.0, 1.0));	
	vec4 shiftedResult = vec4(0.0, result.rgb) * 16.0;	
	result -= shiftedResult;
	
	//Convert to range 0-1
	result /= 15.0;
	
	//return the result	
	return result;
}

void main()																					
{
	//Get the inputs (mimic Direct3D names)
	vec4 inPosition	= gl_Vertex;
	
	//Perform the calculations (as similar as possible to Direct3D)
	float material = inPosition.w;
	inPosition.w = 1.0;
	vec4 outLocalPosition = inPosition;
	vec4 outWorldPosition = world * outLocalPosition;
	vec4 outClipPosition = viewProj * outWorldPosition;
	vec4 outColour = floatToRGBA(material);

	//Set the outputs (mimic Direct3D names)
	gl_Position = outClipPosition;
	gl_TexCoord[0] = outLocalPosition;
	gl_TexCoord[1] = outWorldPosition;
	gl_FrontColor = outColour;
}
