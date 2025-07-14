#pragma once
#include "KHR/khrplatform.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../Common/Enums.h"
#include "Camera.h"

namespace BackEnd {
	void Init();
	void CleanUp();

	bool WindowIsOpen();
	void StartFrame();
	void EndFrame();

	void SetCurrentCamera(Camera& refCam);

	void framebuffer_size_callback(GLFWwindow* _window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);

	extern int scrWidth;
	extern int scrHeight;
	extern float deltaTime;
	extern Camera* currentCam;
}