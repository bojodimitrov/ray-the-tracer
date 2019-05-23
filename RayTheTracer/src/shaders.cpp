#include <string>
#include <glad/glad.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../headers/shaders.hpp"
#include <glm/gtc/type_ptr.inl>

#ifndef SHADERS_H
#define SHADERS_H

std::string shaders::read_shader(const char* shader)
{
	std::ifstream shader_file;
	std::string shader_code;
	shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		shader_file.open(shader);
		std::stringstream str_stream;
		str_stream << shader_file.rdbuf();
		shader_code = str_stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}
	shader_file.close();
	return shader_code;
}

shaders::shaders(const char* vertex_shader_path, const char* fragment_shader_path)
{
	//auto vertex_shader_str = read_shader("./src/vertex_shader");
	auto vertex_shader_str = this->read_shader(vertex_shader_path);
	auto vertex_shader_text = vertex_shader_str.c_str();
	//auto fragment_shader_str = read_shader("./src/fragment_shader");
	auto fragment_shader_str = this->read_shader(fragment_shader_path);
	auto fragment_shader_text = fragment_shader_str.c_str();

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

	this->shader_id_ = shader_program;
}

void shaders::use() const
{
	glUseProgram(this->shader_id_);
}

GLuint shaders::get_id() const
{
	return this->shader_id_;
}

void shaders::feed_mat(const char* matrix, glm::mat4 value) const
{
	const auto location = glGetUniformLocation(this->get_id(), matrix);
	glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(value));
}

void shaders::feed_vec(const char* name, glm::vec3 value) const
{
	glUniform3fv(glGetUniformLocation(this->get_id(), name), 1, value_ptr(value));
}

void shaders::feed_float(const char* name, const float value) const
{
	glUniform1f(glGetUniformLocation(this->get_id(), name), value);
}
#endif
