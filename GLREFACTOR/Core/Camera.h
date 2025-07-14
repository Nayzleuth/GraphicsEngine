#pragma once
#include <glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Common/Enums.h"

struct Camera {
	CameraMode currentMode;

	glm::mat4 getView();
	glm::mat4 getProj();

	Camera();
	void setCamPos(glm::vec3 position);
	void moveCam();
	void mouseHandling(double x, double y);

	float cameraSpeed;
	// View
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;

	glm::vec3 cameraDir;

	glm::vec3 up;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;

	// Perspective
	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;

	// Mouse
	bool firstMouse;
	float lastX;
	float lastY;
	float yaw;
	float pitch;

	glm::mat4 view = glm::mat4(0);
	glm::mat4 projection = glm::mat4(0);
};