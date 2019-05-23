#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 
uniform Material material; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light; 

varying vec3 normal;
varying vec3 fragment_position;

uniform vec3 view_pos;

void main()
{
	vec3 ambient = light.ambient * material.ambient;

	vec3 norm = normalize(normal);
	vec3 light_direction = normalize(light.position - fragment_position);
	float diff = max(dot(norm, light_direction), 0.0);
	vec3 diffuse =  light.diffuse * (material.diffuse * diff);

	vec3 view_direction = normalize(view_pos - fragment_position);
	vec3 reflect_direction = reflect(-light_direction, norm);  
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
	vec3 specular = light.specular * (material.specular * spec);

	vec3 lighting = ambient + diffuse + specular;
    gl_FragColor = vec4(lighting, 1.0);
};
