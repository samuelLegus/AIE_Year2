#include "FBXManager.h"
#include <GL/glew.h>

void FBXManager::InitFBXSceneResource(FBXFile * a_pScene)
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

		// tell our shaders where the information within our buffers lie
		// eg: attrubute 0 is expected to be the verticy's position. it should be 4 floats, representing xyzw
		// eg: attrubute 1 is expected to be the verticy's color. it should be 4 floats, representing rgba
		// eg: attrubute 2 is expected to be the verticy's texture coordinate. it should be 2 floats, representing U and V
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::ColourOffset);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::TexCoord1Offset);

		// finally, where done describing our mesh to the shader
		// we can describe the next mesh
		glBindVertexArray(0);
	}
}

void FBXManager::DestroyFBXSceneResource(FBXFile * a_pScene)
{
	unsigned int meshCount = a_pScene->getMeshCount();
	unsigned int matCount = a_pScene->getMaterialCount();

	for (unsigned int i = 0; i < meshCount; ++i)
	{
		FBXMeshNode * pMesh = a_pScene->getMeshByIndex(i);
		OGL_FBXRenderData * ro = (OGL_FBXRenderData *)pMesh->m_userData;

		glDeleteBuffers(1, &ro->VBO);
		glDeleteBuffers(1, &ro->IBO);
		glDeleteBuffers(1, &ro->VAO);

		delete ro;
	}

	for (unsigned int i = 0; i < matCount; ++i)
	{
		FBXMaterial *pMaterial = a_pScene->getMaterialByIndex(i);
		// ?????
	}

}

void FBXManager::UpdateFBXSceneResource(FBXFile * a_pScene)
{
	a_pScene->getRoot()->updateGlobalTransform();
}

void FBXManager::RenderFBXSceneResource(FBXFile * a_pScene, glm::mat4 a_view, glm::mat4 a_projection)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// activate a shader
	glUseProgram(m_ShaderProgram);
	// enable transparent blending


	// activate a shader
	glUseProgram(m_ShaderProgram);

	// get the location of uniforms on the shader
	GLint uDiffuseTexture = glGetUniformLocation(m_ShaderProgram, "DiffuseTexture");
	GLint uDiffuseColor = glGetUniformLocation(m_ShaderProgram, "DiffuseColor");

	GLint uModel = glGetUniformLocation(m_ShaderProgram, "Model");
	GLint uView = glGetUniformLocation(m_ShaderProgram, "View");
	GLint uProjection = glGetUniformLocation(m_ShaderProgram, "Projection");

	GLint uDecayTexture = glGetUniformLocation(m_ShaderProgram, "DecayTexture");
	GLint uDecayValue = glGetUniformLocation(m_ShaderProgram, "DecayValue");

	// for each mesh in the model...
	for (int i = 0; i<a_pScene->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode *mesh = a_pScene->getMeshByIndex(i);

		// get the render data attached to the m_userData pointer for this mesh
		OGL_FBXRenderData *ro = (OGL_FBXRenderData *)mesh->m_userData;

		// Bind the texture to one of the ActiveTextures
		// if your shader supported multiple textures, you would bind each texture to a new Active Texture ID here
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_decayTexture);

		// tell the shader which texture to use
		glUniform1i(uDiffuseTexture, 1);
		glUniform1i(uDecayTexture, 2);

		// tell the shader the decay value
		glUniform1f(uDecayValue, m_decayValue);

		// send the Model, View and Projection Matrices to the shader
		glUniformMatrix4fv(uModel, 1, false, glm::value_ptr(mesh->m_globalTransform));
		glUniformMatrix4fv(uView, 1, false, glm::value_ptr(a_view));
		glUniformMatrix4fv(uProjection, 1, false, glm::value_ptr(a_projection));

		// bind our vertex array object
		// remember in the initialise function, we bound the VAO and IBO to the VAO
		// so when we bind the VAO, openGL knows what what vertices,
		// indices and vertex attributes to send to the shader
		glBindVertexArray(ro->VAO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);

	}

	// reset back to the default active texture
	glActiveTexture(GL_TEXTURE0);

	// finally, we have finished rendering the meshes
	// disable this shader
	glUseProgram(0);

}