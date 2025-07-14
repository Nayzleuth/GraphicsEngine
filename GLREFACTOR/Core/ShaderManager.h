#pragma once
#include <unordered_map>
#include <string>
#include "../Renderer/ShaderStructs.h"

namespace ShaderManager {
	struct Shaders {
		Shader GeometryPass;
		Shader LightingPass;
		Shader Basic;
		Shader Debug;
		Shader Matte;
		Shader DebugPoints;
		Shader TextureQuad;
		Shader ScrQuad;
		Shader PostProcessingBlur;
		Shader DitherQuad;
		Shader PointsOnly;
	};

	extern Shaders g_Shaders;

	void Init();
}
