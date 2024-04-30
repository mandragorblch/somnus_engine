#include "particle.h"

uint32_t particle::calcGlobal_ID(cluster_it_t clust_it) {
	++(p_vk->physical_object_amount);
	uint32_t PO_index;
	if (p_vk->unusedPO.size()) {
		PO_index = p_vk->unusedPO.front();
		p_vk->unusedPO.pop_front();
		
	}
	else {
		PO_index = p_vk->physicalObjectsInfo.size();
		p_vk->physicalObjectsInfo.push_back(physicalObjectInfo{});
	}
	clust_it->PO.push_back(PO_index);
	return PO_index;
}

void particle::calcGlobal_IDs(cluster_it_t clust_it, uint32_t clusterSize) {
	p_vk->physical_object_amount += clusterSize;
	for (uint32_t counter = 0; counter < clusterSize; ++counter) {
		if (p_vk->unusedPO.size()) {
			clust_it->PO.push_back(p_vk->unusedPO.front());
			p_vk->unusedPO.pop_front();
		}
		else {
			clust_it->PO.push_back(p_vk->physicalObjectsInfo.size());
			p_vk->physicalObjectsInfo.push_back(physicalObjectInfo{});
		}
	}
}

particle::cluster_it_t particle::calcClusterID(uint32_t clusterSize)
{
	uint32_t cluster_ID;
	if(p_vk->unusedClusterIndexes.size()){
		cluster_ID = p_vk->unusedClusterIndexes.front();
		p_vk->clusters_continuous[cluster_ID].index = clusterSize ;
		p_vk->unusedClusterIndexes.pop_front();
	}
	else {
		cluster_ID = p_vk->clusters_continuous.size();
		p_vk->clusters_continuous.push_back(index_wrapper{ clusterSize });
	}
	cluster_info.emplace_back(clusterInfo{ cluster_ID, clusterSize, {} });
	cluster_PO_global_indexes.push_back(&(cluster_info.back().PO));
	cluster_info.back().PO_iterator = (--cluster_PO_global_indexes.end());
	return --cluster_info.end();
}

/// <summary>
/// constructor for particle
/// </summary>
/// <param name="particlesAmount"></param>
/// <param name="pos"></param>
/// <param name="p_mesh"></param>
/// <param name="p_vk"></param>
particle::particle(class model* p_model, class vk* p_vk): p_model(p_model), p_vk(p_vk)
{
	local_batch = new indirectBatchCluster;
	firstIndexMesh = p_model->first_mesh_id;

	local_batch->p_model = p_model;
	local_batch->instance_count = particlesAmount;
	local_batch->mesh_count = 1;
	local_batch->firstMeshIndex = firstIndexMesh;
	local_batch->clustered_PO_global_indexes = &cluster_PO_global_indexes;

	p_vk->draws_cluster.push_back(local_batch);
}

/// <summary>
/// avoid it
/// </summary>
/// <param name="clust_it"></param>
/// <param name="global_ID"></param>
void particle::removeInstance(cluster_it_t clust_it, uint32_t global_ID) {
	--(*particlesAmount);
	p_vk->draw_object_count_changed = true;
	--(p_vk->physical_object_amount);
	--(p_vk->render_object_amount);
	p_vk->unusedPO.push_back(global_ID);

	if (auto iter = std::find(clust_it->PO.begin(), clust_it->PO.end(), global_ID); iter != clust_it->PO.end()) {
		clust_it->PO.erase(iter);
	}
	else {
		assert(0);
	}
}

void particle::removeInstance(cluster_it_t clust_it, PO_global_indexes_t::iterator PO_it) {
	--(*particlesAmount);
	p_vk->draw_object_count_changed = true;
	--(p_vk->physical_object_amount);
	--(p_vk->render_object_amount);
	p_vk->unusedPO.push_back(*PO_it);

	clust_it->PO.erase(PO_it);
}

void particle::removeCluster(cluster_it_t clust_it) {
	p_vk->draw_object_count_changed = true;
	(p_vk->physical_object_amount) -= clust_it->size;
	(p_vk->render_object_amount) -= clust_it->size;
	(*particlesAmount) -= clust_it->size;

	p_vk->unusedPO.splice(p_vk->unusedPO.end(), clust_it->PO);

	p_vk->unusedClusterIndexes.push_back(clust_it->index);
	cluster_PO_global_indexes.erase(clust_it->PO_iterator);
	cluster_info.erase(clust_it);
}

particle::~particle()
{
	delete local_batch;
}