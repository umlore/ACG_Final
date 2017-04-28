#version 330 core

in vec3 fPosition;

uniform sampler2D albedo; 
uniform sampler2D normal; 
uniform sampler2D inposition;

uniform vec2 texSize;
uniform float radius;
uniform vec3 colorin;
/* bad name, this is a light position*/
uniform vec3 cameraPosition;
uniform vec3 lightCenter; 

out vec3 color;

void main()
{
	
	vec3 mat = texture(albedo, gl_FragCoord.xy/texSize).xyz;
	vec3 norm = texture(normal, gl_FragCoord.xy/texSize).xyz;
	vec3 pos = texture(inposition, gl_FragCoord.xy/texSize).xyz;

	vec3 directionToLight = normalize(lightCenter - pos);
	float lightDot = max(0.0f, dot(directionToLight,norm));

	float fragDistToLight = distance(lightCenter, pos);
	
	float intensity = 1.0f - (fragDistToLight / radius);

	color = intensity*mat*colorin*lightDot;
}

