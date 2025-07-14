#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Core/BackEnd.h"

namespace Input {
	extern glm::vec3 InputVec;
	
	void ProcessInput(GLFWwindow* _window);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
}
