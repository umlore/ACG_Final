#version 330 core

uniform sampler2D texIn;
uniform vec2 texSize;

out vec3 color;

void main()
{
  vec3 abc = texture( texIn, gl_FragCoord.xy/texSize).xyz;
}
