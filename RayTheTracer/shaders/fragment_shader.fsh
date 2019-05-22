#version 330 core
varying vec3 color;
varying vec3 normal;
varying vec3 fragment_position;

uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 view_pos;

float specular_strength = 0.8;
float ambient_strength = 0.1;

void main()
{
	vec3 light_direction = normalize(light_pos - fragment_position);
	vec3 view_direction = normalize(view_pos - fragment_position);
	vec3 norm = normalize(normal);
	vec3 reflect_direction = reflect(-light_direction, norm);  

	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), 128);
	vec3 specular = specular_strength * spec * light_color;  

	float diff = max(dot(norm, light_direction), 0.0);
	vec3 diffuse = diff * light_color;

	vec3 lighting = (ambient_strength * light_color) + diffuse + specular;
    gl_FragColor = vec4(lighting * color, 1.0);
};
