#version 330 core

uniform sampler2D albedo; 
uniform sampler2D normal; 
uniform vec2 texSize;

out vec3 color;

void main()
{
	vec3 abc = texture(albedo, gl_FragCoord.xy/texSize).xyz;
  abc = 0.2f*vec3(gl_FragCoord.x/texSize.x, gl_FragCoord.y/texSize.y, 1.0f)+0.8f*abc;
	color = abc;
}
