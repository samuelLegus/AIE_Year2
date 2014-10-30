#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "AdvancedTextureTutorial.h"
#include "FBXFile.h"

AdvancedTextureTutorial::AdvancedTextureTutorial()
{

}

AdvancedTextureTutorial::~AdvancedTextureTutorial()
{

}

void AdvancedTextureTutorial::BuildProgram()
{
	unsigned int vs = Utility::loadShader("shaders/FBXDecayVS.glsl", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("shaders/FBXDecayFS.glsl", GL_FRAGMENT_SHADER);
	const char* inputs[] = { "Position", "Color", "TexCoord1" };
	const char * outputs[] = { "vUV", "vColor" };
	m_program = Utility::createProgram(vs, 0, 0, 0, fs, 3, inputs, 2, outputs);
	glDeleteShader(vs);
	glDeleteShader(fs);
}

bool AdvancedTextureTutorial::onCreate(int a_argc, char* a_argv[])
{
	Assessment::onCreate(a_argc, a_argv);
	BuildProgram();
	m_fbx = new FBXFile();
	if (!m_fbx->load("models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER))
	{
		printf("FBX file could not be loaded!");
	}
	m_fbx->initialiseOpenGLTextures();
	InitFBXSceneResource(m_fbx);

	m_decayTexture.LoadFromFile("images/decayTexture.png");
	m_metallicTexture.LoadFromFile("images/metallicTexture.png");
	m_useSecondaryTexture = false;
	m_decayValue = 1.0f;

	return true;
}

void AdvancedTextureTutorial::onUpdate(float a_deltaTime)
{


	Assessment::onUpdate(a_deltaTime);

	//Controls decay value sent to the shader.
	if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
		m_decayValue -= a_deltaTime;
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) 
		m_decayValue += a_deltaTime;
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
		m_useSecondaryTexture = true;
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		m_useSecondaryTexture = false;

	if (m_decayValue < 0.0f)
		m_decayValue = 0.0f;
	if (m_decayValue > 1.0f)
		m_decayValue = 1.0f;

	UpdateFBXSceneResource(m_fbx);
}

void AdvancedTextureTutorial::onDraw()
{
	Assessment::onDraw();
	glm::mat4 viewMatrix = glm::inverse(m_cameraMatrix);
	RenderFBXSceneResource(m_fbx, viewMatrix, m_projectionMatrix);
}

void AdvancedTextureTutorial::onDestroy()
{
	Assessment::onDestroy();
	DestroyFBXSceneResource(m_fbx);
	m_fbx->unload();
	delete m_fbx;
	m_fbx = NULL;
}

void AdvancedTextureTutorial::InitFBXSceneResource(FBXFile *a_pScene)
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

void AdvancedTextureTutorial::DestroyFBXSceneResource(FBXFile *a_pScene)
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

void AdvancedTextureTutorial::UpdateFBXSceneResource(FBXFile *a_pScene)
{
	a_pScene->getRoot()->updateGlobalTransform();
}

void AdvancedTextureTutorial::RenderFBXSceneResource(FBXFile *a_pScene, glm::mat4 a_view, glm::mat4 a_projection)
{
	// activate a shader
	glUseProgram(m_program);

	// enable transparent blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// get the location of uniforms on the shader
	GLint uDiffuseTexture = glGetUniformLocation(m_program, "DiffuseTexture");
	GLint uDiffuseColor = glGetUniformLocation(m_program, "DiffuseColor");

	GLint uModel = glGetUniformLocation(m_program, "Model");
	GLint uView = glGetUniformLocation(m_program, "View");
	GLint uProjection = glGetUniformLocation(m_program, "Projection");

	// uniforms added to control the decay texture and decay value in the shader
	GLint uDecayTexture = glGetUniformLocation(m_program, "DecayTexture");
	GLint uDecayValue = glGetUniformLocation(m_program, "DecayValue");

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
		if (m_useSecondaryTexture)
			glBindTexture(GL_TEXTURE_2D, m_metallicTexture.textureID);
		else
			glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);

		// decay texture binding
		glActiveTexture(GL_TEXTURE2);

		glBindTexture(GL_TEXTURE_2D, m_decayTexture.textureID);

		// reset back to the default active texture
		glActiveTexture(GL_TEXTURE0);

		// tell the shader which texture to use
		glUniform1i(uDiffuseTexture, 1);
		glUniform1i(uDecayTexture, 2);
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

	// finally, we have finished rendering the meshes
	// disable this shader
	glUseProgram(0);
}