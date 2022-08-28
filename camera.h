#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

class Camera
{
private:
	glm::vec3 cameraPosition;
	glm::vec3 cameraDirection;
	glm::vec3 worldUp;

	float cameraSpeed;
	float cameraSens;

	float yaw = -90.0f;
	float pitch = 0.0f;

public:
	Camera(glm::vec3 startPosition, float cameraSpeed, float cameraSens = 0.1f)
	{
		this->cameraSpeed = cameraSpeed;
		this->cameraSens = cameraSens;

		cameraPosition = startPosition;
		cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f); // assuming camera starts off looking straight
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	void moveForward(float deltaTime)
	{
		cameraPosition += cameraSpeed * cameraDirection * deltaTime;
	}

	void moveRight(float deltaTime)
	{
		glm::vec3 localRight = glm::cross(cameraDirection, worldUp);
		cameraPosition += localRight * cameraSpeed * deltaTime;
	}

	void moveLeft(float deltaTime)
	{
		glm::vec3 localRight = glm::cross(cameraDirection, worldUp);
		cameraPosition -= localRight * cameraSpeed * deltaTime;
	}

	void moveBackward(float deltaTime)
	{
		cameraPosition -= cameraSpeed * cameraDirection * deltaTime;
	}

	void rotateMouseToAngles(float xoffset, float yoffset)
	{
		yaw += xoffset * cameraSens;
		pitch += yoffset * cameraSens;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraDirection = glm::normalize(direction);
	}
	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(cameraPosition, cameraPosition + cameraDirection, worldUp);
	}

	glm::vec3 getWorldPosition()
	{
		return cameraPosition;
	}
};

#endif