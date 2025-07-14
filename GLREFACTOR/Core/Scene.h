#pragma once
#include "AssetManager.h" 
#include "ShaderManager.h" 
#include "../Renderer/ShaderStructs.h" 
#include "../Renderer/Renderer.h" 
#include "../Common/Util.h" 
#include "../Core/Controllers.h" 
#include <glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp> 
#include <iostream> 
#include <vector>
#include <string>


struct ObjPosData {
	ObjPosData() {};
	ObjPosData(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl)
		: position(pos),
		rotation(rot),
		scale(scl) {};
	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	glm::vec3 scale = glm::vec3(1);
};

struct Material {
	glm::vec3 color = glm::vec3(-1);
};

struct Object {
	Object(ObjPosData posData, std::string meshKey, Shader& chosenShader) :
		shader(chosenShader),
		mesh(AssetManager::g_MeshCollection.at(meshKey)),
		texture(-1),
		position(posData.position),
		rotation(posData.rotation),
		scale(posData.scale) {};

	Object(ObjPosData posData, std::string meshKey, Shader& chosenShader, std::string texKey) :
		shader(chosenShader),
		mesh(AssetManager::g_MeshCollection.at(meshKey)),
		texture(AssetManager::g_TextureCollection.at(texKey)),
		position(posData.position),
		rotation(posData.rotation),
		scale(posData.scale) {};

	void render();
	void renderShaderOverride(Shader newShader);
	void setPos(glm::vec3 newPosition);
	void rotateObj(float angle, glm::vec3 axis);
	void scaleObj(glm::vec3 scaleFactors);

	Shader& shader;
	Mesh& mesh;
	int texture;
	Material material;
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

namespace Scene {
	extern std::vector<Object> g_ObjectsInScene;
	extern std::vector<Point> g_DebugPoints;
	extern std::vector<Line> g_DebugStripLines;
	extern std::vector<Line> g_DebugLoopLines;
	extern std::vector<Square> g_DebugSquares;
	extern std::vector<RectangularPrism> g_DebugCubes;
	extern std::vector<DynamicRecPrism> g_DebugDynamicCubes;
	extern CharacterController playerOne;

	void Init();
}

