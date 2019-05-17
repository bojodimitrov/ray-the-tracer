#include <string>
#include <glad/glad.h>
#include <cstdio>
#include <iostream>

#ifndef SHADERS_H
#define SHADERS_H

static const char* vertex_shader_text =
"#version 110\n"
"attribute vec3 vCol;\n"
"attribute vec3 vPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";
static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";


GLuint setup_shaders()
{
	const auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
	glCompileShader(vertex_shader);
	// check for shader errors
	int success;
	char info_log[512];
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
		fprintf(stderr, "Error: %s\n", "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
	}
	// fragment shader
	const auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
		fprintf(stderr, "Error: %s\n", "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");
	}
	// now link shaders
	const auto shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
		fprintf(stderr, "Error: %s\n", "ERROR::SHADER::PROGRAM::LINKING_FAILED\n");
	}
	// now delete shaders
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}
#endif
