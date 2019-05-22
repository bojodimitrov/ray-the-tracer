#include <iostream>
#include <glm/vec3.hpp>

using namespace glm;

class material
{
	// the three properties must sum up to 1
	float diffusion_;
	float refraction_;
	float reflecion_;
	vec3 color_;

public:
	material(const float diffusion, const float refraction, const float reflection, const vec3 color) : color_(color)
	{
		if((diffusion + refraction + reflection) != 1.0f)
		{
			std::cout << "ERROR::MATERIAL::MATERIAL_PROPERTIES_MUST_SUM_TO_ONE" << std::endl;
		}
		this->diffusion_ = diffusion;
		this->refraction_ = refraction;
		this->reflecion_ = reflection;
	}

	float diffuse() const
	{
		return this->diffusion_;
	}

	float refract() const
	{
		return this->refraction_;
	}

	float reflect() const
	{
		return this->reflecion_;
	}

	vec3 dye() const
	{
		return this->color_;
	}
};
