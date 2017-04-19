#version 330 core

// Interpolated values from the vertex shader
in vec3 interpWorldPosition;
in vec3 interpWorldNormal;
in vec3 interpEyeDirection;
in vec3 interpColor;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform int colormode;

// ----------------------------------------------
void main(){

  vec3 LightColor = vec3(1,1,1);
  float LightPower = 0.0f;

  // surface normal
  vec3 surface_normal = interpWorldNormal; 
  
  // Material properties
  vec3 MaterialDiffuseColor = interpColor; 

  if(!gl_FrontFacing ) {
    MaterialDiffuseColor = vec3(0.0,0.0,0.6); 
    surface_normal = -surface_normal;
  }

  color = MaterialDiffuseColor;
}










