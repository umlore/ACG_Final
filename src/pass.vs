#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 1) in vec3 vPosition;

void main(){
	gl_Position = vec4(vPosition,0);
}


