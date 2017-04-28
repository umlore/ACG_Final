#version 330 core

uniform sampler2D albedo; 
uniform sampler2D normal; 
uniform sampler2D inposition;
uniform vec2 texSize;

out vec3 color;

void main()
{
	vec3 mat = texture(albedo, gl_FragCoord.xy/texSize).xyz;
	vec3 norm = texture(normal, gl_FragCoord.xy/texSize).xyz;
	vec3 pos = texture(inposition, gl_FragCoord.xy/texSize).xyz;
	float rate = length(pos); 
  vec3 mix = 0.2f*vec3(gl_FragCoord.x/texSize.x, gl_FragCoord.y/texSize.y, 1.0f)+0.6f*mat+0.2f*norm;
	color = mix;//*(1.0f-(rate/25.0f));
}
