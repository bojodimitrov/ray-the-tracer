#version 110
attribute vec3 vCol;
attribute vec3 vPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	gl_Position = projection * view * model * vec4(vPos, 1.0);
};