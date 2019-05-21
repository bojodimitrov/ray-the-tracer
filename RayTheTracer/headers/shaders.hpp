#pragma once
#include <glad/glad.h>
#include <string>
#include <glm/mat4x2.hpp>

GLuint setup_shaders();

class shaders
{
	GLuint shader_id_;
	static std::string read_shader(const char*);
public:
	shaders(const char*, const char*);
	void use() const;
	GLuint get_id() const;
	void feed_mat(const char*, glm::mat4) const;
	void feed_vec(const char*, glm::vec3) const;
};