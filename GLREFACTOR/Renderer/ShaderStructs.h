#pragma once
#include <glad/glad.h>
#include <unordered_map>
#include <glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

struct Shader {
	std::unordered_map<std::string, unsigned int> uniforms;
	void use();
	void load(std::string vertPath, std::string fragPath);
	void assignUniform(std::string);
	void updateInt(std::string, int);
	void updateMat4(std::string, glm::mat4);
	void updateVec3(std::string, glm::vec3 updateVal);
	void updateVec2(std::string, glm::vec2 updateVal);
	void updateBool(std::string, bool updateVal);
	int m_ID = -1;
};