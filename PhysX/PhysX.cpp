#include "PhysX.h"
#include "PhysX/PhysX_glm_conversion.h"
#include <list>

// Detects a trigger using the shape's simulation filter data. See createTriggerShape() function.
bool inline isTrigger(const physx::PxFilterData& data)
{
    if (data.word0 != 0xffffffff)
        return false;
    if (data.word1 != 0xffffffff)
        return false;
    if (data.word2 != 0xffffffff)
        return false;
    if (data.word3 != 0xffffffff)
        return false;
    return true;
}

physx::PxFilterFlags triggersUsingFilterShader(physx::PxFilterObjectAttributes /*attributes0*/, physx::PxFilterData filterData0,
    physx::PxFilterObjectAttributes /*attributes1*/, physx::PxFilterData filterData1,
    physx::PxPairFlags& pairFlags, const void* /*constantBlock*/, physx::PxU32 /*constantBlockSize*/)
{
    //	printf("contactReportFilterShader\n");

    // We need to detect whether one of the shapes is a trigger.
    const bool isTriggerPair = isTrigger(filterData0) || isTrigger(filterData1);

    // If we have a trigger, replicate the trigger codepath from PxDefaultSimulationFilterShader
    if (isTriggerPair)
    {
        pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
        return physx::PxFilterFlag::eDEFAULT;
    }

    // Otherwise use the default flags for regular pairs
    pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

    if ((filterData0.word0 & filterData1.word1) && (filterData0.word1 & filterData1.word0)) {
        pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
    }

    return physx::PxFilterFlag::eDEFAULT;
}

void ContactReportCallback::onConstraintBreak(physx::PxConstraintInfo* /*constraints*/, physx::PxU32 /*count*/) {
    printf("onConstraintBreak\n");
}

void ContactReportCallback::onWake(physx::PxActor** /*actors*/, physx::PxU32 /*count*/) {
    printf("onWake\n");
}

void ContactReportCallback::onSleep(physx::PxActor** /*actors*/, physx::PxU32 /*count*/)
{
    printf("onSleep\n");
}

void ContactReportCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
    printf("onTrigger: %d trigger pairs\n", count);
    while (count--)
    {
        const physx::PxTriggerPair& current = *pairs++;
        void* temp_data;
        temp_data = current.triggerShape->userData;
        class Object* trigger = reinterpret_cast<class Object*>(temp_data);
        temp_data = current.otherShape->userData;
        class Object* other = reinterpret_cast<class Object*>(temp_data);

        // The reported pairs can be trigger pairs or not. We only enabled contact reports for
        // trigger pairs in the filter shader, so we don't need to do further checks here. In a
        // real-world scenario you would probably need a way to tell whether one of the shapes
        // is a trigger or not. You could e.g. reuse the PxFilterData like we did in the filter
        // shader, or maybe use the shape's userData to identify triggers, or maybe put triggers
        // in a hash-set and test the reported shape pointers against it. Many options here.

        if (current.status & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND /*| physx::PxPairFlag::eNOTIFY_TOUCH_CCD*/)) {
            if (other) {
                printf("Shape %d is overlapping with %d\n\n", other->global_ID, trigger->global_ID);
            }
            else {
                printf("Shape UNDEFINED is overlapping\n\n");
            }
            if (trigger && other) {
                if ((trigger->type & static_cast<int>(TYPE::Damaging)) && (other->type & static_cast<int>(TYPE::Damagable))) {
                    printf("hp: %d\n", other->hp);
                    other->takeDamage(trigger->damage);
                }
            }
        }

        if (current.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            if (other) {
                printf("Shape %d stopped overlap\n\n", other->global_ID);
            }
            else {
                printf("Shape UNDEFINED is overlapping\n\n");
            }
        }
    }
}

void ContactReportCallback::onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32)
{
    printf("onAdvance\n");
}

void ContactReportCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 count)
{
    printf("onContact: %d pairs\n", count);
    while (count--)
    {
        const physx::PxContactPair& current = *pairs++;
        void* temp_data;
        temp_data = current.shapes[0]->userData;
        class Object* obj1 = reinterpret_cast<class Object*>(temp_data);
        temp_data = current.shapes[1]->userData;
        class Object* obj2 = reinterpret_cast<class Object*>(temp_data);

        // The reported pairs can be trigger pairs or not. We only enabled contact reports for
        // trigger pairs in the filter shader, so we don't need to do further checks here. In a
        // real-world scenario you would probably need a way to tell whether one of the shapes
        // is a trigger or not. You could e.g. reuse the PxFilterData like we did in the filter
        // shader, or maybe use the shape's userData to identify triggers, or maybe put triggers
        // in a hash-set and test the reported shape pointers against it. Many options here.

        if (current.events & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND /*| physx::PxPairFlag::eNOTIFY_TOUCH_CCD*/)) {
            if (obj1) {
                printf("Shape %d is in touch\n", obj1->global_ID);
            }
            else {
                printf("Shape UNDEFINED is in touch\n");
            }
            if (obj2) {
                printf("Shape %d is in touch\n\n", obj2->global_ID);
            }
            else {
                printf("Shape UNDEFINED is in touch\n\n");
            }
            if (obj1 && obj2) {
                if ((obj1->type & static_cast<int>(TYPE::Damagable)) && (obj2->type & static_cast<int>(TYPE::Damaging))) {
                    printf("hp: %d\n", obj1->hp);
                    obj1->takeDamage(obj2->damage);
                }
                else if ((obj1->type & static_cast<int>(TYPE::Damaging)) && (obj2->type & static_cast<int>(TYPE::Damagable))) {
                    printf("hp: %d\n", obj2->hp);
                    obj2->takeDamage(obj1->damage);
                }
            }
        }
        if (current.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            if (obj1) {
                printf("Shape %d lost touch\n", obj1->global_ID);
            }
            else {
                printf("Shape UNDEFINED is in touch\n");
            }
            if (obj2) {
                printf("Shape %d lost touch\n\n", obj2->global_ID);
            }
            else {
                printf("Shape UNDEFINED is in touch\n\n");
            }
        }

        if (isTrigger(current.shapes[0]->getSimulationFilterData()) && isTrigger(current.shapes[1]->getSimulationFilterData()))
            printf("Trigger-trigger overlap detected\n");
    }
}

PhysX::PhysX(class vk* p_vk) : p_vk(p_vk) {

}

void PhysX::init()
{
    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
    if (!mFoundation) throw("PxCreateFoundation failed!");
    mPVD = PxCreatePvd(*mFoundation);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
    mPVD->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
    //mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale(),true, mPvd);
    mTolerancesScale.length = 1;        // typical length of an object
    mTolerancesScale.speed = 9.81;         // typical speed of an object, gravity*1s is a reasonable choice
    mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mTolerancesScale, true, mPVD);
    //mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale);

    physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    mDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = mDispatcher;
    sceneDesc.filterShader = /*physx::PxDefaultSimulationFilterShader*/triggersUsingFilterShader;
    sceneDesc.simulationEventCallback = &gContactReportCallback;
    mScene = mPhysics->createScene(sceneDesc);

    physx::PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
}

void PhysX::createSimulation()
{

}

void PhysX::prepareNextFrame()
{
    mScene->removeActors(removed_actors.data(), removed_actors.size());
    removed_actors.clear();
    mScene->addActors(added_actors.data(), added_actors.size());
    added_actors.clear();
}

void PhysX::run()
{
    prepareNextFrame();
    mScene->simulate(1.0f / 60.0f);
    mScene->fetchResults(true);
}

void PhysX::fetchObjectInfo()
{
    decltype(p_vk->p_init_storage->objects)& objects = p_vk->p_init_storage->objects;
    if (objects.size() > 0) {
        auto iterator = objects.begin();
        while (iterator != objects.end()) {//FIX THIS SHIT
            class Object* current = *iterator;
            if (current->should_be_deleted)
            {
                switch (current->obj_ID) {
                case OBJECT_ID::Bomb:
                    reinterpret_cast<Bomb*>(current)->explode();
                    break;
                }
                delete (current);
                objects.erase(iterator++);
            }
            else {
                if (p_vk->physicalObjectsInfo.size() > 0 && current->global_ID >= 0 && current->p_GO->actors.size() > 0 && current->local_ID >= 0) {
                    physicalObjectInfo& current_objectInfo = p_vk->physicalObjectsInfo[current->global_ID];
                    physx::PxTransform current_transform = current->p_GO->actors[current->local_ID]->getGlobalPose();
                    current_objectInfo.quat_rot = PxQuat_to_vec4_xyzw(current_transform.q);
                    current_objectInfo.pos_index.x = current_transform.p.x;
                    current_objectInfo.pos_index.y = current_transform.p.y;
                    current_objectInfo.pos_index.z = current_transform.p.z;
                    current->should_be_deleted = current_objectInfo.should_be_deleted;
                }
                ++iterator;
            }
        }
    }
}
