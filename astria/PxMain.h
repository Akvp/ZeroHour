#ifndef _PX_MAIN_H_
#define _PX_MAIN_H_

#include "Common.h"
#include "utils.h"

#include <PxPhysicsAPI.h>
#include <extensions\PxExtensionsAPI.h>
#include <extensions\PxDefaultAllocator.h>
#include <extensions\PxDefaultErrorCallback.h>
#include <extensions\PxDefaultSimulationFilterShader.h>
#include <extensions\PxShapeExt.h>
#include <extensions\PxSimpleFactory.h>
#include <foundation\PxFoundation.h>

#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysX3Cooking_x86.lib")
#pragma comment(lib, "PxTask.lib")

using namespace physx;

class CPhysx
{
public:
	CPhysx();
	~CPhysx();

	bool OnInitialize();
	void OnStep();
	void OnRender();

	void OnShutdown();

private:
	//basic objects
	PxFoundation* mFoundation;
	static PxDefaultAllocator gDefaultAllocatorCallback;
	static PxDefaultErrorCallback gDefaultErrorCallback;
	PxPhysics* mPhysics;
	PxCooking* mCooking;

	//Scene
	PxScene* mScene;
	PxSceneDesc* mSceneDescriptor;
	static PxSimulationFilterShader gDefaultFilterShader;
	PxDefaultCpuDispatcher* mCpuDispatcher;

	//settings
	bool mRecordMemoryAllocations;
	int mNumThreads;
};


#endif