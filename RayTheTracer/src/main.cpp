#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <../headers/shaders.hpp>
#include <../src/shapes.cpp>
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

	glBufferData(GL_ARRAY_BUFFER, sizeof(x_axis) , x_axis, GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, 2);

	glBufferData(GL_ARRAY_BUFFER, sizeof(y_axis), y_axis, GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, 2);

	glBufferData(GL_ARRAY_BUFFER, sizeof(z_axis), z_axis, GL_STATIC_DRAW);
	glDrawArrays(GL_LINES, 0, 2);
}

glm::mat4 setup_view()
{
	auto view = glm::mat4(1.0f);

	view = translate(view, glm::vec3(0.0f, 0.0f, -6.0f));

	// initially we are looking frome above
	view = rotate(view, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = rotate(view, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = rotate(view, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));


	return view;
}

glm::mat4 setup_proj()
{
	auto projection = glm::perspective(glm::radians(45.0f), float(scr_width) / float(scr_height), 0.1f, 100.0f);
	return projection;
}

void generate_vertex_array(GLuint *vertex_array)
{
	glGenVertexArrays(1, vertex_array);
	glBindVertexArray(*vertex_array);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
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
	const auto axis_shader = new shaders("./shaders/vertex_shader.vsh", "./shaders/axis_shader.fsh");

	GLuint vertex_buffer, vertex_array, light_vertex_buffer;
	glGenBuffers(1, &vertex_buffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	generate_vertex_array(&vertex_array);
	generate_vertex_array(&light_vertex_buffer);

	glEnable(GL_DEPTH_TEST);

	general_shader->use();
	// glUseProgram(shader_program);

	shape* rect = new rectangle(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	shape* light_source = new rectangle(glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(1.0f, 1.0f, 1.0f));
	light_source->translate(glm::vec3(1.6f, 1.6f, 1.6f), true);


	const auto view_mat = setup_view();
	const auto proj_mat = setup_proj();

	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(vertex_buffer);
		const auto rotation = rotate(view_mat, float(glfwGetTime()) * glm::radians(-20.0f), glm::vec3(0.0f, 0.0f, 1.0f));

		general_shader->use();
		general_shader->feed_mat("view", rotation);
		general_shader->feed_mat("projection", proj_mat);
		glUniform3fv(glGetUniformLocation(general_shader->get_id(), "light_color"), 1, value_ptr(glm::vec3(1.1f, 1.1f, 1.1f)));

		rect->draw(general_shader);

		axis_shader->use();
		axis_shader->feed_mat("view", rotation);
		axis_shader->feed_mat("projection", proj_mat);
		axis_shader->feed_mat("model", glm::mat4(1.0f));
		draw_coordinate_system();

		glBindVertexArray(light_vertex_buffer);
		lighting_shader->use();
		general_shader->feed_mat("view", rotation);
		general_shader->feed_mat("projection", proj_mat);
		light_source->draw(lighting_shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete rect;
	delete light_source;
	delete general_shader; 
	delete lighting_shader;
	delete axis_shader;

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}



//glm::mat4 trans = glm::mat4(1.0f);
//trans = rotate(trans, float(glfwGetTime()) * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f));
//trans = scale(trans, glm::vec3(glfwGetTime() * 0.2f, glfwGetTime() *  0.2f, glfwGetTime() * 0.2f));
//glUseProgram(shader_program);
//
//const unsigned int transform_loc = glGetUniformLocation(shader_program, "transform");
//glUniformMatrix4fv(transform_loc, 1, GL_FALSE, value_ptr(trans));
