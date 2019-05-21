// Shapes will be created in local space and will contain model matrix and then transformed (translate, rotate, etc.) in world space
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <../src/structs.cpp>
#include <../headers/shaders.hpp>
#include <glad/glad.h>
#include <iostream>

class shape
{
public:
	virtual void translate(glm::vec3, bool = false) = 0;
	virtual void rotate(float, glm::vec3, bool = false) = 0;
	virtual void scale(glm::vec3, bool = false) = 0;
	virtual void draw(const shaders*) = 0;
	virtual ~shape() {}
};


class rectangle : public shape
{
	point* vertices_;
	int number_of_vertices_;
	glm::mat4 model_{};
	glm::mat4 memory_model_{};

public:
	rectangle(const glm::vec3 dimensions, const glm::vec3 color)
	{
		this->number_of_vertices_ = 36;
		this->vertices_ = new point[this->number_of_vertices_ * 2];

		// floor
		this->vertices_[0] = point(0.0f, 0.0f, 0.0f);
		this->vertices_[2] = point(dimensions.x, 0.0f, 0.0f);
		this->vertices_[4] = point(dimensions.x, dimensions.y, 0.0f);
		this->vertices_[6] = point(0.0f, 0.0f, 0.0f);
		this->vertices_[8] = point(dimensions.x, dimensions.y, 0.0f);
		this->vertices_[10] = point(0.0f, dimensions.y, 0.0f);

		// front wall
		this->vertices_[12] = point(0.0f, 0.0f, 0.0f);
		this->vertices_[14] = point(dimensions.x, 0.0f, 0.0f);
		this->vertices_[16] = point(dimensions.x, 0.0f, dimensions.z);
		this->vertices_[18] = point(0.0f, 0.0f, 0.0f);
		this->vertices_[20] = point(dimensions.x, 0.0f, dimensions.z);
		this->vertices_[22] = point(0.0f, 0.0f, dimensions.z);

		// left wall
		this->vertices_[24] = point(0.0f, 0.0f, 0.0f);
		this->vertices_[26] = point(0.0f, dimensions.y, 0.0f);
		this->vertices_[28] = point(0.0f, dimensions.y, dimensions.z);
		this->vertices_[30] = point(0.0f, 0.0f, 0.0f);
		this->vertices_[32] = point(0.0f, dimensions.y, dimensions.z);
		this->vertices_[34] = point(0.0f, 0.0f, dimensions.z);

		// right wall
		this->vertices_[36] = point(dimensions.x, 0.0f, 0.0f);
		this->vertices_[38] = point(dimensions.x, dimensions.y, 0.0f);
		this->vertices_[40] = point(dimensions.x, dimensions.y, dimensions.z);
		this->vertices_[42] = point(dimensions.x, 0.0f, 0.0f);
		this->vertices_[44] = point(dimensions.x, dimensions.y, dimensions.z);
		this->vertices_[46] = point(dimensions.x, 0.0f, dimensions.z);

		// bottom wall
		this->vertices_[48] = point(0.0f, dimensions.y, 0.0f);
		this->vertices_[50] = point(dimensions.x, dimensions.y, 0.0f);
		this->vertices_[52] = point(dimensions.x, dimensions.y, dimensions.z);
		this->vertices_[54] = point(0.0f, 0.0f, 0.0f);
		this->vertices_[56] = point(dimensions.x, dimensions.y, dimensions.z);
		this->vertices_[58] = point(0.0f, dimensions.y, dimensions.z);

		// ceiling
		this->vertices_[60] = point(0.0f, 0.0f, dimensions.z);
		this->vertices_[62] = point(dimensions.x, 0.0f, dimensions.z);
		this->vertices_[64] = point(dimensions.x, dimensions.y, dimensions.z);
		this->vertices_[66] = point(0.0f, 0.0f, 0.0f);
		this->vertices_[68] = point(dimensions.x, dimensions.y, dimensions.z);
		this->vertices_[70] = point(0.0f, dimensions.y, dimensions.z);

		for (auto i = 1; i < this->number_of_vertices_ * 2; i += 2)
		{
			this->vertices_[i] = point(color.r, color.g, color.b);
		}

		this->model_ = glm::mat4(1.0f);
		this->memory_model_ = glm::mat4(1.0f);
	}

	void translate(const glm::vec3 direction, const bool forever) override
	{
		this->model_ = glm::translate(this->model_, direction);
		if (forever)
		{
			this->memory_model_ = glm::translate(this->memory_model_, direction);
		}
	}

	void rotate(const float angle, const glm::vec3 axis, bool forever) override
	{
		// axis needs to be in normal form
		this->model_ = glm::rotate(this->model_, glm::radians(angle), axis);
	}

	void scale(const glm::vec3 vec, bool forever) override
	{
		this->model_ = glm::scale(this->model_, vec);
	}

	void draw(const shaders* shader) override
	{
		shader->use();

		const auto model_loc = glGetUniformLocation(shader->get_id(), "model");
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, value_ptr(this->model_));

		glBufferData(GL_ARRAY_BUFFER, sizeof(point) * this->number_of_vertices_ * 2, this->vertices_, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, this->number_of_vertices_);


		this->model_ = glm::mat4(this->memory_model_);

		glUniformMatrix4fv(model_loc, 1, GL_FALSE, value_ptr(glm::mat4(1.0f)));
	}

	~rectangle()
	{
		delete this->vertices_;
	}
};

class sphere
{

};