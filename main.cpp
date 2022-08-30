#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "fastnoise.h"

const int DEFAULT_WIDTH = 1920;
const int DEFAULT_HEIGHT = 1080;
const float RENDER_DISTANCE = 100.0f;
const float FAR_PLANE = 1000.0f;

const float DIFFUSE_EPSILON = 0.01f;

const float OCTAVES = 6;
const float NOISE_SCALE = 64;

const float CAMERA_SPEED_DEFAULT = 15.0f;
const float CAMERA_SPEED_FAST = 150.0f;

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

Camera mainCamera(glm::vec3(0.0f, 10.0f, 3.0f), CAMERA_SPEED_DEFAULT);
FastNoiseLite noise;
Shader shader("shader.vs", "shader.fs");

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
	noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
	noise.SetFractalOctaves(OCTAVES);

	shader.compile();
	shader.use();

	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 view = glm::mat4(1.0f);

	glm::mat4 projection = glm::perspective(45.0f, (float)DEFAULT_WIDTH / DEFAULT_HEIGHT, 0.1f, FAR_PLANE);

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

		glm::vec3 cameraPosition = mainCamera.getWorldPosition();
		shader.setVec3("lightPosition", cameraPosition);

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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		mainCamera.moveUp(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		mainCamera.moveDown(deltaTime);
	}

	// TODO: callback later

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		shader.setBool("showNormals", true);
	}
	else
	{
		shader.setBool("showNormals", false);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		mainCamera.setSpeed(CAMERA_SPEED_FAST);
	}
	else
	{
		mainCamera.setSpeed(CAMERA_SPEED_DEFAULT);
	}
}

void render()
{
	// TODO: fix heightmap changing weirdly when camera moves?
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<float> normals;

	// generate vertices
	for (int i = 0; i < RENDER_DISTANCE; i++)
	{
		for (int j = 0; j < RENDER_DISTANCE; j++)
		{
			float worldX = i - (int)RENDER_DISTANCE / 2 + (int)mainCamera.getWorldPosition().x;
			float worldZ = j - (int)RENDER_DISTANCE / 2 + (int)mainCamera.getWorldPosition().z;

			// float worldX = (float)i;
			// float worldZ = (float)j;
			vertices.push_back(worldX);

			float height = NOISE_SCALE * noise.GetNoise(worldX, worldZ);

			vertices.push_back(height);
			vertices.push_back(worldZ);
		}
	}

	// generate indices
	for (int i = 0; i < RENDER_DISTANCE - 1; i++)
	{
		for (int j = 0; j < RENDER_DISTANCE - 1; j++)
		{
			indices.push_back(RENDER_DISTANCE * i + j);
			indices.push_back(RENDER_DISTANCE * i + j + 1);
			indices.push_back(RENDER_DISTANCE * (i + 1) + j);

			indices.push_back(RENDER_DISTANCE * (i + 1) + j);
			indices.push_back(RENDER_DISTANCE * i + j + 1);
			indices.push_back(RENDER_DISTANCE * (i + 1) + j + 1);
		}
	}

	// generate normals
	for (int i = 0; i < RENDER_DISTANCE; i++)
	{
		for (int j = 0; j < RENDER_DISTANCE; j++)
		{
			// method later
			float worldX = i - (int)RENDER_DISTANCE / 2 + (int)mainCamera.getWorldPosition().x;
			float worldZ = j - (int)RENDER_DISTANCE / 2 + (int)mainCamera.getWorldPosition().z;

			// float worldX = (float)i;
			// float worldZ = (float)j;

			glm::vec3 xTangent = glm::vec3(-DIFFUSE_EPSILON, NOISE_SCALE * noise.GetNoise(worldX - DIFFUSE_EPSILON, worldZ), 0) - glm::vec3(DIFFUSE_EPSILON, NOISE_SCALE * noise.GetNoise(worldX + DIFFUSE_EPSILON, worldZ), 0);
			glm::vec3 zTangent = glm::vec3(0, NOISE_SCALE * noise.GetNoise(worldX, worldZ - DIFFUSE_EPSILON), -DIFFUSE_EPSILON) - glm::vec3(0, NOISE_SCALE * noise.GetNoise(worldX, worldZ + DIFFUSE_EPSILON), DIFFUSE_EPSILON);

			glm::vec3 norm = glm::cross(zTangent, xTangent);

			normals.push_back(norm.x);
			normals.push_back(norm.y);
			normals.push_back(norm.z);
		}
	}

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO[2];
	glGenBuffers(2, VBO);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	// position
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
	glEnableVertexAttribArray(0);

	// normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLES, (RENDER_DISTANCE - 1) * (RENDER_DISTANCE - 1) * 6, GL_UNSIGNED_INT, 0);

	glDeleteBuffers(2, VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}