#include "Input.h"
#include <iostream>

namespace Input {
    glm::vec3 InputVec = glm::vec3(0);

    void ProcessInput(GLFWwindow* _window) {
		if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(_window, true);
		}

        InputVec = glm::vec3(0);

        // Cam Movement
        // Needs to become player movement
        if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
            InputVec.z -= 1;
        if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
            InputVec.z += 1;
        if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
            InputVec.x -= 1;
        if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
            InputVec.x += 1;
        if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS)
            InputVec.y += 1;
        if (glfwGetKey(_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            InputVec.y -= 1;
        
        if (glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            BackEnd::currentCam->cameraSpeed = 10;
        else BackEnd::currentCam->cameraSpeed = 5;
	}

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_T && action == GLFW_PRESS) {
            if (BackEnd::currentCam->currentMode == CameraMode::FREE)
                BackEnd::currentCam->currentMode = CameraMode::PLAYER;
            else if (BackEnd::currentCam->currentMode == CameraMode::PLAYER)
                BackEnd::currentCam->currentMode = CameraMode::FREE;
        }
    }
}