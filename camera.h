#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera(glm::vec3 cameraPos)
	{
		cameraPosition = cameraPos;
		cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f); // assuming camera starts off looking straight
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	}

private:
	glm::vec3 cameraPosition;
	glm::vec3 cameraTarget;
	glm::vec3 worldUp;
};

#endif