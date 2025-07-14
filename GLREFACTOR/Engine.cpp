#include <iostream>
#include "Engine.h"
#include "Renderer/Renderer.h"
#include "Core/BackEnd.h"
#include "Core/Scene.h"
#include "Core/Physics.h"

namespace Engine {
	void run() {
		BackEnd::Init();

		while (BackEnd::WindowIsOpen()) {
			BackEnd::StartFrame();

			Scene::playerOne.Update();
			Physics::Update();
			Renderer::RenderFrame();

			BackEnd::EndFrame();
		}

		BackEnd::CleanUp();
	};
}