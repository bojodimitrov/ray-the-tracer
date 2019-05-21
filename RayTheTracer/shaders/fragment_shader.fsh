#version 110
varying vec3 color;
uniform vec3 light_color;

float ambient_strength = 0.1;
vec3 lighting = ambient_strength * light_color;

void main()
{
    gl_FragColor = vec4(lighting * color, 1.0);
};
