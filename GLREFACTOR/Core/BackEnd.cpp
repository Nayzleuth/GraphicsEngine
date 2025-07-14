#include <iostream>
#include "BackEnd.h"
#include "AssetManager.h"
#include "Scene.h"
#include "Physics.h"
#include "../Renderer/Renderer.h"
#include "../Input/Input.h"

namespace BackEnd {
	GLFWwindow* _window = NULL;
	Camera* currentCam = nullptr;

	float deltaTime = 0.0f;
	float lastFrameTime = 0.0f;
	int scrWidth = 1920;
	int scrHeight = 1080;
	void Init() {
		if (!glfwInit()) {
			std::cout << "GLFW failed to initialize.";
		}
		_window = glfwCreateWindow(scrWidth, scrHeight, "Engine", NULL, NULL);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwMakeContextCurrent(_window);
		
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
		}

		glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
		glfwSetKeyCallback(_window, Input::key_callback);

		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(_window, mouse_callback);

		glEnable(GL_DEPTH_TEST);

		// Following Inits
		Renderer::Init();
		AssetManager::Init();
		ShaderManager::Init();
		Scene::Init();
		Physics::Init();
	}
	
	void CleanUp() {
		AssetManager::CleanUp();
		Renderer::CleanUp();
		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	bool WindowIsOpen() {
		return !glfwWindowShouldClose(_window);
	}

	void StartFrame() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// DeltaTime
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		glfwPollEvents();
		Input::ProcessInput(_window);
	}

	void EndFrame() {
		glfwSwapBuffers(_window);
	}

	void framebuffer_size_callback(GLFWwindow* _window, int width, int height){
		glViewport(0, 0, width, height);
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
		currentCam->mouseHandling(xpos, ypos);
	}

	void SetCurrentCamera(Camera& refCam) {
		std::cout << "NEW CAMERA SET\n";
		currentCam = &refCam;
	};
}

