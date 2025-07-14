#pragma once
#include "ShaderStructs.h"

namespace Renderer {
	void Init();
	void RenderFrame();
	void RenderScreenQuad(Shader);
	void CleanUp();
}