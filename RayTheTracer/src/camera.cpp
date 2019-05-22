#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class camera
{
	float angle_;
	glm::vec3 position_;
	glm::vec3 up_;
	glm::vec3 target_;

public:
	camera(
		const float angle = 45.0f,
		const glm::vec3 position = glm::vec3(0.0f, -3.0f, 0.0f),
		const glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f),
		const glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f)
		)
		: angle_(angle),
		position_(position),
		up_(up),
		target_(target)
	{
	}

	// Constructor with scalar values
	camera(
		const float pos_x, const float pos_y, const float pos_z, const float up_x, const float up_y, const float up_z, const float tar_x, const float tar_y, const float tar_z,
		const float angle = 45.0f
	): angle_(angle)
	{
		this->position_ = glm::vec3(pos_x, pos_y, pos_z);
		this->up_ = glm::vec3(up_x, up_y, up_z);
		this->target_ = glm::vec3(tar_x, tar_y, tar_z);
	}

	void rotate(const float angle, const glm::vec3 axis)
	{
		const auto rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
		this->position_ = rotation_matrix * glm::vec4(this->position_.x, this->position_.y, this->position_.z, 0.0);
	}

	void translate(const glm::vec3 translation)
	{
		const auto translation_matrix = glm::translate(glm::mat4(1.0f), translation);
		this->position_ = translation_matrix * glm::vec4(this->position_.x, this->position_.y, this->position_.z, 0.0);
	}

	void scale(const float scalar)
	{
		this->position_ = scalar * this->position_;
	}

	float get_angle() const
	{
		return this->angle_;
	}

	glm::mat4 get_view_matrix() const
	{
		return lookAt(position_, target_, up_);
	}

	glm::vec3 get_position() const
	{
		return this->position_;
	}
};
#endif