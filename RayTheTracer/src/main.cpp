#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <../headers/shaders.hpp>
#include <../src/shapes.cpp>
#include <../src/camera.cpp>
#include <cstdlib>
#include <cstdio>

const unsigned int scr_width = 800;
const unsigned int scr_height = 600;

static void error_callback(const int error, const char* description)
{
	fprintf(stderr, "Error: %d\n", error);
	fprintf(stderr, "Error: %s\n", description);
}


static void close_window_callback(GLFWwindow* window, const int key, int scancode, const int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

GLFWwindow* init()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	const auto window = glfwCreateWindow(scr_width, scr_height, "Ray the tracer", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwSetKeyCallback(window, close_window_callback);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return window;
}

void draw_coordinate_system()
{
	point x_axis[] = {
		point(-2.0f, 0.0f, 0.0f), point(0.0f, 0.0f, 0.0f),
		point(2.0f, 0.0f, 0.0f), point(0.9f, 0.2f, 0.1f)
	};// x is RED

	point y_axis[] = {
		point(0.0f, -2.0f, 0.0f), point(0.0f, 0.0f, 0.0f),
		point(0.0f, 2.0f, 0.0f), point(0.1f, 0.9f, 0.1f),
	};// y is GREEN

	point z_axis[] = {
		point(0.0f, 0.0f, -2.0f), point(0.0f, 0.0f, 0.0f),
		point(0.0f, 0.0f, 2.0f), point(0.2f, 0.5f, 1.0f),
	};// z is BLUE

	glBufferData(GL_ARRAY_BUFFER, sizeof(x_axis), x_axis, GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, 2);

	glBufferData(GL_ARRAY_BUFFER, sizeof(y_axis), y_axis, GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, 2);

	glBufferData(GL_ARRAY_BUFFER, sizeof(z_axis), z_axis, GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, 2);
}

mat4 setup_view(const vec3 view_pos)
{
	auto view = mat4(1.0f);

	view = translate(view, view_pos);

	// initially we are looking frome above
	view = rotate(view, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	view = rotate(view, radians(30.0f), vec3(1.0f, 0.0f, 0.0f));
	view = rotate(view, radians(-30.0f), vec3(0.0f, 0.0f, 1.0f));

	return view;
}


void generate_vertex_array(GLuint *vertex_array, const int triplets)
{
	glGenVertexArrays(1, vertex_array);
	glBindVertexArray(*vertex_array);
	for (auto i = 0; i < triplets; i++)
	{
		glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, 3 * triplets * sizeof(float), reinterpret_cast<void*>(3 * i * sizeof(float)));
		glEnableVertexAttribArray(i);
	}
}

int main()
{
	auto const window = init();
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
	{
		fprintf(stderr, "Error: %s\n", "Failed to initialize GLAD");
		return -1;
	}

	// const auto shader_program = setup_shaders();
	const auto general_shader = new shaders("./shaders/vertex_shader.vsh", "./shaders/fragment_shader.fsh");
	const auto lighting_shader = new shaders("./shaders/lighting_shader.vsh", "./shaders/lighting_shader.fsh");
	const auto axis_shader = new shaders("./shaders/axis_shader.vsh", "./shaders/axis_shader.fsh");
	const auto cam = new camera();
	cam->rotate(-30.0f, vec3(1.0f, 0.0f, 0.0f));
	cam->rotate(-30.0f, vec3(0.0f, 0.0f, 1.0f));
	cam->scale(1.8f);

	GLuint vertex_buffer, vertex_array, light_vertex_array, axis_array;
	glGenBuffers(1, &vertex_buffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	generate_vertex_array(&vertex_array, 3);
	generate_vertex_array(&light_vertex_array, 3);
	generate_vertex_array(&axis_array, 2);

	glEnable(GL_DEPTH_TEST);

	const auto light_position = vec3(1.2f, 1.2f, 0.3f);
	auto light_world_position = vec3(light_position);

	general_shader->use();
	general_shader->feed_vec("light_pos", light_position);

	const auto gold = new material(0.5f, 0.0f, 0.5f, vec3(1.0f, 0.83f, 0.3f));
	const auto light = new material(1.0f, 0.0f, 0.0f, vec3(1.0f, 1.0f, 1.0f));

	shape* rect = new cuboid(gold);
	shape* light_source = new cuboid(light);


	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vertex_array);
		// cam->rotate(-0.02f, vec3(0.0f, 0.0f, 1.0f));
		const auto proj_mat = perspective(radians(cam->get_angle()), float(scr_width) / float(scr_height), 0.1f, 100.0f);

		general_shader->use();
		general_shader->feed_mat("view", cam->get_view_matrix());
		general_shader->feed_mat("projection", proj_mat);
		general_shader->feed_vec("light_color", vec3(1.0f, 1.0f, 1.0f));
		general_shader->feed_vec("view_pos", cam->get_position());
		general_shader->feed_vec("light_pos", light_world_position);

		//rect->rotate(float(glfwGetTime()) * 15.0f, vec3(0.0f, 0.0f, 1.0f));
		rect->draw(general_shader);

		glBindVertexArray(axis_array);
		axis_shader->use();
		axis_shader->feed_mat("view", cam->get_view_matrix());
		axis_shader->feed_mat("projection", proj_mat);
		axis_shader->feed_mat("model", mat4(1.0f));
		draw_coordinate_system();

		glBindVertexArray(light_vertex_array);
		lighting_shader->use();
		lighting_shader->feed_mat("view", cam->get_view_matrix());
		lighting_shader->feed_mat("projection", proj_mat);
		light_source->rotate(float(glfwGetTime()) * 30.0f, vec3(0.0f, 0.0f, 1.0f));
		light_source->translate(light_position);
		light_source->scale(vec3(0.1f, 0.1f, 0.1f));

		light_world_position = light_source->get_location();
		light_source->draw(lighting_shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete rect;
	delete gold;
	delete light;
	delete cam;
	delete light_source;
	delete general_shader;
	delete lighting_shader;
	delete axis_shader;

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
