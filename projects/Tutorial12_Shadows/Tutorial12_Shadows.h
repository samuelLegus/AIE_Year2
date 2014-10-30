#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>	// add to tutorial?

#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>	// add to tutorial?

#include <stb_image.h>
#include <GL\glew.h>

struct TextureData
{
	GLuint textureID;

	int width = 0;
	int height = 0;
	int format = 0;

	int LoadTexture(const char * filePath)
	{
		unsigned char * data = stbi_load(filePath, &width, &height, &format, STBI_default);

		if (data != nullptr)
		{
			printf("--TextureLoad--\nFile: %s\nWidth: %i\nHeight: %i\nFormat: %i\n", filePath, width, height, format);
		}
		else
		{
			printf("Could not load data.\n");
			printf("File: %s\nWidth: %i\nHeight: %i\nFormat: %i\n", filePath, width, height, format);
			return -1;
		}

		// Convert format from STBI format to OpenGL formnat
		switch (format)
		{
		case STBI_grey:
		{
			format = GL_LUMINANCE;
			break;
		}
		case STBI_grey_alpha:
		{
			format = GL_LUMINANCE_ALPHA;
			break;
		}
		case STBI_rgb:
		{
			format = GL_RGB;
			break;
		}
		case STBI_rgb_alpha:
		{
			format = GL_RGBA;
			break;
		}
		default:
		{
			printf("WARNING: Texture format not supported!\n");
		}
		};

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			format,
			width,
			height,
			0,
			format,
			GL_UNSIGNED_BYTE,
			data);

		// Set filtering mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// look into generating mipmaps

		// Unbind texture so another function won't accidentally change it
		glBindTexture(GL_TEXTURE_2D, 0);

		delete[] data;
		return textureID;
	}
};

// derived application class that wraps up all globals neatly
class Tutorial12_Shadows : public Application
{
public:

	Tutorial12_Shadows();
	virtual ~Tutorial12_Shadows();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	

	// FBX
	void InitFBXSceneResource(FBXFile * a_pScene);
	void UpdateFBXSceneResource(FBXFile * a_pScene);
	void RenderFBXSceneResource(FBXFile * a_pScene, glm::mat4 a_view, glm::mat4 a_projection);
	void DestroyFBXSceneResource(FBXFile * a_pScene);

	struct OGL_FBXRenderData
	{
		unsigned int VBO;	// Vertex Buffer Object
		unsigned int IBO;	// Index Buffer Object
		unsigned int VAO;	// Vertex Array Object
	};

	FBXFile * m_fbx;

	// Shadow Tutorial specific stuff
	int m_shadowWidth, m_shadowHeight;
	glm::vec4 m_lightDirection;
	glm::mat4 m_shadowProjectionViewMatrix;
	unsigned int m_program;
	unsigned int m_depthProgram;
	unsigned int m_2dprogram;
	unsigned int m_shadowFramebuffer;
	unsigned int m_shadowTexture;

	struct Quad {
		unsigned int vbo;
		unsigned int vao;
	} m_2dQuad;

	void createShadowBuffer();
	void setupLightAndShadowMatrix();
	void create2DQuad();
	void displayShadowMap();
};

