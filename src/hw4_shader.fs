#version 330 core

// Interpolated values from the vertex shader
in vec3 vertexPosition_worldspace;
in vec3 EyeDirection_cameraspace;
in vec3 myColor;
in vec3 vertexNormal_worldspace;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform vec3 LightPosition_worldspace;
uniform int colormode;
uniform int whichshader;
uniform vec3 cameraPosition_worldspace;

void main()
{
	vec3 origin = vec3(0f,0f,0f);
	float distance = length( vertexPosition_worldspace - cameraPosition_worldspace)/ 25.0f;
  vec3 MaterialDiffuseColor = myColor;
  color = vec3(0.3,0.3,0.3) * MaterialDiffuseColor;
	//color = (vertexNormal_worldspace + vec3(1f,1f,1f))/2f;
}

#if 0

// ----------------------------------------------
/*
void main(){

  vec3 LightColor = vec3(1,1,1);
  float LightPower = 0.0f;

  // surface normal
  vec3 surface_normal =  vertexNormal_worldspace;
  
  // Material properties
  vec3 MaterialDiffuseColor = myColor;
  
  vec3 MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor;
  vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

  if(!gl_FrontFacing ) {
    MaterialDiffuseColor = vec3(0.0,0.0,0.6); 
    MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor;
    MaterialSpecularColor = vec3(0.1,0.1,0.3);
    surface_normal = -surface_normal;
  }

	if (gl_FragCoord.x < 0f)
	{
		color = vec3(0,0,0);
	}
}
#endif
