#version 330 core

// Interpolated values from the vertex shader
in vec3 vertexPosition_worldspace;
in vec3 vertexNormal_worldspace;

// Ouput data
out vec3 color;
out vec3 normal;
out vec3 outposition;

// uniforms
uniform vec3 colorin;

void main()
{
	color = colorin;
	normal = (vec3(1,1,1) + vertexNormal_worldspace) / 2.0f; 
	outposition = vertexPosition_worldspace;
}

