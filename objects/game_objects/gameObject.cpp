#include "gameObject.h"

gameObject::gameObject(class model* p_model, class init_storage* p_storage, class vk* p_vk, class PhysX* p_PhysX): p_storage(p_storage), p_vk(p_vk), p_PhysX(p_PhysX), meshSize(p_model->meshes.size()) {
	firstIndexMesh = p_model->first_mesh_id;

	indirectBatch* current_batch = new indirectBatch{};
	current_batch->p_model = p_model;
	current_batch->instance_count = amount;
	current_batch->mesh_count = meshSize;
	current_batch->firstMeshIndex = firstIndexMesh;
	current_batch->PO_global_indexes = &PO_global_indexes;

	p_vk->draws.push_back(current_batch);
}

gameObject::gameObject(init_storage* p_storage, vk* p_vk, PhysX* p_PhysX): p_storage(p_storage), p_vk(p_vk), p_PhysX(p_PhysX)
{

}

void gameObject::createShape(const physx::PxVec3& sides, float mass, TYPE  filter_type, TYPE  filter_mask, bool isTrigger) {
	this->mass = mass;
	physx::PxTransform t(physx::PxVec3(0.0f));
	physx::PxShape* shape = p_PhysX->mPhysics->createShape(physx::PxBoxGeometry(sides.x, sides.y, sides.z), *p_PhysX->mMaterial);
	shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
	const physx::PxFilterData triggerFilterData(static_cast<int>(filter_type), static_cast<int>(filter_mask), 0, 0);
	shape->setSimulationFilterData(triggerFilterData);
	//physx::PxRigidDynamic* box = p_PhysX->mPhysics->createRigidDynamic(t);
	//box->attachShape(*shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*box, mass);
	//p_PhysX->mScene->addActor(*box);
	//shape->release();
	//dynamics.push_back(box);
	//p_PhysX->shapes->push_back(shape);
	shapes.push_back(shape);
}

void gameObject::createShape(float radius, float mass, TYPE filter_type, TYPE filter_mask, bool isTrigger) {
	this->mass = mass;
	physx::PxTransform t(physx::PxVec3(0.0f));
	physx::PxShape* shape = p_PhysX->mPhysics->createShape(physx::PxSphereGeometry(radius), *p_PhysX->mMaterial, true);
	if (mass == 0.0f) {
		shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
		shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		//shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);
	}
	const physx::PxFilterData triggerFilterData(static_cast<int>(filter_type), static_cast<int>(filter_mask), 0, 0);
	shape->setSimulationFilterData(triggerFilterData);
	//physx::PxRigidDynamic* sphere = p_PhysX->mPhysics->createRigidDynamic(t);
	//sphere->attachShape(*shape);
	//physx::PxRigidBodyExt::updateMassAndInertia(*sphere, mass);
	//p_PhysX->mScene->addActor(*sphere);
	//shape->release();
	//dynamics.push_back(sphere);
	//p_PhysX->shapes->push_back(shape);
	shapes.push_back(shape);
}

void gameObject::createShape(std::string path, float mass, TYPE filter_type, TYPE filter_mask, bool isTrigger) {
	this->mass = mass;

	std::vector<physx::PxVec3> convexVerts;

	load_collider_from_file(path, &convexVerts);

	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = convexVerts.size();
	convexDesc.points.stride = sizeof(physx::PxVec3);
	convexDesc.points.data = convexVerts.data();
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	physx::PxTolerancesScale scale;
	physx::PxCookingParams params(scale);

	physx::PxConvexMesh* convexMesh = nullptr;
	physx::PxDefaultMemoryOutputStream outBuffer;
	PxCookConvexMesh(params, convexDesc, outBuffer);
	physx::PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
	convexMesh = p_PhysX->mPhysics->createConvexMesh(stream);

	physx::PxConvexMeshGeometry convexGeometry;
	convexGeometry.convexMesh = convexMesh;
	physx::PxShape* convexShape = p_PhysX->mPhysics->createShape(convexGeometry, *p_PhysX->mMaterial);
	const physx::PxFilterData triggerFilterData(static_cast<int>(filter_type), static_cast<int>(filter_mask), 0, 0);
	convexShape->setSimulationFilterData(triggerFilterData);
	convexShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);

	//p_PhysX->shapes->push_back(convexShape);
	shapes.push_back(convexShape);
	//physx::PxRigidDynamic* convex = p_PhysX->mPhysics->createRigidDynamic(t);
	//p_PhysX->rigids_dynamic->push_back(convex);
	//dynamics.push_back(convex);
	//convex->attachShape(*convexShape);

	//physx::PxVec3 massLocalePose = physx::PxVec3(3, 0, 0);
	//physx::PxRigidBodyExt::updateMassAndInertia(*convex, 1.f);
	//convexShape->release();

	//p_PhysX->mScene->addActor(*convex);
}

void gameObject::createShape(std::string path, TYPE filter_type, TYPE filter_mask, bool isTrigger)
{
	this->mass = 0.0f;

	std::vector<physx::PxVec3> triangleVerts;

	load_collider_from_file(path, &triangleVerts);

	physx::PxTriangleMeshDesc triangleDesc;
	triangleDesc.points.count = triangleVerts.size();
	triangleDesc.points.stride = sizeof(physx::PxVec3);
	triangleDesc.points.data = triangleVerts.data();
	triangleDesc.flags/* = physx::PxTriangleMeshFlag::*/;

	physx::PxTolerancesScale scale;
	physx::PxCookingParams params(scale);

	physx::PxTriangleMesh* triangleMesh = nullptr;
	physx::PxDefaultMemoryOutputStream outBuffer;
	PxCookTriangleMesh(params, triangleDesc, outBuffer);
	physx::PxDefaultMemoryInputData stream(outBuffer.getData(), outBuffer.getSize());
	triangleMesh = p_PhysX->mPhysics->createTriangleMesh(stream);

	physx::PxTriangleMeshGeometry triangleGeometry;
	triangleGeometry.triangleMesh = triangleMesh;
	physx::PxShape* triangleShape = p_PhysX->mPhysics->createShape(triangleGeometry, *p_PhysX->mMaterial);
	const physx::PxFilterData triggerFilterData(static_cast<int>(filter_type), static_cast<int>(filter_mask), 0, 0);
	triangleShape->setSimulationFilterData(triggerFilterData);
	triangleShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, isTrigger);

	shapes.push_back(triangleShape);
}

int gameObject::getLocal_ID()
{
	if (unusedIndexes.size()) {
		int ID = unusedIndexes.front();
		unusedIndexes.pop_front();
		return ID;
	}
	else {
		return (actors.size());
	}
}

int gameObject::getGlobal_ID() {
	if (p_vk->unusedPO.size()) {
		int PO_index = p_vk->unusedPO.front();
		p_vk->unusedPO.pop_front();
		++p_vk->physical_object_amount;
		PO_global_indexes.push_back(PO_index);
		return PO_index;
	}
	else {
		p_vk->physicalObjectsInfo.emplace_back(physicalObjectInfo{});
		PO_global_indexes.push_back(p_vk->physicalObjectsInfo.size() - 1);
		++p_vk->physical_object_amount;
		return (p_vk->physicalObjectsInfo.size() - 1);
	}
}

void gameObject::addDynamicInstance(class Object* obj, const physx::PxVec3& pos, const physx::PxQuat& quat, const physx::PxVec3& linear_velocity, const physx::PxVec3& angular_velocity, float lifeTime)
{
	int local_ID = obj->local_ID;
	int global_ID = obj->global_ID;
	physicalObjectInfo& current_info = p_vk->physicalObjectsInfo[global_ID];

	physx::PxTransform t{ pos, quat };
	physx::PxRigidDynamic* copy = p_PhysX->mPhysics->createRigidDynamic(t);

	++(*amount);
	copy->setAngularVelocity(angular_velocity, false);
	copy->setLinearVelocity(linear_velocity, false);
	
	physx::PxShape* shape_copy = p_PhysX->mPhysics->createShape(shapes[0]->getGeometry(), *p_PhysX->mMaterial);
	shape_copy->userData = obj;
	shape_copy->setSimulationFilterData(shapes[0]->getSimulationFilterData());
	shape_copy->setFlags(shapes[0]->getFlags());
	copy->attachShape(*(shape_copy));
	shape_copy->release();
	if(mass == 0.0f){
		copy->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	}

	physx::PxRigidBodyExt::updateMassAndInertia(*copy, mass);
	p_PhysX->added_actors.push_back(copy);

	current_info.type = PO_TYPE::TRUE_PO;
	current_info.lifeTime = lifeTime;
	current_info.should_be_deleted = false;
	//physicalObjectsInfo->push_back(currentPOI);//rewrite to PO of GO(one GO can has a several PO child-parent)

	if(meshSize)
	{
		p_vk->render_object_amount += meshSize;
		p_vk->draw_object_count_changed = true;
	}
	if (actors.size() == local_ID) {
		actors.push_back(copy);
	} else {
		actors[local_ID] = copy;
		//print_r(ID);
	}
}

void gameObject::addStaticInstance(Object* obj, const physx::PxVec3& pos, const physx::PxQuat& quat, const physx::PxVec3& linear_velocity, const physx::PxVec3& angular_velocity, float lifeTime)
{
	int ID = obj->local_ID;
	int global_ID = obj->global_ID;
	physicalObjectInfo& current_info = p_vk->physicalObjectsInfo[global_ID];

	physx::PxTransform t{ pos, quat };
	physx::PxRigidStatic* copy = p_PhysX->mPhysics->createRigidStatic(t);

	++(*amount);
	//copy->setAngularVelocity(angular_velocity, false);
	//copy->setLinearVelocity(linear_velocity, false);

	physx::PxShape* shape_copy = p_PhysX->mPhysics->createShape(shapes[0]->getGeometry(), *p_PhysX->mMaterial);
	shape_copy->userData = obj;
	shape_copy->setSimulationFilterData(shapes[0]->getSimulationFilterData());
	shape_copy->setFlags(shapes[0]->getFlags());
	copy->attachShape(*(shape_copy));
	shape_copy->release();
	if (mass == 0.0f) {
		copy->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	}

	//physx::PxRigidBodyExt::updateMassAndInertia(*copy, mass);
	p_PhysX->added_actors.push_back(copy);

	current_info.type = PO_TYPE::TRUE_PO;
	current_info.lifeTime = lifeTime;
	current_info.should_be_deleted = false;
	//physicalObjectsInfo->push_back(currentPOI);//rewrite to PO of GO(one GO can has a several PO child-parent)

	if (meshSize)
	{
		p_vk->render_object_amount += meshSize;
		p_vk->draw_object_count_changed = true;
	}
	if (actors.size() == ID) {
		actors.push_back(copy);
	}
	else {
		actors[ID] = copy;
	}
}

void gameObject::removeInstance(class Object* obj)
{
	int local_ID = obj->local_ID;
	int global_ID = obj->global_ID;
	unusedIndexes.push_back(local_ID);
	p_vk->unusedPO.push_back(global_ID);
	--p_vk->physical_object_amount;

	if (auto iter = std::find(PO_global_indexes.begin(), PO_global_indexes.end(), global_ID); iter != PO_global_indexes.end()) {
		PO_global_indexes.erase(iter);
	}
	else {
		assert(0);
	}
	--(*amount);

	p_PhysX->removed_actors.push_back(actors[local_ID]);

	if (meshSize)
	{
		p_vk->render_object_amount -= meshSize;
		p_vk->draw_object_count_changed = true;
	}
}

void gameObject::removeInstance(class Object* obj, auto iter)
{
	int local_ID = obj->local_ID;
	int global_ID = obj->global_ID;
	unusedIndexes.push_back(local_ID);
	p_vk->unusedPO.push_back(global_ID);
	--p_vk->physical_object_amount;
	PO_global_indexes.erase(iter);

	--(*amount);

	p_PhysX->removed_actors.push_back(actors[local_ID]);

	if (meshSize)
	{
		p_vk->render_object_amount -= meshSize;
		p_vk->draw_object_count_changed = true;
	}
}

gameObject::~gameObject()
{
	for (indirectBatch* batch : local_batches) {
		delete batch;
	}
}
