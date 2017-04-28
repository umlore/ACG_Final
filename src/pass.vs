#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vPosition;
out vec3 fPosition;

void main(){
	gl_Position = vec4(vPosition,1.0);
	fPosition = vPosition;
}
