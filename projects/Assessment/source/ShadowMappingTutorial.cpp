#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ShadowMappingTutorial.h"
#include "FBXFile.h"

ShadowMappingTutorial::ShadowMappingTutorial()
{

}

ShadowMappingTutorial::~ShadowMappingTutorial()
{

}

bool ShadowMappingTutorial::onCreate(int a_argc, char* a_argv[])
{
	Assessment::onCreate(a_argc, a_argv);

	//Make the depth shader here.
	unsigned int depth_vs = Utility::loadShader("shaders/ShadowMapRTT_VS.glsl", GL_VERTEX_SHADER);
	unsigned int depth_fs = Utility::loadShader("shaders/ShadowMapRTT_FS.glsl", GL_FRAGMENT_SHADER);
	m_depthShader = Utility::createProgram(depth_vs, 0, 0, 0, depth_fs);
	glDeleteShader(depth_vs);
	glDeleteShader(depth_fs);

	//Loading up out FBX Model here.
	m_fbx = new FBXFile();
	if (!m_fbx->load("models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER))
	{
		printf("FBX file could not be loaded!");
	}
	m_fbx->initialiseOpenGLTextures();
	InitFBXSceneResource(m_fbx);

	//FBO creation code, lines 124 - 146ish Tutorial 16

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// No color output in the bound framebuffer, only depth.
	glDrawBuffer(GL_NONE);

	// Normal shader here
	unsigned int render_vs = Utility::loadShader("shaders/ShadowMapRenderVS.glsl", GL_VERTEX_SHADER);
	unsigned int render_fs = Utility::loadShader("shaders/ShadowMapRenderFS.glsl", GL_FRAGMENT_SHADER);
	m_depthShader = Utility::createProgram(render_vs, 0, 0, 0, render_fs);
	glDeleteShader(render_vs);
	glDeleteShader(render_fs);

	// Setting up Uniforms here.
	m_uTexture = glGetUniformLocation(m_shader, "myTextureSampler");
	m_uMVP = glGetUniformLocation(m_shader, "MVP");
	m_uDepthBiasMVP = glGetUniformLocation(m_shader, "DepthBiasMVP");
	m_uShadowMap = glGetUniformLocation(m_shader, "shadowMap");

	unsigned int quad_vs = Utility::loadShader("shaders/ShadowQuadVS.glsl", GL_VERTEX_SHADER);
	unsigned int quad_fs = Utility::loadShader("shaders/ShadowQuadFS.glsl", GL_FRAGMENT_SHADER);
	m_depthShader = Utility::createProgram(quad_vs, 0, 0, 0, quad_fs);
	glDeleteShader(quad_vs);
	glDeleteShader(quad_fs);
	create2DQuad();


	return true;
}

void ShadowMappingTutorial::onUpdate(float a_deltaTime)
{
	Assessment::onUpdate(a_deltaTime);
	UpdateFBXSceneResource(m_fbx);
}

void ShadowMappingTutorial::onDraw()
{
	Assessment::onDraw();
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);
	RenderFBXSceneResource(m_fbx, viewMatrix, m_projectionMatrix);
	displayShadowMap();
}

void ShadowMappingTutorial::onDestroy()
{
	Assessment::onDestroy();
	DestroyFBXSceneResource(m_fbx);
	m_fbx->unload();
	delete m_fbx;
	m_fbx = NULL;
}

void ShadowMappingTutorial::InitFBXSceneResource(FBXFile *a_pScene)
{
	// how manu meshes and materials are stored within the fbx file
	unsigned int meshCount = a_pScene->getMeshCount();
	unsigned int matCount = a_pScene->getMaterialCount();

	// loop through each mesh
	for (int i = 0; i<meshCount; ++i)
	{
		// get the current mesh
		FBXMeshNode *pMesh = a_pScene->getMeshByIndex(i);

		// genorate our OGL_FBXRenderData for storing the meshes VBO, IBO and VAO
		// and assign it to the meshes m_userData pointer so that we can retrive 
		// it again within the render function
		OGL_FBXRenderData *ro = new OGL_FBXRenderData();
		pMesh->m_userData = ro;

		// OPENGL: genorate the VBO, IBO and VAO
		glGenBuffers(1, &ro->VBO);
		glGenBuffers(1, &ro->IBO);
		glGenVertexArrays(1, &ro->VAO);

		// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
		glBindVertexArray(ro->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, ro->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ro->IBO);

		// Send the vertex data to the VBO
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

		// send the index data to the IBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		// enable the attribute locations that will be used on our shaders
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::NormalOffset);

		// finally, where done describing our mesh to the shader
		// we can describe the next mesh
		glBindVertexArray(0);
	}


}

void ShadowMappingTutorial::DestroyFBXSceneResource(FBXFile *a_pScene)
{
	// how manu meshes and materials are stored within the fbx file
	unsigned int meshCount = a_pScene->getMeshCount();
	unsigned int matCount = a_pScene->getMaterialCount();

	// remove meshes
	for (unsigned int i = 0; i<meshCount; i++)
	{
		// Get the current mesh and retrive the render data we assigned to m_userData
		FBXMeshNode* pMesh = a_pScene->getMeshByIndex(i);
		OGL_FBXRenderData *ro = (OGL_FBXRenderData *)pMesh->m_userData;

		// delete the buffers and free memory from the graphics card
		glDeleteBuffers(1, &ro->VBO);
		glDeleteBuffers(1, &ro->IBO);
		glDeleteVertexArrays(1, &ro->VAO);

		// this is memory we created earlier in the InitFBXSceneResources function
		// make sure to destroy it
		delete ro;

	}

	// loop through each of the materials
	for (int i = 0; i<matCount; i++)
	{
		// get the current material
		FBXMaterial *pMaterial = a_pScene->getMaterialByIndex(i);
		for (int j = 0; j<FBXMaterial::TextureTypes_Count; j++)
		{
			// delete the texture if it was loaded
			/*if (pMaterial->textureIDs[j] != 0)
			glDeleteTextures(1, &pMaterial->textureIDs[j]);*/
		}
	}
}

void ShadowMappingTutorial::UpdateFBXSceneResource(FBXFile *a_pScene)
{
	a_pScene->getRoot()->updateGlobalTransform();
}

void ShadowMappingTutorial::RenderFBXSceneResource(FBXFile *a_pScene, glm::mat4 a_view, glm::mat4 a_projection)
{
	// Depth Shader Shenanigans here.
	m_uTexture = glGetUniformLocation(m_shader, "myTextureSampler");
	m_uMVP = glGetUniformLocation(m_shader, "MVP");
	m_uDepthBiasMVP = glGetUniformLocation(m_shader, "DepthBiasMVP");
	m_uShadowMap = glGetUniformLocation(m_shader, "shadowMap");
	// for each mesh in the model...
	for (int i = 0; i<a_pScene->getMeshCount(); ++i)
	{
		//depth shader stuff
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
		glViewport(0, 0, 1024, 1024);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(m_depthShader);

		// get the current mesh
		FBXMeshNode *mesh = a_pScene->getMeshByIndex(i);

		// get the render data attached to the m_userData pointer for this mesh
		OGL_FBXRenderData *ro = (OGL_FBXRenderData *)mesh->m_userData;

		// Compute the MVP matrix from the light's point of view
		glm::vec3 lightInvDir = glm::vec3(1, 1, 0);

		// Compute the MVP matrix from the light's point of view
		glm::mat4 depthProjectionMatrix = glm::ortho<float>(-3, 3, -3, 3, -50, 50);
		glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 depthModelMatrix = glm::mat4(1.0);
		glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

		glUniformMatrix4fv(m_uDepthMVP, 1, GL_FALSE, &depthMVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, ro->VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::PositionOffset);
		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ro->IBO);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,      // mode
			mesh->m_indices.size(),    // count
			GL_UNSIGNED_SHORT, // type
			(void*)0           // element array buffer offset
			);

		glDisableVertexAttribArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//normal shader stuff
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(m_shader);
		
		glm::mat4 MVP = m_projectionMatrix * glm::inverse(m_cameraMatrix) * mesh->m_globalTransform;
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
			);

		glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
		glUniformMatrix4fv(m_uMVP, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(m_uDepthBiasMVP, 1, GL_FALSE, &depthBiasMVP[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(m_uTexture, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);
		glUniform1i(m_uShadowMap, 1);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, ro->VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::NormalOffset);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ro->IBO);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,      // mode
			mesh->m_indices.size(),    // count
			GL_UNSIGNED_SHORT, // type
			(void*)0           // element array buffer offset
			);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

	}

	glUseProgram(0);
}