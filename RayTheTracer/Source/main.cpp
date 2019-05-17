#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm/glm.hpp>
#include <glm/glm/ext.hpp>
#include <shaders.h>
#include <cstdlib>
#include <cstdio>

const unsigned int scr_width = 800;
const unsigned int scr_height = 600;

struct point
{
	float x;
	float y;
	float z;

	point(const float x, const float y, const float z) : x(x), y(y), z(z) {}
};

point vertices[] = {
	point(0.5f, -0.5f, 0.0f), point(1.0f, 0.0f, 0.0f),
	point(-0.5f, -0.5f, 0.0f), point(1.0f, 0.0f, 0.0f),
	point(0.0f, 0.5f, 0.0f), point(1.0f, 0.0f, 0.0f),

	point(0.5f, -0.5f, 0.0f), point(0.0f, 1.0f, 0.0f),
	point(-0.5f, -0.5f, 0.0f), point(0.0f, 1.0f, 0.0f),
	point(0.0f, 0.0f, 0.5f), point(0.0f, 1.0f, 0.0f),

	point(-0.5f, -0.5f, 0.0f), point(0.0f, 0.0f, 1.0f),
	point(0.0f, 0.5f, 0.0f), point(0.0f, 0.0f, 1.0f),
	point(0.0f, 0.0f, 0.5f), point(0.0f, 0.0f, 1.0f),

	point(0.5f, -0.5f, 0.0f), point(1.0f, 0.0f, 1.0f),
	point(0.0f, 0.5f, 0.0f), point(1.0f, 0.0f, 1.0f),
	point(0.0f, 0.0f, 0.5f), point(1.0f, 0.0f, 1.0f),
};


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

	const auto shader_program = setup_shaders();

	unsigned int vertex_buffer, vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &vertex_buffer);

	glBindVertexArray(vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(shader_program);

	while (!glfwWindowShouldClose(window))
	{
		// render
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);

		auto model = glm::mat4(1.0f);
		model = rotate(model, float(glfwGetTime()) * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

		auto view = glm::mat4(1.0f);
		// note that we're translating the scene in the reverse direction of where we want to move
		view = translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		auto projection = glm::perspective(glm::radians(45.0f), float(scr_width) / float(scr_height), 0.1f, 100.0f);

		const auto model_loc = glGetUniformLocation(shader_program, "model");
		const auto view_loc = glGetUniformLocation(shader_program, "view");
		const auto projection_loc = glGetUniformLocation(shader_program, "projection"); 
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, value_ptr(model));
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection));

		glBindVertexArray(vertex_array);
		glDrawArrays(GL_TRIANGLES, 0, 12);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
