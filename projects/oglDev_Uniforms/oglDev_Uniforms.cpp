#include "oglDev_Uniforms.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

oglDev_Uniforms::oglDev_Uniforms()
{

}

oglDev_Uniforms::~oglDev_Uniforms()
{

}

bool oglDev_Uniforms::onCreate(int a_argc, char* a_argv[]) 
{

	this->CreateVertexBuffer();
	this->CreateShaderProgram();

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

	return true;
}

void oglDev_Uniforms::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	
	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void oglDev_Uniforms::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	
	this->RenderTriangle();
}

void oglDev_Uniforms::onDestroy()
{

}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new oglDev_Uniforms();
	
	if (app->create("AIE - oglDev_Uniforms",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}

// Tutorial functions :

void oglDev_Uniforms::CreateVertexBuffer()
{
	glm::vec3 Vertices[3];
	Vertices[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
	Vertices[1] = glm::vec3(1.0f, -1.0f, 0.0f);
	Vertices[2] = glm::vec3(0.0f, 1.0f, 0.0f);

	glGenBuffers(1, &m_vbo); // create generic buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo); // specify what buffer is for
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW); // shove data into buffer
}

void oglDev_Uniforms::CreateShaderProgram()
{
	//loading the shaders like I did before mostly.
	GLuint vs = Utility::loadShader("basic.vert", GL_VERTEX_SHADER);
	GLuint fs = Utility::loadShader("basic.frag", GL_FRAGMENT_SHADER);

	m_program = Utility::createProgram(vs, 0, 0, 0, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	//except here I run the program once so I can setup the scale uniform
	//I think that setting uniforms every frame like AIE does isn't always necessary
	glUseProgram(m_program);
	m_gScaleLocation = glGetUniformLocation(m_program, "gScale");
}

void oglDev_Uniforms::RenderTriangle()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;

	Scale += 0.001f;

	glUniform1f(m_gScaleLocation, sinf(Scale));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glDisableVertexAttribArray(0);
}