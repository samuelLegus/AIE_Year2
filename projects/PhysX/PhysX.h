#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>

using namespace physx;

// derived application class that wraps up all globals neatly
class PhysX : public Application
{
public:

	PhysX();
	virtual ~PhysX();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	//TUT1
	void addWidget(PxShape* shape, PxRigidDynamic* actor);
	void addBox(PxShape * pShape, PxRigidDynamic * actor);
	void addSphere(PxShape * pShape, PxRigidDynamic * actor);
	void addGrid(const glm::vec3& a_center, const glm::mat4* a_transform, int increments, float size);
	//TUT2
	void setupPhysX();
	void enableSphereGun();



	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

//FUUUCCKKK
class myAllocator : public PxAllocatorCallback
{
public:
	virtual ~myAllocator() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
	{
		void* pointer = _aligned_malloc(size, 16);
		return pointer;
	}
	virtual void deallocate(void* ptr)
	{
		_aligned_free(ptr);
	}
};