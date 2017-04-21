#version 330 core

uniform sampler2D texIn;
uniform vec2 texSize;

out vec3 color;

void main()
{
  //vec3 abc = vec3(gl_FragCoord.x/texSize.x, gl_FragCoord.y/texSize.y, 1.0f);
	vec3 abc = texture(texIn, gl_FragCoord.xy/texSize).xyz;
	color = abc;
}
