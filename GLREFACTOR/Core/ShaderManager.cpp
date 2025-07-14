#include "../Renderer/ShaderStructs.h"
#include "ShaderManager.h"
#include <unordered_map>
#include <string>
#include <iostream>

namespace ShaderManager {
	Shaders g_Shaders;

	void Init() {
		g_Shaders.GeometryPass.load("GeometryPass.vert", "GeometryPass.frag");		
		g_Shaders.LightingPass.load("LightingPass.vert", "LightingPass.frag");		
		g_Shaders.Basic.load("Basic.vert", "Basic.frag");
		g_Shaders.Debug.load("Debug.vert", "Debug.frag");
		g_Shaders.Matte.load("Matte.vert", "Matte.frag");
		g_Shaders.DebugPoints.load("DebugPoints.vert", "DebugPoints.frag");
		g_Shaders.TextureQuad.load("TextureQuad.vert", "TextureQuad.frag");	
		g_Shaders.ScrQuad.load("ScrQuadDefault.vert", "ScrQuadDefault.frag");
		g_Shaders.PostProcessingBlur.load("PostProcessingBlur.vert", "PostProcessingBlur.frag");
		g_Shaders.DitherQuad.load("BasicDither.vert", "BasicDither.frag");
		g_Shaders.PointsOnly.load("PointsOnly.vert", "PointsOnly.frag");

		g_Shaders.LightingPass.assignUniform("positionTex");
		g_Shaders.LightingPass.assignUniform("albedoTex");
		g_Shaders.LightingPass.assignUniform("normalTex");

		g_Shaders.Matte.assignUniform("color");

		g_Shaders.ScrQuad.assignUniform("screenTexture");

		g_Shaders.PostProcessingBlur.assignUniform("screenTexture");
		g_Shaders.PostProcessingBlur.assignUniform("TexelSize");
		g_Shaders.PostProcessingBlur.assignUniform("horizontal");

		g_Shaders.DitherQuad.assignUniform("screenTexture");
	};
}