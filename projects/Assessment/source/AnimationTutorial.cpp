#include "AnimationTutorial.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

AnimationTutorial::AnimationTutorial()
{
}

AnimationTutorial::~AnimationTutorial()
{

}

void AnimationTutorial::BuildProgram()
{
	unsigned int vs = Utility::loadShader("shaders/AnimationVS.glsl", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/AnimationFS.glsl", GL_FRAGMENT_SHADER);

	m_shader = Utility::createProgram(vs, 0, 0, 0, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);
}

bool AnimationTutorial::onCreate(int a_argc, char * a_argv[])
{
	Assessment::onCreate(a_argc, a_argv);

	BuildProgram();

	m_fbx = new FBXFile();
	if (!m_fbx->load("models/characters/Pyro/pyro.fbx", FBXFile::UNITS_METER))
	{
		printf("FBX file could not be loaded!");
	}
	m_fbx->initialiseOpenGLTextures();
	InitFBXSceneResource(m_fbx);

	return true;
}

void AnimationTutorial::onUpdate(float a_deltaTime)
{
	Assessment::onUpdate(a_deltaTime);
	FBXSkeleton * skeleton = m_fbx->getSkeletonByIndex(0);
	FBXAnimation * animation = m_fbx->getAnimationByIndex(0);

	skeleton->evaluate(animation, Utility::getTotalTime());
	UpdateFBXSceneResource(m_fbx);
}

void AnimationTutorial::onDraw()
{
	Assessment::onDraw();
	glm::mat4 view = glm::inverse(m_cameraMatrix);
	RenderFBXSceneResource(m_fbx, view, m_projectionMatrix);
}

void AnimationTutorial::onDestroy()
{
	Assessment::onDestroy();
}

void AnimationTutorial::InitFBXSceneResource(FBXFile * a_pScene)
{
	unsigned int meshCount = a_pScene->getMeshCount();
	unsigned int matCount = a_pScene->getMaterialCount();

	// loop over meshes
	for (int i = 0; i < meshCount; ++i)
	{

		// get current mesh
		FBXMeshNode * pMesh = a_pScene->getMeshByIndex(i);

		OGL_FBXRenderData * ro = new OGL_FBXRenderData();
		pMesh->m_userData = ro;

		// generate buffers
		glGenBuffers(1, &ro->VBO);
		glGenBuffers(1, &ro->IBO);
		glGenVertexArrays(1, &ro->VAO);

		// bind VAO, then VBO and IBO to VAO
		glBindVertexArray(ro->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, ro->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ro->IBO);

		// send data to VBO
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);

		// send data to IBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		// enable attribute locations on shaders
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::IndicesOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::WeightsOffset);

		glBindVertexArray(0);
	}
}

void AnimationTutorial::UpdateFBXSceneResource(FBXFile * a_pScene)
{
	a_pScene->getRoot()->updateGlobalTransform();
}

void AnimationTutorial::RenderFBXSceneResource(FBXFile * a_pScene, glm::mat4 a_view, glm::mat4 a_projection)
{

	glUseProgram(m_shader);

	FBXSkeleton * skeleton = a_pScene->getSkeletonByIndex(0);
	skeleton->updateBones();

	GLuint uBones = glGetUniformLocation(m_shader, "Bones");
	GLuint uDiffuseTex = glGetUniformLocation(m_shader, "DiffuseTexture");
	//GLuint uMVP = glGetUniformLocation(m_shader, "MVP");

	GLuint uModel = glGetUniformLocation(m_shader, "Model");
	GLuint uView = glGetUniformLocation(m_shader, "View");
	GLuint uProjection = glGetUniformLocation(m_shader, "Projection");

	for (int i = 0; i < m_fbx->getMeshCount(); i++)
	{
		FBXMeshNode * mesh = m_fbx->getMeshByIndex(i);

		OGL_FBXRenderData *ro = (OGL_FBXRenderData*)mesh->m_userData;

		//glm::mat4 MVP = a_projection * a_view * mesh->m_globalTransform;
		//glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(mesh->m_globalTransform));
		glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(a_view));
		glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(a_projection));
		glUniformMatrix4fv(uBones, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);
		glUniform1i(uDiffuseTex, 1);

		glBindVertexArray(ro->VAO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	// Unbind program
	glUseProgram(0);
}

void AnimationTutorial::DestroyFBXSceneResource(FBXFile * a_pScene)
{
	// No. of Meshes and Materials in FBX File
	unsigned int meshCount = a_pScene->getMeshCount();
	unsigned int matCount = a_pScene->getMaterialCount();

	// remove meshes
	for (unsigned int i = 0; i < meshCount; i++)
	{
		// current mesh and retrieve the render data we assigned ot m_userData
		FBXMeshNode * pMesh = a_pScene->getMeshByIndex(i);
		OGL_FBXRenderData * ro = (OGL_FBXRenderData *)pMesh->m_userData;

		glDeleteBuffers(1, &ro->VBO);
		glDeleteBuffers(1, &ro->IBO);
		glDeleteVertexArrays(1, &ro->VAO);

		delete ro;
	}

	for (int i = 0; i < matCount; i++)
	{
		FBXMaterial * pMaterial = a_pScene->getMaterialByIndex(i);
		for (int j = 0; j < FBXMaterial::TextureTypes_Count; j++)
		{
			if (pMaterial->textures[j] != NULL)
			{
				glDeleteTextures(1, &pMaterial->textures[j]->handle);
			}
		}
	}
}





