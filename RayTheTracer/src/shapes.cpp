// Shapes will be created in local space and will contain model matrix and then transformed (translate, rotate, etc.) in world space
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <../src/structs.cpp>
#include <../headers/shaders.hpp>
#include <../src/material.cpp>
#include <glad/glad.h>
#include <vector>

const double pi = 3.1415926535897;


class shape
{
public:
	virtual void sculpt(vec3) = 0;
	virtual void translate(vec3, bool = false) = 0;
	virtual void rotate(float, vec3, bool = false) = 0;
	virtual void scale(vec3, bool = false) = 0;
	virtual void draw(const shaders*) = 0;
	virtual ~shape() {}
};

class wall : public shape
{
	point* vertices_;
	mat4 model_{};
	mat4 memory_model_{};
	material* material_;
	int number_of_vertices_;

public:
	wall(const material* mat)
	{
		this->number_of_vertices_ = 6;
		this->vertices_ = new point[this->number_of_vertices_ * 3];

		this->vertices_[0] = point(-0.5f, -0.5f, 0.0f);
		this->vertices_[3] = point(0.5f, -0.5f, 0.0f);
		this->vertices_[6] = point(-0.5f, 0.5f, 0.0f);

		this->vertices_[9] = point(0.5f, 0.5f, 0.0f);
		this->vertices_[12] = point(0.5f, -0.5f, 0.0f);
		this->vertices_[15] = point(-0.5f, 0.5f, 0.0f);

		for (auto i = 2; i < this->number_of_vertices_ * 3; i += 3)
		{
			this->vertices_[i] = point(0, 0, 1);
		}
		for (auto i = 1; i < this->number_of_vertices_ * 3; i += 3)
		{
			this->vertices_[i] = point(mat->dye().r, mat->dye().g, mat->dye().b);
		}

		this->model_ = mat4(1.0f);
		this->memory_model_ = mat4(1.0f);

		this->material_ = new material(mat->absorb(), mat->refract(), mat->reflect(), mat->dye());
	}

	void sculpt(const vec3 dimensions) override
	{
		this->scale(dimensions, true);
	}

	void translate(const vec3 direction, const bool forever) override
	{
		this->model_ = glm::translate(this->model_, direction);
		if (forever)
		{
			this->memory_model_ = glm::translate(this->memory_model_, direction);
		}
	}

	void rotate(const float angle, const vec3 axis, const bool forever) override
	{
		// axis needs to be in normal form
		this->model_ = glm::rotate(this->model_, radians(angle), axis);
		if (forever)
		{
			this->memory_model_ = glm::rotate(this->memory_model_, radians(angle), axis);
		}
	}

	void scale(const vec3 vec, const bool forever) override
	{
		this->model_ = glm::scale(this->model_, vec);
		if (forever)
		{
			this->memory_model_ = glm::scale(this->memory_model_, vec);
		}
	}

	void draw(const shaders* shader) override
	{
		shader->feed_mat("model", this->model_);
		shader->feed_vec("material.ambient", this->material_->dye());
		shader->feed_vec("material.diffuse", this->material_->dye());
		shader->feed_vec("material.specular", vec3(0.5f));
		shader->feed_float("material.shininess", 128.0f);

		glBufferData(GL_ARRAY_BUFFER, sizeof(point) * this->number_of_vertices_ * 3, this->vertices_, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, this->number_of_vertices_);

		this->model_ = mat4(this->memory_model_);
	}

	vec3 get_corner(int bottom, int top) const
	{
		if (bottom != 0 && bottom != 1)
		{
			bottom = 0;
		}
		if (top != 0 && top != 1)
		{
			top = 0;
		}
		const auto index = bottom * 3 + top * 6;
		return this->model_ * vec4(this->vertices_[index].x, this->vertices_[index].y, this->vertices_[index].z, 1.0f);
	}

	~wall()
	{
		delete this->vertices_;
		delete this->material_;
	}
};

class cuboid : public shape
{
	point* vertices_;
	int number_of_vertices_;
	mat4 model_{};
	mat4 memory_model_{};
	material* material_;

public:
	cuboid(const material* mat)
	{
		this->number_of_vertices_ = 36;
		this->vertices_ = new point[this->number_of_vertices_ * 3];

		// floor
		this->vertices_[0] = point(-0.5f, -0.5f, -0.5f);
		this->vertices_[3] = point(0.5f, -0.5f, -0.5f);
		this->vertices_[6] = point(0.5f, 0.5f, -0.5f);
		this->vertices_[9] = point(-0.5f, -0.5f, -0.5f);
		this->vertices_[12] = point(0.5f, 0.5f, -0.5f);
		this->vertices_[15] = point(-0.5f, 0.5f, -0.5f);
		for (auto i = 2; i < 18; i += 3)
		{
			this->vertices_[i] = point(0, 0, -1);
		}
		// front wall
		this->vertices_[18] = point(-0.5f, -0.5f, -0.5f);
		this->vertices_[21] = point(0.5, -0.5f, -0.5f);
		this->vertices_[24] = point(0.5, -0.5f, 0.5);
		this->vertices_[27] = point(-0.5f, -0.5f, -0.5f);
		this->vertices_[30] = point(0.5, -0.5f, 0.5);
		this->vertices_[33] = point(-0.5f, -0.5f, 0.5);
		for (auto i = 20; i < 36; i += 3)
		{
			this->vertices_[i] = point(0, -1, 0);
		}
		// left wall
		this->vertices_[36] = point(-0.5f, -0.5f, -0.5f);
		this->vertices_[39] = point(-0.5f, 0.5, -0.5f);
		this->vertices_[42] = point(-0.5f, 0.5, 0.5);
		this->vertices_[45] = point(-0.5f, -0.5f, -0.5f);
		this->vertices_[48] = point(-0.5f, 0.5, 0.5);
		this->vertices_[51] = point(-0.5f, -0.5f, 0.5);
		for (auto i = 38; i < 54; i += 3)
		{
			this->vertices_[i] = point(-1, 0, 0);
		}
		// right wall
		this->vertices_[54] = point(0.5, -0.5f, -0.5f);
		this->vertices_[57] = point(0.5, 0.5, -0.5f);
		this->vertices_[60] = point(0.5, 0.5, 0.5);
		this->vertices_[63] = point(0.5, -0.5f, -0.5f);
		this->vertices_[66] = point(0.5, 0.5, 0.5);
		this->vertices_[69] = point(0.5, -0.5f, 0.5);
		for (auto i = 56; i < 72; i += 3)
		{
			this->vertices_[i] = point(1, 0, 0);
		}
		// bottom wall
		this->vertices_[72] = point(-0.5f, 0.5, -0.5f);
		this->vertices_[75] = point(0.5, 0.5, -0.5f);
		this->vertices_[78] = point(0.5, 0.5, 0.5);
		this->vertices_[81] = point(-0.5f, 0.5, -0.5f);
		this->vertices_[84] = point(0.5, 0.5, 0.5);
		this->vertices_[87] = point(-0.5f, 0.5, 0.5);
		for (auto i = 74; i < 90; i += 3)
		{
			this->vertices_[i] = point(0, 1, 0);
		}
		// ceiling
		this->vertices_[90] = point(-0.5f, -0.5f, 0.5);
		this->vertices_[93] = point(0.5, -0.5f, 0.5);
		this->vertices_[96] = point(0.5, 0.5, 0.5);
		this->vertices_[99] = point(-0.5f, -0.5f, 0.5);
		this->vertices_[102] = point(0.5, 0.5, 0.5);
		this->vertices_[105] = point(-0.5f, 0.5, 0.5);
		for (auto i = 92; i < 108; i += 3)
		{
			this->vertices_[i] = point(0, 0, 1);
		}
		for (auto i = 1; i < this->number_of_vertices_ * 3; i += 3)
		{
			this->vertices_[i] = point(mat->dye().r, mat->dye().g, mat->dye().b);
		}

		this->model_ = mat4(1.0f);
		this->memory_model_ = mat4(1.0f);

		this->material_ = new material(mat->absorb(), mat->refract(), mat->reflect(), mat->dye());
	}

	void sculpt(const vec3 dimensions) override
	{
		this->scale(dimensions, true);
	}

	void translate(const vec3 direction, const bool forever) override
	{
		this->model_ = glm::translate(this->model_, direction);
		if (forever)
		{
			this->memory_model_ = glm::translate(this->memory_model_, direction);
		}
	}

	void rotate(const float angle, const vec3 axis, const bool forever) override
	{
		// axis needs to be in normal form
		this->model_ = glm::rotate(this->model_, radians(angle), axis);
		if (forever)
		{
			this->memory_model_ = glm::rotate(this->memory_model_, radians(angle), axis);
		}
	}

	void scale(const vec3 vec, const bool forever) override
	{
		this->model_ = glm::scale(this->model_, vec);
		if (forever)
		{
			this->memory_model_ = glm::scale(this->memory_model_, vec);
		}
	}

	void draw(const shaders* shader) override
	{
		shader->feed_mat("model", this->model_);
		shader->feed_vec("material.ambient", this->material_->dye());
		shader->feed_vec("material.diffuse", this->material_->dye());
		shader->feed_vec("material.specular", vec3(0.5f));
		shader->feed_float("material.shininess", 128.0f);

		glBufferData(GL_ARRAY_BUFFER, sizeof(point) * this->number_of_vertices_ * 3, this->vertices_, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, this->number_of_vertices_);

		this->model_ = mat4(this->memory_model_);
	}

	~cuboid()
	{
		delete this->vertices_;
		delete this->material_;
	}
};

class sphere : public shape
{
	point* vertices_;
	int number_of_vertices_;
	mat4 model_{};
	mat4 memory_model_{};
	material* material_;

public:
	sphere(const material* mat, const int density)
	{
		auto vertices = std::vector<point>();
		for (auto i = -(density / 2); i < density / 2; i++)
		{
			for (auto j = 0; j < density; j++)
			{
				const auto phi = i * (glm::pi<float>() / density);
				const auto phi_next = (i + 1) * (glm::pi<float>() / density);
				const auto theta = j * 2 * glm::pi<float>() / density;
				const auto theta_next = (j + 1) * 2 * glm::pi<float>() / density;

				vertices.emplace_back(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi));
				vertices.emplace_back(mat->dye().x, mat->dye().y, mat->dye().z);
				vertices.emplace_back(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi));

				vertices.emplace_back(cos(phi_next) * cos(theta), cos(phi_next) * sin(theta), sin(phi_next));
				vertices.emplace_back(mat->dye().x, mat->dye().y, mat->dye().z);
				vertices.emplace_back(cos(phi_next) * cos(theta), cos(phi_next) * sin(theta), sin(phi_next));

				vertices.emplace_back(cos(phi_next) * cos(theta_next), cos(phi_next) * sin(theta_next), sin(phi_next));
				vertices.emplace_back(mat->dye().x, mat->dye().y, mat->dye().z);
				vertices.emplace_back(cos(phi_next) * cos(theta_next), cos(phi_next) * sin(theta_next), sin(phi_next));

				vertices.emplace_back(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi));
				vertices.emplace_back(mat->dye().x, mat->dye().y, mat->dye().z);
				vertices.emplace_back(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi));

				vertices.emplace_back(cos(phi_next) * cos(theta_next), cos(phi_next) * sin(theta_next), sin(phi_next));
				vertices.emplace_back(mat->dye().x, mat->dye().y, mat->dye().z);
				vertices.emplace_back(cos(phi_next) * cos(theta_next), cos(phi_next) * sin(theta_next), sin(phi_next));

				vertices.emplace_back(cos(phi) * cos(theta_next), cos(phi) * sin(theta_next), sin(phi));
				vertices.emplace_back(mat->dye().x, mat->dye().y, mat->dye().z);
				vertices.emplace_back(cos(phi) * cos(theta_next), cos(phi) * sin(theta_next), sin(phi));
			}
		}

		this->vertices_ = new point[vertices.size()];
		this->number_of_vertices_ = float(vertices.size() / 3);
		for (auto i = 0; i < int(vertices.size()); i++)
		{
			this->vertices_[i] = point(vertices[i].x, vertices[i].y, vertices[i].z);
		}

		this->model_ = mat4(1.0f);
		this->memory_model_ = mat4(1.0f);

		this->material_ = new material(mat->absorb(), mat->refract(), mat->reflect(), mat->dye());
	}

	void sculpt(const vec3 dimensions) override
	{
		this->scale(dimensions, true);
	}

	void translate(const vec3 direction, const bool forever) override
	{
		this->model_ = glm::translate(this->model_, direction);
		if (forever)
		{
			this->memory_model_ = glm::translate(this->memory_model_, direction);
		}
	}

	void rotate(const float angle, const vec3 axis, const bool forever) override
	{
		// axis needs to be in normal form
		this->model_ = glm::rotate(this->model_, radians(angle), axis);
		if (forever)
		{
			this->memory_model_ = glm::rotate(this->memory_model_, radians(angle), axis);
		}
	}

	void scale(const vec3 vec, const bool forever) override
	{
		this->model_ = glm::scale(this->model_, vec);
		if (forever)
		{
			this->memory_model_ = glm::scale(this->memory_model_, vec);
		}
	}

	void draw(const shaders* shader) override
	{
		shader->feed_mat("model", this->model_);
		shader->feed_vec("material.ambient", this->material_->dye());
		shader->feed_vec("material.diffuse", this->material_->dye());
		shader->feed_vec("material.specular", vec3(0.3f));
		shader->feed_float("material.shininess", 128.0f);

		glBufferData(GL_ARRAY_BUFFER, sizeof(point) * this->number_of_vertices_ * 3, this->vertices_, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, this->number_of_vertices_);

		this->model_ = mat4(this->memory_model_);
	}

	vec3 get_centre() const
	{
		return this->model_ * vec4(0.0f, 0.0f, 0.0f, 1.0);
	}

	~sphere()
	{
		delete this->vertices_;
		delete this->material_;
	}
};