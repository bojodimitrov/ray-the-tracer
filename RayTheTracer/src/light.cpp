#include <../src/shapes.cpp>

class light
{
	sphere* lamp_;
	light_properties* light_props_;

public:
	light(const vec3 location)
	{
		const auto light_color = vec3(1.0f, 1.0f, 1.0f);
		this->light_props_ = new light_properties(light_color * vec3(0.2f), light_color * vec3(0.5f), light_color);
		const auto lights = new material(0.0f, 0.0f, 1.0f, light_props_->specular_color);
		this->lamp_ = new sphere(lights, 20);
		this->lamp_->translate(location, true);
		this->lamp_->scale(vec3(0.05f, 0.05f, 0.05f), true);
	}

	void rotate(const float angle, const vec3 axis) const
	{
		this->lamp_->rotate(angle, axis, false);
	}

	void translate(const vec3 direction) const
	{
		this->lamp_->translate(direction, false);
	}


	void scale(const vec3 magnitude) const
	{
		this->lamp_->scale(magnitude, false);
	}

	vec3 get_location() const
	{
		return this->lamp_->get_centre();
	}

	light_properties* get_properties() const
	{
		return this->light_props_;
	}

	void draw(shaders* shader) const
	{
		this->lamp_->draw(shader);
	}

	~light()
	{
		delete this->lamp_;
		delete this->light_props_;
	}
};