#include <iostream>
#include "Controllers.h"

CharacterController::CharacterController() : playerCam() {
	BackEnd::SetCurrentCamera(playerCam);
};

void CharacterController::Update() {
	if (playerCam.currentMode == CameraMode::FREE) {
		playerCam.moveCam();
		return;
	}

	verticalVelocity -= gravity * BackEnd::deltaTime;

	if (position.y <= -15) {
		verticalVelocity = 0;
		position.y = -15;
	}

	PhysMovement();
	playerCam.setCamPos(position);
};

void CharacterController::PhysMovement() {
	glm::vec3 playerDirectional = glm::vec3(Input::InputVec.x, 0, Input::InputVec.z);
	if(glm::length(playerDirectional) > 0.0f) playerDirectional = glm::normalize(playerDirectional);

	glm::vec3 front = glm::normalize(glm::vec3(playerCam.cameraFront.x, 0, playerCam.cameraFront.z));
	glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));

	glm::vec3 movementVec = -front * playerDirectional.z + right * playerDirectional.x;
	
	position += movementVec * movementSpeed * BackEnd::deltaTime;

	if (Input::InputVec.y > 0 && verticalVelocity == 0) {
		verticalVelocity = jumpStr;
	}

	position.y += verticalVelocity * BackEnd::deltaTime;
}