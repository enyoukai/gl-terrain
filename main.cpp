#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>

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

Camera mainCamera(glm::vec3(0.0f, 0.0f, 3.0f), 15.0f);
FastNoiseLite noise;

GLFWwindow *window;

int initWindow();
int gladInit();
void processInputs();
void render();

void frame_buffer_size_callback(GLFWwindow *, int, int);
void cursor_position_callback(GLFWwindow *, double, double);

int main()
{
	initWindow();
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

	Shader shader("shader.vs", "shader.fs");
	shader.use();

	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 view = glm::mat4(1.0f);

	glm::mat4 projection = glm::perspective(45.0f, (float)DEFAULT_WIDTH / DEFAULT_HEIGHT, 0.1f, 200.0f);

	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	lastFrame = glfwGetTime();

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

		render();

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

void render()
{
	unsigned int CHUNK_WIDTH = 100;
	unsigned int CHUNK_HEIGHT = 100;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	// generate vertices
	for (int i = 0; i < CHUNK_WIDTH; i++)
	{
		for (int j = 0; j < CHUNK_HEIGHT; j++)
		{
			vertices.push_back((float)i);

			// octave method later
			float height = 64 * noise.GetNoise(1 * (float)i, 1 * (float)j) +
						   32 * noise.GetNoise(2 * (float)i, 2 * (float)j) +
						   16 * noise.GetNoise(4 * (float)i, 4 * (float)j) +
						   8 * noise.GetNoise(8 * (float)i, 8 * (float)j) +
						   4 * noise.GetNoise(16 * (float)i, 16 * (float)j) +
						   2 * noise.GetNoise(32 * (float)i, 32 * (float)j);

			vertices.push_back(height);
			vertices.push_back((float)j);
		}
	}

	// generate indices
	for (int i = 0; i < CHUNK_WIDTH - 1; i++)
	{
		for (int j = 0; j < CHUNK_HEIGHT - 1; j++)
		{
			indices.push_back(CHUNK_WIDTH * i + j);
			indices.push_back(CHUNK_WIDTH * i + j + 1);
			indices.push_back(CHUNK_WIDTH * (i + 1) + j);

			indices.push_back(CHUNK_WIDTH * (i + 1) + j);
			indices.push_back(CHUNK_WIDTH * i + j + 1);
			indices.push_back(CHUNK_WIDTH * (i + 1) + j + 1);
		}
	}

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
	glEnableVertexAttribArray(0);

	glDrawElements(GL_TRIANGLES, (CHUNK_WIDTH - 1) * (CHUNK_HEIGHT - 1) * 6, GL_UNSIGNED_INT, 0);
	// glDrawArrays(GL_TRIANGLES, 0, 3);
}