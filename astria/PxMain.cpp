#include "PxMain.h"

PxDefaultAllocator CPhysx::gDefaultAllocatorCallback;
PxDefaultErrorCallback CPhysx::gDefaultErrorCallback;
PxSimulationFilterShader CPhysx::gDefaultFilterShader;

CPhysx::CPhysx()
{
	mFoundation = NULL;
	gDefaultFilterShader = PxDefaultSimulationFilterShader;

	//Initialize settings
	mRecordMemoryAllocations = true;
	mNumThreads = 1;
}

CPhysx::~CPhysx()
{

}

bool CPhysx::OnInitialize()
{
	//Title of all error and warning window in this method
	const std::string LOG_TAG = "PhysX OnInitialize Error";
	
	//Create the foundation
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!mFoundation)
	{
		Error(LOG_TAG, "PxCreateFoundation failed!");
		return false;
	}

	//Create a PxPhysics object with no profile zone manager
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(), mRecordMemoryAllocations);

	//Initialize the cooking library
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));

	//Initialize extensions
	if (!PxInitExtensions(*mPhysics))
	{
		Error(LOG_TAG, "PxInitExtensions failed!");
		return false;
	}

	//Initialize scene
	mSceneDescriptor = new PxSceneDesc(mPhysics->getTolerancesScale());
	mSceneDescriptor->gravity = PxVec3(0.0f, -9.81f, 0.0f);
	// ===>customize mSceneDescriptor here<===

	if (!mSceneDescriptor->cpuDispatcher)
	{
		mCpuDispatcher = PxDefaultCpuDispatcherCreate(mNumThreads);
		if (!mCpuDispatcher)
		{
			Error(LOG_TAG, "PxDefaultCpuDispatcherCreate failed!");
			return false;
		}
		mSceneDescriptor->cpuDispatcher = mCpuDispatcher;
	}
	if (!mSceneDescriptor->filterShader)
	{
		mSceneDescriptor->filterShader = gDefaultFilterShader;
	}
#ifdef PX_WINDOWS
	//Set gpu dispatcher for CUDA-accelerated features
	//if (!mSceneDescriptor->gpuDispatcher && mCudaContextManager)
	//{
	//	mSceneDescriptor->gpuDispatcher = mCudaContextManager->getGpuDispatcher();
	//}
#endif

	mScene = mPhysics->createScene(*mSceneDescriptor);
	if (!mScene)
	{
		Error(LOG_TAG, "createScene failed!");
		return false;
	}

	return true;
}

void CPhysx::OnShutdown()
{
	mPhysics->release();
	mFoundation->release();
}