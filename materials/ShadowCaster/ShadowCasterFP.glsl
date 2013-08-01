#version 120

/* This file implements standard programs for depth shadow mapping. 
   These particular ones are suitable for additive lighting models, and
   include 3 techniques to reduce depth fighting on self-shadowed surfaces,
   constant bias, gradient (slope-scale) bias, and a fuzzy shadow map comparison*/

// Shadow caster fragment program for high-precision single-channel textures    
void main()
    
{
	//Get the inputs (mimic Direct3D names)
	vec2 depth	= gl_TexCoord[0].xy;
	
    float finalDepth = depth.x / depth.y;
	
    // just smear across all components 
    // therefore this one needs high individual channel precision
    vec4 result = vec4(finalDepth, finalDepth, finalDepth, 1);
	
	gl_FragColor = result;
}
