#include "PhysicsCustom.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

PhysicsCustom::PhysicsCustom()
{

}

PhysicsCustom::~PhysicsCustom()
{

}

bool PhysicsCustom::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

	//Ortho camera code - deciding not to use this afterall
	//float aspectRatio = DEFAULT_SCREENWIDTH / (float)DEFAULT_SCREENHEIGHT;
	//float fSize = 50;
	//float farPlane = 200;
	// create the ortho camera
	//m_projectionMatrix = glm::ortho<float>(-fSize * aspectRatio, fSize * aspectRatio, -fSize, fSize, 0, farPlane);

	//Top down perspective
	//Remember that the z value for the first paramater must be >= .1 cause that's how we set it up in the glm::perspective call .
	m_cameraMatrix = glm::inverse(glm::lookAt(glm::vec3(0, 1000, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)));

	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / (float)height, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Setup my stuff here...
	//m_engine = new PhysicsEngine(glm::vec3(0, -9.81, 0)); //Reg
	m_engine = new PhysicsEngine(glm::vec3(0, 0, 0));	//Zero G
	m_engine->addBall(glm::vec3(0, 10, 10.f), 5, 1);
	m_engine->addBall(glm::vec3(0, 10, -5), 5, 1);
	m_engine->m_actors[0]->m_colour = glm::vec4(1, 1, 1, .75f);
	

		
		
	m_engine->debugVolumes();

	return true;
}

void PhysicsCustom::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	m_engine->onUpdate(a_deltaTime);

	if (glfwGetKey(m_window, GLFW_KEY_F) == GLFW_PRESS)
	{
		m_engine->m_actors[0]->applyForceToActor(m_engine->m_actors[1], glm::vec3(0, 0, -2.0f));
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void PhysicsCustom::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//draw all da things
	m_engine->onDraw();

	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);

	// get window dimensions for 2D orthographic projection
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);

	Gizmos::draw2D(glm::ortho<float>(0, width, 0, height, -1.0f, 1.0f));

}

void PhysicsCustom::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new PhysicsCustom();
	
	if (app->create("AIE - PhysicsCustom",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}