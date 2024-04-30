#pragma once
#ifndef GAME_OBJECT
#define GAME_OBJECT

#include "glm/glm.hpp"
#include "render_objects/model.h"
#include "vulkan/vk.h"
#include "PhysX/PhysX_glm_conversion.h"
#include "misc/simple_obj_loader.h"
#include "objects/types/types.h"
#include <queue>
#include <list>
#include <boost/pool/pool_alloc.hpp>

extern enum class TYPE;

class gameObject
{
public:
	unsigned int particlesAmount = 0;
	glm::vec3 pos = glm::vec3(0);
	class mesh* p_mesh = nullptr;
	std::vector<indirectBatch*> local_batches{};
	std::list<uint32_t, boost::fast_pool_allocator<uint32_t>> PO_global_indexes;
	std::list<uint32_t, boost::fast_pool_allocator<uint32_t>> unusedIndexes;
	unsigned int firstIndexMesh = 0;
	uint32_t meshSize = 0;

	uint32_t* amount = new uint32_t(0);

	std::vector<physx::PxShape*> shapes;
	std::vector<physx::PxTransform*> transforms;
	std::vector<physx::PxRigidActor*> actors;
	float mass;

	void createShape(const physx::PxVec3& sides, float mass, TYPE  filter_type, TYPE  filter_mask, bool isTrigger = false);
	void createShape(float radius, float mass, TYPE filter_type, TYPE  filter_mask, bool isTrigger = false);
	void createShape(std::string path, float mass, TYPE  filter_type, TYPE  filter_mask, bool isTrigger = false);
	void createShape(std::string path, TYPE  filter_type, TYPE  filter_mask, bool isTrigger = false);
	int getLocal_ID();
	int getGlobal_ID();
	void addDynamicInstance(class Object* obj, const physx::PxVec3& pos = physx::PxVec3(0), const physx::PxQuat& quat = physx::PxQuat(0, 0, 0, 1), const physx::PxVec3& linear_velocity = physx::PxVec3(0), const physx::PxVec3& angular_velocity = physx::PxVec3(0), float lifeTime = -1.0f);
	void addStaticInstance(class Object* obj, const physx::PxVec3& pos = physx::PxVec3(0), const physx::PxQuat& quat = physx::PxQuat(0, 0, 0, 1), const physx::PxVec3& linear_velocity = physx::PxVec3(0), const physx::PxVec3& angular_velocity = physx::PxVec3(0), float lifeTime = -1.0f);
	void removeInstance(class Object* obj);
	void removeInstance(class Object* obj, auto iter);

	/**
	*\brief pos_index is a center position
	*\brief p_mesh is a pointer to some mesh(particle 3d object)
	*/
	gameObject(class model* p_model, class init_storage* p_storage, class vk* p_vk, class PhysX* p_PhysX);
	gameObject(class init_storage* p_storage, class vk* p_vk, class PhysX* p_PhysX);
	gameObject() = default;
	~gameObject();

//private:
	class init_storage* p_storage;
	class vk* p_vk;
	class PhysX* p_PhysX;
};

#endif