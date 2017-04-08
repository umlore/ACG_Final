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


// ----------------------------------------------
// a shader for a black & white checkerboard
vec3 checkerboard(vec3 pos) {
  // determine the parity of this point in the 3D checkerboard
  int count = 0;
  if (mod(pos.x,0.3)> 0.15) count++;
  if (mod(pos.y,0.3)> 0.15) count++;
  if (mod(pos.z,0.3)> 0.15) count++;
  if (count == 1 || count == 3) {
    return vec3(0.1,0.1,0.1);
  } else {
    return vec3(1,1,1);
  }
}


// ----------------------------------------------
// a shader for a black & white checkerboard
vec3 orange(vec3 pos, inout vec3 normal) {
  // the base color is orange!
  vec3 color = vec3(1.0,0.5,0.1);


  // FIXME: uncommenting seems to have really slow performance even
  //    when not rendering "orange".  Perhaps we should have separate
  //    shaders and not load the orange shader if it's not being used.

  // high frequency noise added to the normal for the bump map
  
  
  // ASSIGNMENT: UNCOMMENT TO SEE THE WRINKLY ORANGE!
  //   normal = normalize(normal+0.4*noise3(70.0*pos));

  
  return color;
}

// ----------------------------------------------
// a shader for a black & white checkerboard
vec3 wood(vec3 pos, vec3 normal) {


  // ASSIGNMENT:  IMPLEMENT A CREATIVE "WOOD" SHADER
  //  (you may also edit the vertex shader as you like)


  // NOTE/FIXME: might be more performance efficient to pull this into a
  //   separate shader file & only compile current shader?


  // currently just a solid brown color
  return vec3(0.6,0.4,0.2);
}

// ----------------------------------------------
void main(){

  vec3 LightColor = vec3(1,1,1);
  float LightPower = 4.0f; 

  // surface normal
  vec3 surface_normal =  vertexNormal_worldspace;
  
  // Material properties
  vec3 MaterialDiffuseColor = myColor;
  if (whichshader == 1) {
    MaterialDiffuseColor = checkerboard(vertexPosition_worldspace);
  } else if (whichshader == 2) {
    vec3 normal2;
    MaterialDiffuseColor = orange(vertexPosition_worldspace,surface_normal);
  } else if (whichshader == 3) {
    MaterialDiffuseColor = wood(vertexPosition_worldspace,surface_normal);
  }

  vec3 MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor;
  vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);
  if(!gl_FrontFacing ) {
    MaterialDiffuseColor = vec3(0.0,0.0,0.6); 
    MaterialAmbientColor = vec3(0.3,0.3,0.3) * MaterialDiffuseColor;
    MaterialSpecularColor = vec3(0.1,0.1,0.3);
    surface_normal = -surface_normal;
  }

  // Direction & distance to the light
  vec3 dirToLight = (LightPosition_worldspace - vertexPosition_worldspace);
  float distanceToLight = length( dirToLight );
  dirToLight = normalize(dirToLight);
  
  // Cosine of the angle between the normal and the light direction, 
  // clamped above 0
  //  - light is at the vertical of the triangle -> 1
  //  - light is perpendicular to the triangle -> 0
  //  - light is behind the triangle -> 0
  float cosTheta = clamp( dot( surface_normal,dirToLight ), 0,1 );

  // REFLECTION  
  // Eye vector (towards the camera)
  vec3 E = normalize(EyeDirection_cameraspace);
  // Direction in which the triangle reflects the light
  vec3 R = reflect(-dirToLight,surface_normal);
  // Cosine of the angle between the Eye vector and the Reflect vector,
  // clamped to 0
  //  - Looking into the reflection -> 1
  //  - Looking elsewhere -> < 1
  float cosAlpha = clamp( dot( E,R ), 0,1 );
  

  if (colormode == 0) {
    // mode 0: NO LIGHTING
    // mode 1: NO LIGHTING
    color = MaterialDiffuseColor;
  } else if (colormode == 1) {
    // mode 1: STANDARD PHONG LIGHTING (LIGHT ON)
    color = 
      MaterialAmbientColor +
      MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distanceToLight) +
      MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distanceToLight); 
    // NOTE: actually in real-world physics this should be divded by distance^2
    //    (but the dynamic range is probably too big for typical screens)
  } else if (colormode == 2) {
    // mode 2: AMBIENT ONLY (LIGHT OFF) 
    color = MaterialAmbientColor;
  }
}










