#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "stb_image.h"
#include "camera.h"
#include "fastnoise.h"

const int DEFAULT_WIDTH = 1920;
const int DEFAULT_HEIGHT = 1080;

// struct later
const float skyR = 0.0f;
const float skyG = 135.0f / 255.0f;
const float skyB = 1.0f;

GLFWwindow *window;

int initWindow();
int loadGLAD();
void processInputs();

void frame_buffer_size_callback(GLFWwindow *, int, int);

int main()
{
	initWindow();

	float vertices[] = {
		-0.5, -0.5, 0.0f,
		0.5, -0.5, 0.0f,
		0.0f, 0.5f, 0.0f};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
	glEnableVertexAttribArray(0);

	Shader shader("shader.vs", "shader.fs");
	shader.use();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));

	glm::mat4 view = glm::mat4(1.0f);

	glm::mat4 projection = glm::perspective(45.0f, (float)DEFAULT_WIDTH / DEFAULT_HEIGHT, 0.1f, 100.0f);

	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	while (!glfwWindowShouldClose(window))
	{
		processInputs();

		glClearColor(skyR, skyG, skyB, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

int initWindow()
{
	// basic boilerplate code

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "glTerrain", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to init window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	loadGLAD();

	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

	return 0;
}

int loadGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	return 0;
}

void frame_buffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInputs()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}