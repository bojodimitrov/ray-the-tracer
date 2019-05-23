#include <iostream>
#include <glm/vec3.hpp>

using namespace glm;

class material
{
	// the three properties must sum up to 1
	float absorbtion_;
	float refraction_;
	float reflecion_;
	vec3 color_;

public:
	material(const float absorbtion, const float refraction, const float reflection, const vec3 color) : color_(color)
	{
		if ((absorbtion + refraction + reflection) != 1.0f)
		{
			std::cout << "ERROR::MATERIAL::MATERIAL_PROPERTIES_MUST_SUM_TO_ONE" << std::endl;
		}
		this->absorbtion_ = absorbtion;
		this->refraction_ = refraction;
		this->reflecion_ = reflection;
	}

	float absorb() const
	{
		return this->absorbtion_;
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

struct light_properties
{
	vec3 ambient_color;
	vec3 diffusion_color;
	vec3 specular_color;

	light_properties(const vec3 ambient, const vec3 diffusion, const vec3 specular) :
		ambient_color(ambient),
		diffusion_color(diffusion),
		specular_color(specular) 
	{}
};
