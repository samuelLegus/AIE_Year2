#include "PhysX.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <vector>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

//globals from tutorial

PxFoundation * g_PhysicsFoundation = nullptr;	// Singleton class allows us to access high level SDK
PxPhysics * g_Physics = nullptr;				// Singleton class acts as a factory for object creation, provides access to globals that effect scenes
PxScene * g_PhysicsScene = nullptr;				// Where stuff happens

//to be explained later -> 
PxDefaultErrorCallback gDefaultErrorCallback;
PxDefaultAllocator gDefaultAllocatorCallback;
PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
PxMaterial * g_PhysicsMaterial = nullptr;
PxCooking * g_PhysicsCooker = nullptr;

std::vector<PxRigidDynamic *> g_PhysXActors;	// Local copy of our PhysX Scene


PhysX::PhysX()
{

}

PhysX::~PhysX()
{

}

bool PhysX::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,100,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );

	// get window dimensions to calculate aspect ratio
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);

	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, width / (float)height, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Video tutorial stuff
	setupPhysX();
	

	//UHHH?
	//add a plane
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi*0.95f, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(), *g_PhysicsMaterial);
	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);

	//add a box
	float density = 10;
	PxBoxGeometry box(2, 2, 2);
	PxTransform transform(PxVec3(0, 20, 0));
	PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box, *g_PhysicsMaterial, density);
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
	//add it to our copy of the scene
	g_PhysXActors.push_back(dynamicActor);

	//add sphere
	PxSphereGeometry sphere(2);	//I think the ctor takes a radius? Can't tell
								//API for the ctor has a PxReal param, far as I can tell PxReal can be an integer or floating point value as well...
	PxTransform sphereTransform(PxVec3(5, 20, 0));
	PxRigidDynamic * sphereDynamicActor = PxCreateDynamic(*g_Physics, sphereTransform, sphere, *g_PhysicsMaterial, density);
	g_PhysicsScene->addActor(*sphereDynamicActor);
	g_PhysXActors.push_back(sphereDynamicActor);

	return true;
}

void PhysX::onUpdate(float a_deltaTime) 
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
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();

	//PHYSX
	g_PhysicsScene->simulate(1 / 60.f);
	while (g_PhysicsScene->fetchResults() == false)
	{
		// don’t need to do anything here yet but we still need to do the fetch
	}

	enableSphereGun();

	for (auto actor : g_PhysXActors)
	{
		PxU32 nShapes = actor->getNbShapes();
		PxShape** shapes = new PxShape*[nShapes];
		actor->getShapes(shapes, nShapes); 
		while (nShapes--)
		{
			addWidget(shapes[nShapes], actor);
		}

	}
}

void PhysX::onDraw() 
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
	Gizmos::draw2D(glm::ortho<float>(0, width, 0, height, -1.0f, 1.0f));
}

void PhysX::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();

	//PHYSX
	g_PhysicsCooker->release();
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new PhysX();
	
	if (app->create("AIE - PhysX",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}

void PhysX::addWidget(PxShape* shape, PxRigidDynamic* actor)
{
	PxGeometryType::Enum type = shape->getGeometryType();
	switch (type)
	{
	case PxGeometryType::eBOX:
		addBox(shape, actor);
		break;
	case PxGeometryType::eSPHERE:
		addSphere(shape,actor);
		break;
	}
}

void PhysX::addBox(PxShape * pShape, PxRigidDynamic * actor)
{
	PxBoxGeometry geometry; 
	float width = 1, height = 1, length = 1;
	bool status = pShape->getBoxGeometry(geometry);
	if (status)
	{
		width = geometry.halfExtents.x;
		height = geometry.halfExtents.y;
		length = geometry.halfExtents.z;
	}
	//get the transform for this PhysX collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*pShape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
		m.column1.x, m.column1.y, m.column1.z, m.column1.w,
		m.column2.x, m.column2.y, m.column2.z, m.column2.w,
		m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	//get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec3 extents = glm::vec3(width, height, length);
	glm::vec4 colour(1, 0, 0, 1); 
	//create da box
	Gizmos::addAABBFilled(position, extents, colour, &M);
}

void PhysX::addSphere(PxShape * pShape, PxRigidDynamic * actor)
{
	PxSphereGeometry geometry;
	float radius = 1;
	bool status = pShape->getSphereGeometry(geometry);
	if (status)
	{
		radius = geometry.radius;
	}

	//get the transform for this PhysX collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*pShape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
		m.column1.x, m.column1.y, m.column1.z, m.column1.w,
		m.column2.x, m.column2.y, m.column2.z, m.column2.w,
		m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	//get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec4 colour(1, 0, 0, 1);
	//create da box
	Gizmos::addSphere(position, radius, 10, 10, colour, &M);
}

void PhysX::addGrid(const glm::vec3& a_center, const glm::mat4* a_transform, int increments, float size)
{
	int halfIncrements = increments / 2;
	for (int i = 0; i < increments + 1; ++i)
	{
		glm::vec4 colour = i == halfIncrements ? glm::vec4(1, 0, 1, 1) : glm::vec4(0, 0, 0, 1);
		glm::vec3 l1Start = glm::vec3((-halfIncrements + i)*size, 0, halfIncrements*size);
		glm::vec3 l1End = glm::vec3((-halfIncrements + i)*size, 0, -halfIncrements*size);
		glm::vec3 l2Start = glm::vec3(halfIncrements*size, 0, (-halfIncrements + i)*size);
		glm::vec3 l2End = glm::vec3(-halfIncrements*size, 0, (-halfIncrements + i)*size);

		l1Start = (*a_transform * glm::vec4(l1Start, 0)).xyz + a_center;
		l1End = (*a_transform * glm::vec4(l1End, 0)).xyz + a_center;
		l2Start = (*a_transform * glm::vec4(l2Start, 0)).xyz + a_center;
		l2End = (*a_transform * glm::vec4(l2End, 0)).xyz + a_center;
		Gizmos::addLine(l1Start, l1End, colour);
		Gizmos::addLine(l2Start, l2End, colour);
	}
}

//T2

void PhysX::setupPhysX()
{
	PxAllocatorCallback * myCallback = new myAllocator();	//HOKAY GHETTO C ALLOCATOR - Have to use cause 64 bit OS
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback, gDefaultErrorCallback);	//DEFAULT ERROR SHIT
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxTolerancesScale());			//SDK?
	g_PhysicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxCookingParams(PxTolerancesScale())); //SDK?
	PxInitExtensions(*g_Physics); //SDK?
	//create physics material
	g_PhysicsMaterial = g_Physics->createMaterial(.5f, .5f, .6f); //TO BE EXPLAINED
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale()); //RENDERING YESSS
	sceneDesc.gravity = PxVec3(0, -30.0f, 0);				//...
	sceneDesc.filterShader = gDefaultFilterShader;			//...
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);	//MAKE THE CPU DO THE WORK I GUESS. EXPLAINED LATER?
	g_PhysicsScene = g_Physics->createScene(sceneDesc);		//MAKE DA SCENE

	if (g_PhysicsScene)
	{
		printf("Start PhysX scene2");						//
	}
}

void PhysX::enableSphereGun()
{
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		float density = 10;

		//add sphere
		PxSphereGeometry sphere(2);	//I think the ctor takes a radius? Can't tell
		//API for the ctor has a PxReal param, far as I can tell PxReal can be an integer or floating point value as well...

		glm::vec3 cameraPosition(m_cameraMatrix[3]); //Get the camera position using the matrix4x4 in the framework
		cameraPosition += glm::normalize(cameraPosition) * 15.0f; //Should make it so the sphere's spawn a bit in front of the camera 

		PxTransform sphereTransform(PxVec3(cameraPosition.x, cameraPosition.y, cameraPosition.z)); //convert glm::vec3 to physX vec3

		PxRigidDynamic * sphereDynamicActor = PxCreateDynamic(*g_Physics, sphereTransform, sphere, *g_PhysicsMaterial, density);
		glm::vec3 velocity = glm::normalize(cameraPosition) * 25.0f;
		sphereDynamicActor->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));

		g_PhysicsScene->addActor(*sphereDynamicActor);
		g_PhysXActors.push_back(sphereDynamicActor);
	}
}


