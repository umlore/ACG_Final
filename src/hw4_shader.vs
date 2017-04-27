#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

// Output data
out vec3 vertexPosition_worldspace;
out vec3 vertexNormal_worldspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

void main(){
  
  // Output position of the vertex, in clip space : MVP * position
  gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
  
  vec4 temp = (M * vec4(vertexPosition_modelspace,1));

  // Position of the vertex, in worldspace : M * position
  vertexPosition_worldspace = temp.xyz/temp.w;
  
  vertexNormal_worldspace = normalize (M * vec4(vertexNormal_modelspace,0)).xyz; 
}
