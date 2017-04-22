#version 330 core

uniform sampler2D texIn;
uniform vec2 texSize;

out vec3 color;

void main()
{
	vec3 abc = texture(texIn, gl_FragCoord.xy/texSize).xyz;
  abc = 0.5f*vec3(gl_FragCoord.x/texSize.x, gl_FragCoord.y/texSize.y, 1.0f)+0.5f*abc;
	color = abc;
}
