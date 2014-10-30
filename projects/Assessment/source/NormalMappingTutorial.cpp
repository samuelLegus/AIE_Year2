#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "NormalMappingTutorial.h"
#include "FBXFile.h"

NormalMappingTutorial::NormalMappingTutorial()
{

}

NormalMappingTutorial::~NormalMappingTutorial()
{

}

void NormalMappingTutorial::BuildProgram()
{
	unsigned int vs = Utility::loadShader("shaders/NormalMapVS.glsl", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/NormalMapFS.glsl", GL_FRAGMENT_SHADER);
	const char* inputs[] = { "Position", "Normal", "Tangent", "BiNormal", "TexCoord1" };
	const char * outputs[] = { "ourColor" };
	m_program = Utility::createProgram(vs, 0, 0, 0, fs, 5, inputs, 1, outputs);
	glDeleteShader(vs);
	glDeleteShader(fs);
}

bool NormalMappingTutorial::onCreate(int a_argc, char* a_argv[])
{
	Assessment::onCreate(a_argc, a_argv);
	BuildProgram();
	m_model.fbx = new FBXFile();
	if (!m_model.fbx->load("models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER))
	{
		printf("FBX file could not be loaded!");
	}
	m_model.fbx->initialiseOpenGLTextures();
	InitFBXSceneResource(m_model.fbx);

	m_model.lightPosition = glm::vec3(0, 5, 0);
	m_model.lightDirection = glm::vec3(0, -1, 0);
	m_model.lightColor = glm::vec3(1, 1, 1);
	m_model.ambientLightColor = glm::vec4(0, 0, 1, .75);

	m_model.normalMap.LoadFromFile("models/soulspear/soulspear_normal.tga");
	m_model.specularMap.LoadFromFile("models/soulspear/soulspear_specular.tga");

	return true;
}

void NormalMappingTutorial::onUpdate(float a_deltaTime)
{
	Assessment::onUpdate(a_deltaTime);
	UpdateFBXSceneResource(m_model.fbx);
	m_model.lightPosition.z = std::sin(glfwGetTime()) * 2.5f;
	m_model.lightPosition.x = std::cos(glfwGetTime()) * 2.5f;
}

void NormalMappingTutorial::onDraw()
{
	Assessment::onDraw();
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);
	RenderFBXSceneResource(m_model.fbx, viewMatrix, m_projectionMatrix);
}

void NormalMappingTutorial::onDestroy()
{
	Assessment::onDestroy();
	DestroyFBXSceneResource(m_model.fbx);
	m_model.fbx->unload();
	delete m_model.fbx;
	m_model.fbx = NULL;
}

void NormalMappingTutorial::InitFBXSceneResource(FBXFile *a_pScene)
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
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);


		// tell our shaders where the information within our buffers lie
		// eg: attrubute 0 is expected to be the verticy's position. it should be 4 floats, representing xyzw
		// eg: attrubute 1 is expected to be the verticy's color. it should be 4 floats, representing rgba
		// eg: attrubute 2 is expected to be the verticy's texture coordinate. it should be 2 floats, representing U and V
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::NormalOffset);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::TangentOffset);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::BiNormalOffset);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::TexCoord1Offset);


		// finally, where done describing our mesh to the shader
		// we can describe the next mesh
		glBindVertexArray(0);
	}


}

void NormalMappingTutorial::DestroyFBXSceneResource(FBXFile *a_pScene)
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

void NormalMappingTutorial::UpdateFBXSceneResource(FBXFile *a_pScene)
{
	a_pScene->getRoot()->updateGlobalTransform();
}

void NormalMappingTutorial::RenderFBXSceneResource(FBXFile *a_pScene, glm::mat4 a_view, glm::mat4 a_projection)
{
	// activate a shader
	glUseProgram(m_program);

	// enable transparent blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// get the location of uniforms on the shader
	GLint uMVP = glGetUniformLocation(m_program, "MVP");
	GLint uMV = glGetUniformLocation(m_program, "MV");
	GLint uNormal = glGetUniformLocation(m_program, "NormalMatrix");

	GLint uLightPos = glGetUniformLocation(m_program, "LightPosition");

	//fs
	GLint uLightColor = glGetUniformLocation(m_program, "LightColor");
	GLint uAmbLightColor = glGetUniformLocation(m_program, "AmbientLightColor");

	GLint uDiffuseTexture = glGetUniformLocation(m_program, "DiffuseTexture");
	GLint uNormalTexture = glGetUniformLocation(m_program, "NormalTexture");
	GLint uSpecularTexture = glGetUniformLocation(m_program, "SpecularTexture");

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
		glBindTexture(GL_TEXTURE_2D, m_model.normalMap.textureID);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_model.specularMap.textureID);

		// tell the shader which texture to use
		glUniform1i(uDiffuseTexture, 1);
		glUniform1i(uNormalTexture, 2);
		glUniform1i(uSpecularTexture, 3);

		// send the Model, View and Projection Matrices to the shader
		glm::mat4 MVP = m_projectionMatrix * a_view * mesh->m_globalTransform;
		glm::mat4 MV = a_view * mesh->m_globalTransform;
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(a_view * mesh->m_globalTransform)));

		glUniformMatrix4fv(uMVP, 1, false, glm::value_ptr(MVP));
		glUniformMatrix4fv(uMV, 1, false, glm::value_ptr(MV));
		glUniformMatrix3fv(uNormal, 1, false, glm::value_ptr(normalMatrix));
		glUniform3fv(uLightPos, 1, glm::value_ptr(m_model.lightPosition));
		glUniform3fv(uLightColor, 1, glm::value_ptr(m_model.lightColor));
		glUniform4fv(uAmbLightColor, 1, glm::value_ptr(m_model.ambientLightColor));

		// bind our vertex array object
		// remember in the initialise function, we bound the VAO and IBO to the VAO
		// so when we bind the VAO, openGL knows what what vertices,
		// indices and vertex attributes to send to the shader
		glBindVertexArray(ro->VAO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	glActiveTexture(GL_TEXTURE0);
	glUseProgram(0);
}