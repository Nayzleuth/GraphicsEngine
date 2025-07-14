#pragma once
#include "Scene.h"
#include <glm.hpp>
#include <iostream>

struct RigidBodyCube {
	RigidBodyCube(Object bodyModel, float massSet) : body(bodyModel) {
		position = bodyModel.position;
		rotation = bodyModel.rotation;
		mass = massSet;

		// Update AABB
		updatedBoundsBox.min = body.mesh.meshBoundingBox.min * body.scale + position;
		updatedBoundsBox.max = body.mesh.meshBoundingBox.max * body.scale + position;

		// Visual Debugger
		Scene::g_DebugDynamicCubes.emplace_back(DynamicRecPrism(position, glm::vec3(1)));
		debugIndex = Scene::g_DebugDynamicCubes.size() - 1;

		// Update OBB
		glm::mat4 modelMatrix = GetModelMatrix();

		orientedBoundsBox.axes[0] = glm::normalize(glm::vec3(modelMatrix * glm::vec4(1, 0, 0, 0)));
		orientedBoundsBox.axes[1] = glm::normalize(glm::vec3(modelMatrix * glm::vec4(0, 1, 0, 0)));
		orientedBoundsBox.axes[2] = glm::normalize(glm::vec3(modelMatrix * glm::vec4(0, 0, 1, 0)));

		glm::vec3 meshHalfExtents = 0.5f * (body.mesh.meshBoundingBox.max - body.mesh.meshBoundingBox.min);
		orientedBoundsBox.halfExtents = glm::vec3(glm::abs(modelMatrix * glm::vec4(meshHalfExtents, 0)));

		orientedBoundsBox.position = position;
	}

	void CalculateAABB() {
		glm::vec3 min(FLT_MAX);
		glm::vec3 max(-FLT_MAX);

		for (const glm::vec3& vertex : body.mesh.meshVertices) {
			// Apply scale, then rotate, then translate
			glm::vec3 worldVertex = position + rotation * (vertex * body.scale);

			min = glm::min(min, worldVertex);
			max = glm::max(max, worldVertex);
		}

		updatedBoundsBox.min = min;
		updatedBoundsBox.max = max;
	} 
	
	void moveObject(glm::vec3 direction) {
		position += direction;
		orientedBoundsBox.position = position;

		body.model = GetModelMatrix();
		Scene::g_DebugDynamicCubes[debugIndex].updateFromOBB(orientedBoundsBox);

		updatedBoundsBox.min = body.mesh.meshBoundingBox.min + position;
		updatedBoundsBox.max = body.mesh.meshBoundingBox.max + position;
	}

	void rotateObject(glm::quat rot) {
		rotation = glm::normalize(rot * rotation);
		orientedBoundsBox.axes[0] = rotation * glm::vec3(1, 0, 0);
		orientedBoundsBox.axes[1] = rotation * glm::vec3(0, 1, 0);
		orientedBoundsBox.axes[2] = rotation * glm::vec3(0, 0, 1);

		body.model = GetModelMatrix();

		CalculateAABB();

		glm::mat3 basis = glm::mat3(
			orientedBoundsBox.axes[0],  
			orientedBoundsBox.axes[1],  
			orientedBoundsBox.axes[2]   
		);
		
		glm::quat rotationQuat = glm::quat_cast(basis);

		Scene::g_DebugDynamicCubes[debugIndex].updateFromOBB(orientedBoundsBox);
		Scene::g_DebugDynamicCubes[debugIndex].updateModel();
	}

	glm::mat4 GetModelMatrix() const {
		glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
		glm::mat4 R = glm::mat4_cast(rotation);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), body.scale);
		return T * R * S;
	}

	AABB updatedBoundsBox;
	OBB orientedBoundsBox;
	int debugIndex = -1;
	Object body;

	float mass = 1.0f;
	float inverseMass = 1.0f / mass;
	float restitution = 0; // bounciness, between 0 and 1

	bool isColliding = false;
	bool gravityOn = true;

	glm::vec3 position = glm::vec3(0);
	glm::vec3 velocity = glm::vec3(0);
	glm::quat rotation = glm::quat(glm::vec3(0));
};
	  
struct StaticBodyCube {
	StaticBodyCube(Object bodyModel) : body(bodyModel) {
		position = bodyModel.position;
		rotation = bodyModel.rotation; // Capture initial rotation

		Scene::g_DebugDynamicCubes.emplace_back(DynamicRecPrism(position, glm::vec3(1)));
		debugIndex = Scene::g_DebugDynamicCubes.size() - 1;

		glm::mat4 modelMatrix = GetModelMatrix();

		orientedBoundsBox.axes[0] = glm::normalize(glm::vec3(modelMatrix * glm::vec4(1, 0, 0, 0)));
		orientedBoundsBox.axes[1] = glm::normalize(glm::vec3(modelMatrix * glm::vec4(0, 1, 0, 0)));
		orientedBoundsBox.axes[2] = glm::normalize(glm::vec3(modelMatrix * glm::vec4(0, 0, 1, 0)));

		glm::vec3 meshHalfExtents = 0.5f * (body.mesh.meshBoundingBox.max - body.mesh.meshBoundingBox.min);
		orientedBoundsBox.halfExtents = glm::vec3(glm::abs(modelMatrix * glm::vec4(meshHalfExtents, 0)));

		Scene::g_DebugDynamicCubes[debugIndex].model = modelMatrix;

		orientedBoundsBox.position = position;
		CalculateAABB(); // Implement similar to RigidBodyCube
	}

	glm::mat4 GetModelMatrix() const {
		glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
		glm::mat4 R = glm::mat4_cast(rotation);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), body.scale);
		return T * R * S;
	}

	void CalculateAABB() {
		glm::vec3 min(FLT_MAX);
		glm::vec3 max(-FLT_MAX);

		for (const glm::vec3& vertex : body.mesh.meshVertices) {
			// Apply scale, then rotate, then translate
			glm::vec3 worldVertex = position + rotation * (vertex * body.scale);

			min = glm::min(min, worldVertex);
			max = glm::max(max, worldVertex);
		}

		updatedBoundsBox.min = min;
		updatedBoundsBox.max = max;
	}

	AABB updatedBoundsBox;
	OBB orientedBoundsBox;

	Object body;

	int debugIndex = -1;

	glm::vec3 position = glm::vec3(0);
	glm::quat rotation = glm::quat(glm::vec3(0));
};

namespace Physics { 
	extern std::vector<StaticBodyCube> g_StaticCubes;
	extern std::vector<RigidBodyCube> g_RigidCubes;

	void Init();
	void Update();
	void UpdateRigidBodies();
	bool AABBCheck(AABB a, AABB b);
	bool OBBCheck(OBB a, OBB b);
	void projectOBB(const OBB& obb, const glm::vec3& axis, float& min, float& max);
	bool overlapsOnAxis(const OBB& a, const OBB& b, const glm::vec3& axis);
	void ImpulseCollisionResolver(RigidBodyCube& a, RigidBodyCube& b);
	void ImpulseCollisionResolver(RigidBodyCube& a, StaticBodyCube& b);
}
