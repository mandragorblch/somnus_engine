#pragma once
#include <physx/PxPhysics.h>
#include <physx/PxPhysicsAPI.h>
//#include <vector>
#include "vulkan/vk.h"
#include "objects/types/types.h"
#include <boost/pool/pool_alloc.hpp>

class ContactReportCallback : public physx::PxSimulationEventCallback
{
    void onConstraintBreak(physx::PxConstraintInfo* /*constraints*/, physx::PxU32 /*count*/)								PX_OVERRIDE;

    void onWake(physx::PxActor** /*actors*/, physx::PxU32 /*count*/)														PX_OVERRIDE;

	void onSleep(physx::PxActor** /*actors*/, physx::PxU32 /*count*/)														PX_OVERRIDE;

	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)															PX_OVERRIDE;

    void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32)							PX_OVERRIDE;

    void onContact(const physx::PxContactPairHeader& /*pairHeader*/, const physx::PxContactPair* pairs, physx::PxU32 count)	PX_OVERRIDE;
};

class PhysX
{
private:
	class vk* p_vk = nullptr;
public:
	physx::PxDefaultAllocator mDefaultAllocatorCallback;
	physx::PxDefaultErrorCallback mDefaultErrorCallback;
	physx::PxDefaultCpuDispatcher* mDispatcher = NULL;
	physx::PxTolerancesScale mTolerancesScale;
	physx::PxFoundation* mFoundation = NULL;
	physx::PxPhysics* mPhysics = NULL;
	ContactReportCallback gContactReportCallback;

	physx::PxScene* mScene = NULL;
	physx::PxMaterial* mMaterial = NULL;

	physx::PxPvd* mPVD = NULL;

	std::vector<physx::PxShape*>* shapes = new std::vector<physx::PxShape*>();
	std::vector<physx::PxRigidDynamic*>* rigids_dynamic = new std::vector<physx::PxRigidDynamic*>();
	std::vector<physx::PxRigidStatic*>* rigids_static = new std::vector<physx::PxRigidStatic*>();

	std::vector<physx::PxActor*> removed_actors{};
	std::vector<physx::PxActor*> added_actors{};

	PhysX(class vk* p_vk);
	void init();
	void createSimulation();
	void prepareNextFrame();
	void run();

	void fetchObjectInfo();
};
