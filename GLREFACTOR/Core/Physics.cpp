#include "Physics.h"

struct Contact {
	glm::vec3 point;       // Contact point
	glm::vec3 normal;      // Collision normal (points toward A)
	float depth;           // Penetration depth
};

namespace Physics {
	std::vector<StaticBodyCube> g_StaticCubes;
	std::vector<RigidBodyCube> g_RigidCubes;
	
	std::vector<Contact> ContactManifold;

	void Init() {
		/*
		StaticBodyCube floor(Object(ObjPosData(glm::vec3(0, -17, 0), glm::vec3(0), glm::vec3(100, 1, 100)), "cube.obj", ShaderManager::g_Shaders.Basic));
		g_StaticCubes.emplace_back(floor);
		
		RigidBodyCube firstRigid(Object(ObjPosData(glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(1)), "cube.obj", ShaderManager::g_Shaders.Basic), 1.0f);
		firstRigid.rotateObject(glm::vec3(45, 45, 0));
		g_RigidCubes.emplace_back(firstRigid);	  
		
		StaticBodyCube secStatic(Object(ObjPosData(glm::vec3(0, -2, 0), glm::vec3(0), glm::vec3(1.25f, .1, 1.25f)), "cube.obj", ShaderManager::g_Shaders.Basic));
		g_StaticCubes.emplace_back(secStatic);
		
		RigidBodyCube secRigid(Object(ObjPosData(glm::vec3(0, 5, 0), glm::vec3(0), glm::vec3(1)), "cube.obj", ShaderManager::g_Shaders.Basic), 1.0f);
		secRigid.rotateObject(glm::vec3(45, 45, 0));

		g_RigidCubes.emplace_back(secRigid);
		*/
	};
									 
	void Update() {
		UpdateRigidBodies();
	};

	void UpdateRigidBodies() {
		// Collision Detection
		for (RigidBodyCube& nextCube : g_RigidCubes) {
			nextCube.isColliding = false;

			if (nextCube.isColliding == false && nextCube.gravityOn) {
				nextCube.velocity += (glm::vec3(0, -0.1f, 0) * BackEnd::deltaTime);
			}
			
			//Compare AABBs
			for (StaticBodyCube& staticCheck : g_StaticCubes) {
				if (AABBCheck(staticCheck.updatedBoundsBox, nextCube.updatedBoundsBox)) {
					if (OBBCheck(nextCube.orientedBoundsBox, staticCheck.orientedBoundsBox)) {
						nextCube.velocity = glm::vec3(0);
						nextCube.isColliding = true;
					}
				}
			}

			for (RigidBodyCube& otherCubes : g_RigidCubes) {
				if (&nextCube == &otherCubes) continue;
				if (AABBCheck(otherCubes.updatedBoundsBox, nextCube.updatedBoundsBox)) {
					if (OBBCheck(nextCube.orientedBoundsBox, otherCubes.orientedBoundsBox)) {
						nextCube.velocity = glm::vec3(0);
						nextCube.isColliding = true;
					}
				}
			}

			nextCube.moveObject(nextCube.velocity * BackEnd::deltaTime);
			//nextCube.rotateObject(glm::vec3(2, 2, 0) * BackEnd::deltaTime);
		}
	};
	
	bool AABBCheck(AABB a, AABB b) {
		if ((a.min.x < b.max.x && a.max.x > b.min.x) &&
			(a.min.y < b.max.y && a.max.y > b.min.y) &&
			(a.min.z < b.max.z && a.max.z > b.min.z))
			return true;
		return false;
	}

	bool OBBCheck(OBB a, OBB b) {
		for (int i = 0; i < 3; i++) {
			if (!overlapsOnAxis(a, b, a.axes[i])) {
				return false;
			}
		}	
		for (int i = 0; i < 3; i++) {
			if (!overlapsOnAxis(a, b, b.axes[i])) {
				return false;
			}
		}

		for (int i = 0; i < 3; i++) {
			if (!overlapsOnAxis(a, b, a.axes[i])) return false;
			if (!overlapsOnAxis(a, b, b.axes[i])) return false;
		}

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				glm::vec3 axis = glm::cross(a.axes[i], b.axes[j]);
				if (glm::length(axis) < 0.001f) continue;
				axis = glm::normalize(axis);
				if (!overlapsOnAxis(a, b, axis)) return false;
			}
		}

		return true; 
	};

	void projectOBB(const OBB& obb, const glm::vec3& axis, float& min, float& max) {
		min = max = glm::dot(obb.position, axis); 
		for (int i = 0; i < 8; i++) {
			
			glm::vec3 corner = obb.position;
			corner += obb.axes[0] * ((i & 1) ? -obb.halfExtents.x : obb.halfExtents.x);
			corner += obb.axes[1] * ((i & 2) ? -obb.halfExtents.y : obb.halfExtents.y);
			corner += obb.axes[2] * ((i & 4) ? -obb.halfExtents.z : obb.halfExtents.z);

			float proj = glm::dot(corner, axis);
			min = std::min(min, proj);
			max = std::max(max, proj);
		}
	}

	bool overlapsOnAxis(const OBB& a, const OBB& b, const glm::vec3& axis) {
		float aMin, aMax, bMin, bMax;
		projectOBB(a, axis, aMin, aMax);
		projectOBB(b, axis, bMin, bMax);
		return (aMax >= bMin) && (bMax >= aMin); 
	}

	void ImpulseCollisionResolver(RigidBodyCube& a,  RigidBodyCube& b) {
		glm::vec3 relativeVelocity = a.velocity - b.velocity;

		glm::vec3 delta = b.position - a.position;

		glm::vec3 ahalfSize = (a.updatedBoundsBox.max - a.updatedBoundsBox.min) * 0.5f;
		glm::vec3 bhalfSize = (b.updatedBoundsBox.max - b.updatedBoundsBox.min) * 0.5f;

		float overlapX = (ahalfSize.x + bhalfSize.x) - std::abs(delta.x);
		float overlapY = (ahalfSize.y + bhalfSize.y) - std::abs(delta.y);
		float overlapZ = (ahalfSize.z + bhalfSize.z) - std::abs(delta.z);

		if (overlapX < 0 || overlapY < 0 || overlapZ < 0)
			return; // no collision

		// Find the axis of minimum penetration
		float minOverlap = overlapX;
		glm::vec3 normal = glm::vec3((delta.x < 0) ? -1 : 1, 0, 0);

		if (overlapY < minOverlap) {
			minOverlap = overlapY;
			normal = glm::vec3(0, (delta.y < 0) ? -1 : 1, 0);
		}

		if (overlapZ < minOverlap) {
			minOverlap = overlapZ;
			normal = glm::vec3(0, 0, (delta.z < 0) ? -1 : 1);
		}

		glm::vec3 collisionNormal;

		if (minOverlap == overlapX)
			collisionNormal = glm::vec3(minOverlap, 0, 0);
		else if (minOverlap == overlapY)
			collisionNormal = glm::vec3(0, minOverlap, 0);
		else
			collisionNormal = glm::vec3(0, 0, minOverlap);

		collisionNormal = glm::normalize(collisionNormal);

		float velAlongNormal = glm::dot(relativeVelocity, collisionNormal);

		// if theyre moving away skip
		if (velAlongNormal > 0)
			return;

		float restitution = std::min(a.restitution, b.restitution);

		float impulseScalar = -(1.0f + restitution) * velAlongNormal;
		impulseScalar /= (a.inverseMass + b.inverseMass);

		glm::vec3 impulse = impulseScalar * collisionNormal;

		a.velocity += impulse * a.inverseMass;
		b.velocity -= impulse * b.inverseMass;

		float penetrationDepth = minOverlap;

		glm::vec3 correction = (penetrationDepth / (a.inverseMass + b.inverseMass)) * collisionNormal;


		a.position += correction * a.inverseMass;
		b.position -= correction * b.inverseMass;

		// Recalculate AABBs now that position changed
		a.CalculateAABB();
		b.CalculateAABB();
	} 
	
	void ImpulseCollisionResolver(RigidBodyCube& body,  StaticBodyCube& surface) {
		glm::vec3 delta = body.position - surface.position;

		glm::vec3 bodyHalfSize = (body.updatedBoundsBox.max - body.updatedBoundsBox.min) * 0.5f;
		glm::vec3 bodyPosition = (body.updatedBoundsBox.max + body.updatedBoundsBox.min) * 0.5f;  
		
		glm::vec3 surfaceHalfSize = (surface.updatedBoundsBox.max - surface.updatedBoundsBox.min) * 0.5f;
		glm::vec3 surfacePosition = (surface.updatedBoundsBox.max + surface.updatedBoundsBox.min) * 0.5f;

		glm::vec3 totalHalfSize = bodyHalfSize + surfaceHalfSize;
		glm::vec3 absDelta = glm::abs(delta);

		glm::vec3 overlap = totalHalfSize - absDelta;

		// No collision
		if (overlap.x <= 0 || overlap.y <= 0 || overlap.z <= 0)
			return;

		// Find smallest axis of penetration
		float minOverlap = overlap.x;
		glm::vec3 normal = glm::vec3((delta.x < 0) ? -1.0f : 1.0f, 0, 0);

		if (overlap.y < minOverlap) {
			minOverlap = overlap.y;
			normal = glm::vec3(0, (delta.y < 0) ? -1.0f : 1.0f, 0);
		}

		if (overlap.z < minOverlap) {
			minOverlap = overlap.z;
			normal = glm::vec3(0, 0, (delta.z < 0) ? -1.0f : 1.0f);
		}

		float velAlongNormal = glm::dot(body.velocity, normal);

		if (velAlongNormal > 0.0f)
			return;

		float e = body.restitution;

		float j = -(1.0f + e) * velAlongNormal;
		j /= body.inverseMass;

		glm::vec3 impulse = j * normal;

		body.velocity += impulse * body.inverseMass;

		const float slop = 0.01f;
		float penetrationDepth = minOverlap;
		float correctionAmount = std::max(penetrationDepth - slop, 0.0f);

		glm::vec3 correction = correctionAmount * normal;
		body.position += correction;

		body.CalculateAABB();
	}
}