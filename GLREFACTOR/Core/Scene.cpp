#include "Scene.h"
#include "Camera.h"
#include "../glad/glad.h"
#include "ShaderManager.h"

void Object::render() {
	shader.use();
	mesh.useVAO();

	if (texture != -1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	
	if (material.color.x != -1) {
		shader.updateVec3("color", material.color);
	}

	if (model == glm::mat4(1.0f)) {
		setPos(position);
		rotateObj(rotation.x, glm::vec3(1, 0, 0));
		rotateObj(rotation.y, glm::vec3(0, 1, 0));
		rotateObj(rotation.z, glm::vec3(0, 0, 1));
		scaleObj(scale);
	}

	shader.updateMat4("model", model);
	shader.updateMat4("view", BackEnd::currentCam->getView());
	shader.updateMat4("projection", BackEnd::currentCam->getProj());

	glDrawElements(GL_TRIANGLES, mesh.getIndexCount(), GL_UNSIGNED_INT, 0);
};

void Object::renderShaderOverride(Shader newShader) {
	newShader.use();
	mesh.useVAO();

	if (texture != -1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	if (material.color.x != -1) {
		newShader.updateVec3("color", material.color);
	}

	if (model == glm::mat4(1.0f)) {
		setPos(position);
		rotateObj(rotation.x, glm::vec3(1, 0, 0));
		rotateObj(rotation.y, glm::vec3(0, 1, 0));
		rotateObj(rotation.z, glm::vec3(0, 0, 1));
		scaleObj(scale);
	}

	newShader.updateMat4("model", model);
	newShader.updateMat4("view", BackEnd::currentCam->getView());
	newShader.updateMat4("projection", BackEnd::currentCam->getProj());

	glDrawElements(GL_TRIANGLES, mesh.getIndexCount(), GL_UNSIGNED_INT, 0);
};

void Object::setPos(glm::vec3 newPosition) {
	position = newPosition;
	model = glm::mat4(1.0f);
	model = glm::translate(model, position);
};

void Object::rotateObj(float angle, glm::vec3 axis) {
	model = glm::rotate(model, glm::radians(angle), axis);
}; 

void Object::scaleObj(glm::vec3 scaleFactors) {
	model = glm::scale(model, scaleFactors);
};

namespace Scene {
	std::vector<Object> g_ObjectsInScene;
	std::vector<Point> g_DebugPoints;
	std::vector<Line> g_DebugStripLines;
	std::vector<Line> g_DebugLoopLines;
	std::vector<Square> g_DebugSquares;
	std::vector<RectangularPrism> g_DebugCubes;
	std::vector<DynamicRecPrism> g_DebugDynamicCubes;
	CharacterController playerOne;

	void ScatterObjs() {
		for (int i = 0; i < 50; i++) {
			Object newCube(ObjPosData(Util::randomVec3(-15, 15), Util::randomVec3(0, 1000), glm::vec3(0.75)), "thinfish.obj", ShaderManager::g_Shaders.Basic);
			g_ObjectsInScene.push_back(newCube);
		}

		for (int i = 0; i < 50; i++) {
			Object newCube(ObjPosData(Util::randomVec3(-15, 15), Util::randomVec3(0, 1000), glm::vec3(0.75)), "cube.obj", ShaderManager::g_Shaders.Basic);
			g_ObjectsInScene.push_back(newCube);
		}
	}

	void Init() {
		//ScatterObjs();

		for (Light& newLight : AssetManager::lights) {
			Object lightModel(ObjPosData(glm::vec3(newLight.position), Util::randomVec3(0, 25), glm::vec3(0.1)), "cube.obj", ShaderManager::g_Shaders.Matte);
			lightModel.material.color = newLight.color;
			g_ObjectsInScene.push_back(lightModel);
		}

		Object fish(ObjPosData(glm::vec3(0, 0, -3), glm::vec3(0), glm::vec3(0.75)), "thinfish.obj", ShaderManager::g_Shaders.Basic);
		g_ObjectsInScene.push_back(fish);

		Object cubeTwo(ObjPosData(glm::vec3(-2, -2, 0), glm::vec3(45, 120, 0), glm::vec3(0.75)), "thinfish.obj", ShaderManager::g_Shaders.Basic);
		g_ObjectsInScene.push_back(cubeTwo);

		g_DebugPoints.emplace_back(glm::vec3(1, 23, 0), glm::vec3(1, 0, 0));
		g_DebugPoints.emplace_back(glm::vec3(0, 24, 0), glm::vec3(0, 1, 0));
		g_DebugPoints.emplace_back(glm::vec3(0, 23, 1), glm::vec3(0, 0, 1));
		g_DebugPoints.emplace_back(glm::vec3(-1,23, 0), glm::vec3(1, 1, 0));
		g_DebugPoints.emplace_back(glm::vec3(0, 22, 0), glm::vec3(1, 0, 1));
		g_DebugPoints.emplace_back(glm::vec3(0, 23, -1), glm::vec3(0, 1, 1));
		g_DebugPoints.emplace_back(glm::vec3(0, 23, 0), glm::vec3(1, 1, 1));

		AssetManager::genVAO("DebugPoints");
		AssetManager::VAOFillBuffer(Scene::g_DebugPoints);
		AssetManager::VAOSetAttrib();
		std::cout << "VAO DebugPoints: " << AssetManager::g_VAOs.at("DebugPoints").VAO << "\n";

		Line newLine;
		newLine.AddPointFromVec3(glm::vec3(3, 23, 3));
		newLine.AddPointFromVec3(glm::vec3(-3, 23, 3));
		newLine.SetLineColor(glm::vec3(1, 0, 0));
		newLine.LineSetup();
		g_DebugStripLines.emplace_back(newLine); 
		
		Line multiPointLine;
		multiPointLine.AddPointFromVec3(glm::vec3(3, 23, -3));
		multiPointLine.AddPointFromVec3(glm::vec3(-3, 23, -3));
		multiPointLine.AddPointFromVec3(glm::vec3(-3, 24, -3));
		multiPointLine.SetLineColor(glm::vec3(0, 1, 0));
		multiPointLine.LineSetup();
		g_DebugStripLines.emplace_back(multiPointLine);	   
		
		Line multiPointLineLoop;
		multiPointLineLoop.AddPointFromVec3(glm::vec3(3, 23, -4));
		multiPointLineLoop.AddPointFromVec3(glm::vec3(-3, 23, -4));
		multiPointLineLoop.AddPointFromVec3(glm::vec3(-3, 24, -4));
		multiPointLineLoop.SetLineColor(glm::vec3(0, 0, 1));
		multiPointLineLoop.linePoints[2].color = glm::vec3(1, 0, 0);
		multiPointLineLoop.LineSetup();
		g_DebugLoopLines.emplace_back(multiPointLineLoop);

		Square firstSquare(glm::vec3(0, 23, -5), 1, glm::vec3(0));
		firstSquare.SetWholeColor(glm::vec3(1, 0, 0));
		firstSquare.Setup();
		g_DebugSquares.emplace_back(firstSquare);

		Square secondSquare(glm::vec3(0, 23, -6), 2, glm::vec3(0));
		secondSquare.SetWholeColor(glm::vec3(0, 1, 0));
		secondSquare.Setup();
		g_DebugSquares.emplace_back(secondSquare);

		Square thirdSquare(glm::vec3(0, 23, -7), 2, glm::vec3(15));
		thirdSquare.SetWholeColor(glm::vec3(0, 0, 1));
		thirdSquare.SetCornerColor(glm::vec3(1, 0, 0), "BL");
		thirdSquare.SetCornerColor(glm::vec3(0, 1, 0), "TR");
		thirdSquare.Setup();
		g_DebugSquares.emplace_back(thirdSquare);

		RectangularPrism firstCube(glm::vec3(0, 23, -9), glm::vec3(1.0f), glm::vec3(0));
		firstCube.SetWholeCubeColor(glm::vec3(1, 0, 0));
		g_DebugCubes.emplace_back(firstCube);

		RectangularPrism secondCube(glm::vec3(0, 23, -13), glm::vec3(1.5f), glm::vec3(0));
		secondCube.SetWholeCubeColor(glm::vec3(0, 1, 0));
		g_DebugCubes.emplace_back(secondCube);
		
		RectangularPrism thirdCube(glm::vec3(0, 23, -17), glm::vec3(1.5f), glm::vec3(15));
		thirdCube.SetWholeCubeColor(glm::vec3(0, 0, 1));
		thirdCube.SetFaceColor(RectangularPrism::TOP, glm::vec3(1, 0, 0));
		thirdCube.SetFaceColor(RectangularPrism::FRONT, glm::vec3(0, 1, 0));
		g_DebugCubes.emplace_back(thirdCube);	
		
		RectangularPrism firstWallAttempt(glm::vec3(0, -15.25, -5), glm::vec3(3), glm::vec3(0));
		firstWallAttempt.SetWholeCubeColor(glm::vec3(1));
		g_DebugCubes.emplace_back(firstWallAttempt);
	};
}