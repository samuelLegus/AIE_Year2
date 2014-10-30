#include "Tutorial4.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "Shader.h"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Tutorial4::Tutorial4()
{

}

Tutorial4::~Tutorial4()
{

}

bool Tutorial4::onCreate(int a_argc, char* a_argv[]) 
{
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
	glEnable(GL_CULL_FACE);

	m_renderProgram = LoadShaders("VertexShader.glsl", "FragmentShader.glsl");

	// load an image using STB

	// temp vars for storing the dimensions/format of our image retrieved by STB
	int imgWidth = 0;
	int imgHeight = 0;
	int imgFormat = 0;

	unsigned char * pixelData = stbi_load(	"./textures/numbered_grid.tga",
											&imgWidth,
											&imgHeight,
											&imgFormat,
											STBI_default);



	printf("\nWidth: %i, Height: %i, Format: %i", imgWidth, imgHeight, imgFormat);

	// texture generation, binding, and buffering

	// OGL DOCUMENTATION
	// http://docs.gl/gl4/glGenTextures
	// http://docs.gl/gl4/glBindTexture
	// http://docs.gl/gl4/glTexImage2D

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(	GL_TEXTURE_2D,		//target - only certain GLenum values are accepted.
					0,					//level
					imgFormat,			//internal format - STB does NOT retrieve this properly ._.
					imgWidth,			//width
					imgHeight,			//height
					0,					//border - must be 0 (even the documentation says so)
					GL_RGBA,			//format - 
					GL_UNSIGNED_BYTE,	//type 
					pixelData);			//data - the data retrieved by STB

	// filtering
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// clear bound texture state so we don't accidentally change it
	 glBindTexture(GL_TEXTURE_2D, 0);


	// creating a simple plane to render
	Utility::build3DPlane(10, m_VAO, m_VBO, m_IBO);

	delete[] pixelData;

	return true;
}

void Tutorial4::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	//Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Tutorial4::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(m_renderProgram);

	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	//Gizmos::draw2D(glm::ortho<float>(0, width, 0, height, -1.0f, 1.0f));

	//texture drawing and shit

	unsigned int location = glGetUniformLocation(m_renderProgram, "view");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(viewMatrix));

	location = glGetUniformLocation(m_renderProgram, "projection");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(m_projectionMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	location = glGetUniformLocation(m_renderProgram, "textureMap");
	glUniform1i(location, 0);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}


void Tutorial4::onDestroy()
{
	// clean up anything we created
	// delete the data for the plane
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_IBO);

	// delete the texture
	glDeleteTextures(1, &m_texture);

	glDeleteShader(m_vertShader);
	glDeleteShader(m_fragShader);
	// delete the shader
	glDeleteProgram(m_renderProgram);


}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Tutorial4();
	
	if (app->create("AIE - Tutorial4",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}