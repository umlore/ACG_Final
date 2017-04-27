#version 330 core

uniform sampler2D albedo; 
uniform sampler2D normal; 
uniform vec2 texSize;

out vec3 color;

void main()
{
	vec3 mat = texture(albedo, gl_FragCoord.xy/texSize).xyz;
	vec3 norm = texture(normal, gl_FragCoord.xy/texSize).xyz;
  vec3 mix = 0.2f*vec3(gl_FragCoord.x/texSize.x, gl_FragCoord.y/texSize.y, 1.0f)+0.4f*mat+0.4f*norm;
	color = mix; 
}
