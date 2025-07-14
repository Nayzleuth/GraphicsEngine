#include <vector>
#include <iostream>
#include "Renderer.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../Core/AssetManager.h"
#include "../Core/Scene.h"
#include "../Core/BackEnd.h"
#include "../Core/Physics.h"


struct FBO {
	unsigned int BufferID;
	std::unordered_map<std::string, unsigned int> Attachments;

	unsigned int width, height;

	FBO(unsigned int passedWidth, unsigned int passedHeight) {
		glGenFramebuffers(1, &BufferID);
		width = passedWidth;
		height = passedHeight;
	}

	void createTexAttachment(std::string attachmentName, GLenum attachmentPoint, GLenum bitCount) {
		glBindFramebuffer(GL_FRAMEBUFFER, BufferID);

		// Color Tex
		unsigned int colorTex;
		glGenTextures(1, &colorTex);
		std::cout << "colorTex with name " << attachmentName << " is " << colorTex << "\n";
		glBindTexture(GL_TEXTURE_2D, colorTex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, bitCount, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D, colorTex, 0);

		Attachments[attachmentName] = colorTex;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void createDepthAttachment(std::string attachmentName) {
		glBindFramebuffer(GL_FRAMEBUFFER, BufferID);

		// Depth Texture
		unsigned int depthTex;
		glGenTextures(1, &depthTex);
		glBindTexture(GL_TEXTURE_2D, depthTex);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);
		
		Attachments[attachmentName] = depthTex;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

namespace Renderer {
	void GeometryPass();
	void BloomPass();
	void BlendPass();
	void LightingPass();
	void DrawDebugMaterials();
	void DebugFBO();

	std::vector<FBO> blurFBOs;
	FBO* mainBuffer = nullptr;
	FBO* gBuffer = nullptr;



	void Init() {
		glClearColor(0, 0, 0, 1);
		glPointSize(5);

		mainBuffer = new FBO(BackEnd::scrWidth, BackEnd::scrHeight);
		mainBuffer->createTexAttachment("frame", GL_COLOR_ATTACHMENT0, GL_RGBA32F);
		mainBuffer->createDepthAttachment("Depth");

		gBuffer = new FBO(BackEnd::scrWidth, BackEnd::scrHeight);
		gBuffer->createTexAttachment("position", GL_COLOR_ATTACHMENT0, GL_RGBA32F);
		gBuffer->createTexAttachment("albedo", GL_COLOR_ATTACHMENT1, GL_RGBA32F);
		gBuffer->createTexAttachment("normal", GL_COLOR_ATTACHMENT2, GL_RGBA32F);
		gBuffer->createDepthAttachment("depth");

		unsigned int tempWidth = BackEnd::scrWidth;
		unsigned int tempHeight = BackEnd::scrHeight;

		for (int i = 0; i < 4; i++) {
			FBO blur(tempWidth, tempHeight);


			blur.createTexAttachment("TexOne", GL_COLOR_ATTACHMENT0, GL_RGBA16F);
			blur.createTexAttachment("TexTwo", GL_COLOR_ATTACHMENT1, GL_RGBA16F);
			blur.createDepthAttachment("Depth");

			glBindFramebuffer(GL_FRAMEBUFFER, blur.BufferID);
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "Framebuffer " << i << " is incomplete!\n";
			}

			blurFBOs.push_back(blur);

			tempWidth /= 2;
			tempHeight /= 2;
		}												
	};

	void CleanUp() {
		glDeleteFramebuffers(1, &blurFBOs[0].BufferID);
	}
	
	void RenderFrame() {
		GeometryPass();
		LightingPass();
		BloomPass();
		BlendPass();

		DrawDebugMaterials();
		//DebugFBO();
	};

	void GeometryPass() {
		// 0 position 1 albedo 2 normal
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->BufferID);
		GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, BackEnd::scrWidth, BackEnd::scrHeight);

		for (Object& nextObj : Scene::g_ObjectsInScene) {
			if (nextObj.material.color.x == -1) {
				nextObj.renderShaderOverride(ShaderManager::g_Shaders.GeometryPass);
			}
		}  
		
		for (StaticBodyCube& nextObj : Physics::g_StaticCubes) {
			nextObj.body.renderShaderOverride(ShaderManager::g_Shaders.GeometryPass);
		}  
		
		for (RigidBodyCube& nextObj : Physics::g_RigidCubes) {
			nextObj.body.renderShaderOverride(ShaderManager::g_Shaders.GeometryPass);
		}
	};

	void BloomPass() {
		glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs[0].BufferID);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glViewport(0, 0, blurFBOs[0].width, blurFBOs[0].height);		

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);

		glBlitNamedFramebuffer(gBuffer->BufferID, blurFBOs[0].BufferID,
			0, 0, gBuffer->width, gBuffer->height,
			0, 0, blurFBOs[0].width, blurFBOs[0].height,
			GL_DEPTH_BUFFER_BIT,
			GL_NEAREST);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);

		for (Object nextObj : Scene::g_ObjectsInScene) {
			if (nextObj.material.color.x == -1) {
				nextObj.render();
			}
		}

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		for (Object nextObj : Scene::g_ObjectsInScene) {
			if (nextObj.material.color.x != -1) {
				nextObj.render();
			}
		}

		glDisable(GL_STENCIL_TEST);
		glDisable(GL_DEPTH_TEST);

		// Horizontal
		ShaderManager::g_Shaders.PostProcessingBlur.use();
		ShaderManager::g_Shaders.PostProcessingBlur.updateInt("screenTexture", 0);

		for (int i = 0; i < 4; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs[i].BufferID);
			glViewport(0, 0, blurFBOs[i].width, blurFBOs[i].height);

			ShaderManager::g_Shaders.PostProcessingBlur.updateVec2("TexelSize", glm::vec2(1.0f / blurFBOs[i].width, 1.0f / blurFBOs[i].height));
			ShaderManager::g_Shaders.PostProcessingBlur.updateInt("horizontal", true);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, blurFBOs[i].Attachments["TexOne"]);
			glDrawBuffer(GL_COLOR_ATTACHMENT1);				

			RenderScreenQuad(ShaderManager::g_Shaders.PostProcessingBlur);

			// Vertical
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glBindTexture(GL_TEXTURE_2D, blurFBOs[i].Attachments["TexTwo"]);

			ShaderManager::g_Shaders.PostProcessingBlur.updateInt("horizontal", false);
			RenderScreenQuad(ShaderManager::g_Shaders.PostProcessingBlur);

			if (i < 3) {
				glBlitNamedFramebuffer(blurFBOs[i].BufferID, blurFBOs[i + 1].BufferID,
					0, 0, blurFBOs[i].width, blurFBOs[i].height,
					0, 0, blurFBOs[i + 1].width, blurFBOs[i + 1].height,
					GL_COLOR_BUFFER_BIT,
					GL_LINEAR);					
			}
		}

		glEnable(GL_DEPTH_TEST);
		glBindVertexArray(0);
	}

	void BlendPass() {
		glViewport(0, 0, BackEnd::scrWidth, BackEnd::scrHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ShaderManager::g_Shaders.ScrQuad.use();
		ShaderManager::g_Shaders.ScrQuad.updateInt("screenTexture", 0);
		
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_DEPTH_TEST);

		// Main Geometry (Post Lighting)
		glBindTexture(GL_TEXTURE_2D, mainBuffer->Attachments.at("frame"));
		RenderScreenQuad(ShaderManager::g_Shaders.DitherQuad);				

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		// Gaussian Blur
		glBindTexture(GL_TEXTURE_2D, blurFBOs[3].Attachments.at("TexOne"));
		RenderScreenQuad(ShaderManager::g_Shaders.ScrQuad);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		glClear(GL_DEPTH_BUFFER_BIT);
		glBlitNamedFramebuffer(gBuffer->BufferID, 0,
			0, 0, gBuffer->width, gBuffer->height,
			0, 0, BackEnd::scrWidth, BackEnd::scrHeight,
			GL_DEPTH_BUFFER_BIT,
			GL_NEAREST);

		for (Object nextObj : Scene::g_ObjectsInScene) {
			if (nextObj.material.color.x != -1) {
				nextObj.render();
			}
		}
	}

	void LightingPass() {
		glViewport(0, 0, BackEnd::scrWidth, BackEnd::scrHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, mainBuffer->BufferID);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gBuffer->Attachments.at("position"));	
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gBuffer->Attachments.at("albedo")); 
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gBuffer->Attachments.at("normal"));

		ShaderManager::g_Shaders.LightingPass.use();
		ShaderManager::g_Shaders.LightingPass.updateInt("positionTex", 0);
		ShaderManager::g_Shaders.LightingPass.updateInt("albedoTex", 1);
		ShaderManager::g_Shaders.LightingPass.updateInt("normalTex", 2);

		RenderScreenQuad(ShaderManager::g_Shaders.LightingPass);
	}

	void DrawDebugMaterials() {
		// Draw Points
		ShaderManager::g_Shaders.DebugPoints.use();
		ShaderManager::g_Shaders.DebugPoints.updateMat4("model", glm::mat4(1));
		ShaderManager::g_Shaders.DebugPoints.updateMat4("view", BackEnd::currentCam->getView());
		ShaderManager::g_Shaders.DebugPoints.updateMat4("projection", BackEnd::currentCam->getProj());

		ShaderManager::g_Shaders.PointsOnly.use();
		ShaderManager::g_Shaders.PointsOnly.updateMat4("view", BackEnd::currentCam->getView());
		ShaderManager::g_Shaders.PointsOnly.updateMat4("projection", BackEnd::currentCam->getProj());

		ShaderManager::g_Shaders.DebugPoints.use();
		glBindVertexArray(AssetManager::g_VAOs.at("DebugPoints").VAO);
		glDrawArrays(GL_POINTS, 0, Scene::g_DebugPoints.size());
		
		// Draw Lines
		for (Line nextLine : Scene::g_DebugStripLines) {
			glBindVertexArray(nextLine.vao.VAO);
			glDrawArrays(GL_LINE_STRIP, 0, nextLine.linePoints.size());
			glDrawArrays(GL_POINTS, 0, nextLine.linePoints.size());
		} 
		
		for (Line nextLine : Scene::g_DebugLoopLines) {
			glBindVertexArray(nextLine.vao.VAO);
			glDrawArrays(GL_LINE_LOOP, 0, nextLine.linePoints.size());
			glDrawArrays(GL_POINTS, 0, nextLine.linePoints.size());
		}

		// Draw Squares
		for (Square nextSquare : Scene::g_DebugSquares) {
			glBindVertexArray(nextSquare.squareOutline.vao.VAO);
			ShaderManager::g_Shaders.DebugPoints.updateMat4("model", nextSquare.model);
			glDrawArrays(GL_LINE_LOOP, 0, nextSquare.squareOutline.linePoints.size());
			glDrawArrays(GL_POINTS, 0, nextSquare.squareOutline.linePoints.size());
		}

		// Draw Cubes
		for (RectangularPrism& nextCube : Scene::g_DebugCubes) {
			nextCube.RenderWireframe();
		} 
		
		for (DynamicRecPrism& nextCube : Scene::g_DebugDynamicCubes) {
			nextCube.RenderWireframe();
		}
	};

	void DebugFBO() {
		glViewport(0, 0, BackEnd::scrWidth, BackEnd::scrHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, blurFBOs[3].Attachments.at("TexOne"));
		ShaderManager::g_Shaders.ScrQuad.use();
		ShaderManager::g_Shaders.ScrQuad.updateInt("screenTexture", 0);
		RenderScreenQuad(ShaderManager::g_Shaders.ScrQuad);
	};

	void RenderScreenQuad(Shader usedShader) {
		AssetManager::g_MeshCollection.at("ScreenQuad").useVAO();
		usedShader.use();
		glDrawElements(GL_TRIANGLES, AssetManager::g_MeshCollection.at("ScreenQuad").getIndexCount(), GL_UNSIGNED_INT, 0);
	}
}