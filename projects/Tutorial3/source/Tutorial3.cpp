#include "Tutorial3.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "stb_image.h"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Tutorial3::Tutorial3()
{

}

Tutorial3::~Tutorial3()
{

}

bool Tutorial3::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );

	// get window dimensions to calculate aspect ratio
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / (float)height, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);

	const char * aszInputs[] = { "Position", "Color", "TexCoord1" };
	const char * aszOutputs[] = { "outColor" };

	GLuint vShader = Utility::loadShader("FBX_VertexShader.glsl", GL_VERTEX_SHADER);
	GLuint pShader = Utility::loadShader("FBX_FragmentShader.glsl", GL_FRAGMENT_SHADER);

	m_ShaderProgram = Utility::createProgram(vShader, 0, 0, 0, pShader, 3, aszInputs, 1, aszOutputs);

	m_FBX = new FBXFile();
	m_FBX->load("Models/soulspear/soulspear.fbx", FBXFile::UNITS_CENTIMETER);
	m_FBX->initialiseOpenGLTextures();
	InitFBXSceneResource(m_FBX);

	//
	m_decayValue = 0;

	int decayWidth = 0;
	int decayHeight = 0;
	int decayFormat = 0;
	unsigned char * pixelData = stbi_load("decay.png", &decayWidth, &decayHeight, &decayFormat, STBI_default);
	printf("Width: %i Height: %i Format: %i\n", decayWidth, decayHeight, decayFormat);

	glGenTextures(1, &m_decayTexture);
	glBindTexture(GL_TEXTURE_2D, m_decayTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, decayWidth, decayHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	// delete pixel data here instead!
	delete[] pixelData;
	//

	glDeleteShader(vShader);
	glDeleteShader(pShader);

	return true;
}

void Tutorial3::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a 20x20 grid on the XZ-plane
	for ( int i = 0 ; i < 21 ; ++i )
	{
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	// quit our application when escape is pressed
	//
	if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS) m_decayValue -= a_deltaTime;
	if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) m_decayValue += a_deltaTime;

	if (m_decayValue < 0.0f) m_decayValue = 0.0f;
	if (m_decayValue > 1.0f) m_decayValue = 1.0f;
	//

	UpdateFBXSceneResource(m_FBX);


	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Tutorial3::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	RenderFBXSceneResource(m_FBX, viewMatrix, m_projectionMatrix);
	Gizmos::draw2D(glm::ortho<float>(0, width, 0, height, -1.0f, 1.0f));

	
}

void Tutorial3::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();

	DestroyFBXSceneResource(m_FBX);
	m_FBX->unload();
	delete m_FBX;
	m_FBX = NULL;

	glDeleteProgram(m_ShaderProgram);
}

void Tutorial3::InitFBXSceneResource(FBXFile * a_pScene)
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

void Tutorial3::DestroyFBXSceneResource(FBXFile * a_pScene)
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

void Tutorial3::UpdateFBXSceneResource(FBXFile * a_pScene)
{
	a_pScene->getRoot()->updateGlobalTransform();
}

void Tutorial3::RenderFBXSceneResource(FBXFile * a_pScene, glm::mat4 a_view, glm::mat4 a_projection)
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
	for(int i=0; i<a_pScene->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode *mesh = a_pScene->getMeshByIndex(i);

		// get the render data attached to the m_userData pointer for this mesh
		OGL_FBXRenderData *ro = (OGL_FBXRenderData *) mesh->m_userData;

		// Bind the texture to one of the ActiveTextures
		// if your shader supported multiple textures, you would bind each texture to a new Active Texture ID here
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, mesh->m_material->textures[ FBXMaterial::DiffuseTexture]->handle );

		glActiveTexture( GL_TEXTURE2 );
		glBindTexture( GL_TEXTURE_2D, m_decayTexture );

		// tell the shader which texture to use
		glUniform1i( uDiffuseTexture, 1 );
		glUniform1i( uDecayTexture, 2 );

		// tell the shader the decay value
		glUniform1f( uDecayValue, m_decayValue );
	 
		// send the Model, View and Projection Matrices to the shader
		glUniformMatrix4fv( uModel,	1, false, glm::value_ptr(mesh->m_globalTransform) );
		glUniformMatrix4fv( uView,	1, false, glm::value_ptr(a_view) );
		glUniformMatrix4fv( uProjection,1, false, glm::value_ptr(a_projection) );

		// bind our vertex array object
		// remember in the initialise function, we bound the VAO and IBO to the VAO
		// so when we bind the VAO, openGL knows what what vertices,
		// indices and vertex attributes to send to the shader
		glBindVertexArray(ro->VAO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);

	}

	// reset back to the default active texture
	glActiveTexture( GL_TEXTURE0 );

	// finally, we have finished rendering the meshes
	// disable this shader
	glUseProgram(0);

}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Tutorial3();
	
	if (app->create("AIE - Tutorial3",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}