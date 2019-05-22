#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;
layout(location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec3 normal;
varying vec3 fragment_position;
varying vec3 color;
void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	fragment_position = vec3(model * vec4(aPos, 1.0));
	color = aCol;
	normal = mat3(transpose(inverse(model))) * aNormal; 
};