#pragma once
#ifndef PARTICLE_OBJ
#define PARTICLE_OBJ

#include "misc/vk_misc.h"
#include "render_objects/mesh.h"
#include "vulkan/vk.h"
#include <list>

class particle
{
public:
	uint32_t* particlesAmount = new uint32_t(0);
	typedef std::list<uint32_t, boost::fast_pool_allocator<uint32_t>> PO_global_indexes_t;
	std::list<PO_global_indexes_t*, boost::fast_pool_allocator<PO_global_indexes_t*>> cluster_PO_global_indexes;
	class model* p_model = nullptr;
	indirectBatchCluster* local_batch;
	unsigned int firstIndexMesh = 0;
	std::list<clusterInfo, boost::fast_pool_allocator<clusterInfo>> cluster_info;
	typedef decltype(cluster_info)::iterator cluster_it_t;
	//const std::unordered_set<float> hashed_rand = {
	//	static_cast<float>(rand()) / RAND_MAX * 2 - 1,
	//	static_cast<float>(rand()) / RAND_MAX * 2 - 1,
	//	static_cast<float>(rand()) / RAND_MAX * 2 - 1,
	//	static_cast<float>(rand()) / RAND_MAX * 2 - 1,
	//	static_cast<float>(rand()) / RAND_MAX * 2 - 1
	//};
	//unsigned int rand_index = 0;

	/**
	*\brief pos_index is a center position
	*\brief p_mesh is a pointer to some mesh(particle 3d object)
	*/
	uint32_t calcGlobal_ID(cluster_it_t clust_it);
	void calcGlobal_IDs(cluster_it_t clust_it, uint32_t cluster_size);
	cluster_it_t calcClusterID(uint32_t clusterSize);
	void removeInstance(cluster_it_t clust_it, uint32_t global_ID);
	void removeInstance(cluster_it_t clust_it, PO_global_indexes_t::iterator PO_it);
	void removeCluster(cluster_it_t clust_it);

	particle(class model* p_model, class vk* p_vk);
	particle() = default;
	~particle();

private:
	class vk* p_vk;
};

#endif