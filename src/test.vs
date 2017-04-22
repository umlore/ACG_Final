#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vModelPosition;
layout(location = 1) in vec3 vModelNormal;
layout(location = 2) in vec3 vColor;

// Output data
out vec3 interpWorldPosition;
out vec3 interpWorldNormal;
out vec3 interpCameraNormal;
out vec3 interpEyeDirection;
out vec3 interpColor;

// Values that stay constant for the whole mesh.
uniform mat4 modelView;
uniform mat4 view;
uniform mat4 modelWorldOffset;

void main(){
  
  // Output position of the vertex, in clip space : MVP * position
  gl_Position =  modelView * vec4(vModelPosition,1);
  
  // Position of the vertex, in worldspace : M * position
  interpWorldPosition = (modelWorldOffset * vec4(vModelPosition,1)).xyz;
  
  // Vector that goes from the vertex to the camera, in camera space.
  // In camera space, the camera is at the origin (0,0,0).
  vec3 vertexPosition_cameraspace = ( view * modelWorldOffset * vec4(vModelPosition,1)).xyz;
  
  interpEyeDirection = vec3(0,0,0) - vertexPosition_cameraspace;
  
  interpWorldNormal = normalize (modelWorldOffset * vec4(vModelNormal,0)).xyz; 
  
  // pass color to the fragment shader
  interpColor = vColor;
}


