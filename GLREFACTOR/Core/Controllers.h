#pragma once
#include <glm.hpp>
#include "../Common/Enums.h"
#include "BackEnd.h"
#include "../Input/Input.h"
#include "Camera.h"

struct CharacterController {
	glm::vec3 playerPos = glm::vec3(0);

	CharacterController();
	void Update();
	void PhysMovement();

	Camera playerCam;

	const float gravity = 9.8f;
	float verticalVelocity = 0.0f;

	float jumpStr = 5;
	float movementSpeed = 6;

	glm::vec3 position = glm::vec3(0);
};
