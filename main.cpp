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

float currentFrame;
float lastFrame;
float deltaTime;

float lastX;
float lastY;
float firstMouse = true;

Camera mainCamera(glm::vec3(0.0f, 0.0f, 0.0f), 1.5f);

GLFWwindow *window;

int initWindow();
int gladInit();
void processInputs();

void frame_buffer_size_callback(GLFWwindow *, int, int);
void cursor_position_callback(GLFWwindow *, double, double);

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

	lastFrame = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = glfwGetTime();

		processInputs();

		glClearColor(skyR, skyG, skyB, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view = mainCamera.getViewMatrix();
		shader.setMat4("view", view);

		glBindVertexArray(VAO);
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
	gladInit();

	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	return 0;
}

int gladInit()
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

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	mainCamera.rotateMouseToAngles(xoffset, yoffset);

	lastX = xpos;
	lastY = ypos;
}

void processInputs()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		mainCamera.moveForward(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		mainCamera.moveRight(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		mainCamera.moveLeft(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		mainCamera.moveBackward(deltaTime);
	}
}