#version 330 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Output data
out vec3 fPosition;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 M;

void main(){
  
  // Output position of the vertex, in clip space : MVP * position
  gl_Position = MVP * vec4(vertexPosition_modelspace,1);
  
  vec4 temp = (M * vec4(vertexPosition_modelspace,1));

  // Position of the vertex, in worldspace : M * position
  fPosition = temp.xyz/temp.w;
}
