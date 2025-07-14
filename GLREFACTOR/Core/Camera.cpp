#include "Camera.h"
#include "BackEnd.h"
#include <iostream>
#include "../Input/Input.h"

Camera::Camera() {
	cameraSpeed = 2.5f;
	currentMode = CameraMode::FREE;

	// View
	cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

	cameraDir = glm::normalize(cameraPos - cameraFront);

	up = glm::vec3(0.0f, 1.0f, 0.0f);
	cameraRight = glm::normalize(glm::cross(up, cameraDir));
	cameraUp = glm::cross(cameraDir, cameraRight);

	// Perspective
	fov = 45.0f;
	aspectRatio = static_cast<float>(BackEnd::scrWidth) / static_cast<float>(BackEnd::scrHeight);
	nearPlane = 0.1f;
	farPlane = 1000.0f;

	// Mouse
	firstMouse = true;
	lastX = BackEnd::scrWidth / 2.0;
	lastY = BackEnd::scrHeight / 2.0;
	yaw = -90.0;
	pitch = 0.0;

	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
};

glm::mat4 Camera::getView() {
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	return view;
}

// Proj doesn't need to be recalculated every frame.
glm::mat4 Camera::getProj() {
	projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
	return projection;
} 

void Camera::setCamPos(glm::vec3 position) {
	cameraPos = position;
};

void Camera::moveCam() {
	glm::vec3 CamInputVec = Input::InputVec;
	if (glm::length(CamInputVec) > 0.0f) CamInputVec = glm::normalize(CamInputVec);

	glm::vec3 forward = glm::normalize(cameraFront);
	glm::vec3 right = glm::normalize(glm::cross(forward, cameraUp));
	glm::vec3 up = glm::normalize(glm::cross(right, forward)); 

	glm::vec3 movementVec = forward * (-CamInputVec.z) + up * CamInputVec.y + right * CamInputVec.x;

	cameraPos += movementVec * cameraSpeed * BackEnd::deltaTime;
}

void Camera::mouseHandling(double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
};