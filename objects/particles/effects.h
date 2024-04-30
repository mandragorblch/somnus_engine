#include "misc/vk_misc.h"
#include "vulkan/vk.h"
#include "objects/particles/particle.h"
#include <random>
#include "maths/math.h"
#include <functional>

void sphericalExplosion(const glm::vec3& pos, uint32_t amount, class particle& p_particle, class vk* p_vk);

void sphericalExplosion(const glm::vec3& pos, uint32_t amount, class particle& p_particle, class vk* p_vk)
{
	//int quality = 10'000;
	//std::random_device random;
	//std::mt19937 generate(random());
	//std::uniform_int_distribution<int> first_radius(-1 * quality, 1 * quality);
	//std::uniform_int_distribution<int> mass_disturb(0, 10 * quality);
	//std::uniform_int_distribution<int> velocity_disturb(20 * quality, 80 * quality);
	//std::uniform_int_distribution<int> sign(0, 1);

	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	p_vk->render_object_amount += amount;
	//float cos_theta, theta, cos_phi, phi;
	auto cluster_iter = p_particle.calcClusterID(amount);
	p_particle.calcGlobal_IDs(cluster_iter, amount);

	auto PO_iter = cluster_iter->PO.end();
	(*p_particle.particlesAmount) += amount;
	for (int32_t current_index = 0; current_index < amount; ++current_index) {
		physicalObjectInfo& current_physicalObjectInfo = p_vk->physicalObjectsInfo[*(--PO_iter)];
		//delete p_vk->all_meshes.at(p_particle->firstIndexMesh + particle_index);
		class mesh* current_mesh = p_particle.p_model->meshes[0];

		//print_b(current_mesh->info.index);
		//*current_mesh = *p_particle->p_mesh;

		glm::vec4 rand_quat = custom_maths::fast_normalize_glm_vec4(glm::vec4(static_cast<float>(rand()) / (RAND_MAX / 2) - 1, static_cast<float>(rand()) / (RAND_MAX / 2) - 1, static_cast<float>(rand()) / (RAND_MAX / 2) - 1, static_cast<float>(rand()) / (RAND_MAX / 2) - 1));

		//direction.x = static_cast<float>(first_radius(generate)) / quality;
		//float x_squared = direction.x * direction.x;
		//std::uniform_real_distribution<> second_radius(0.f, 1.f - x_squared);
		//direction.y = sqrtf(second_radius(generate)) * (sign(generate) ? -1. : 1.);
		//float y_squared = direction.y * direction.y;
		//std::uniform_real_distribution<> third_radius(0.f, 1.f - x_squared - y_squared);
		//direction.z = sqrtf(1 - x_squared - y_squared) * (sign(generate) ? -1. : 1.);
		//direction.w = sqrtf(1 - x_squared - y_squared - direction.z * direction.z) * (sign(generate) ? -1. : 1.);

		//direction = glm::normalize(glm::vec4(first_radius(generate), 0, first_radius(generate), 0));

		//cos_theta = direction.z;
		//theta = acosf(cos_theta);
		//cos_phi = direction.x / sinf(theta);
		//phi = (acosf(cos_phi) * (direction.y < 0 ? -1. : 1.));

		//float theta = static_cast<float>(first_radius(generate)) * 3.1415926535 / quality;
		//float phi = static_cast<float>(first_radius(generate)) * 3.1415926535 / quality;

		//glm::vec3 direction = glm::vec3(sinf(theta) * cosf(phi), sinf(theta) * sinf(phi), cosf(theta));

		current_physicalObjectInfo.pos_index = glm::vec4(pos, reinterpret_cast<float&>(p_particle.cluster_info.back().index));
		current_physicalObjectInfo.velocity_mass = glm::vec4(custom_maths::rotation_by_quat(glm::vec3(0, 1, 0), rand_quat) * ((static_cast<float>(rand()) / RAND_MAX) * 20), static_cast<float>(rand()) / RAND_MAX + 1);
		current_physicalObjectInfo.angularVelocity_momentOfInertia = /*glm::vec4(direction * (static_cast<float>(velocity_disturb(generate)) / quality), 1.f) * 0.f*/glm::vec4(0.f);
		current_physicalObjectInfo.force = glm::vec4(0);
		current_physicalObjectInfo.quat_rot = rand_quat;
		current_physicalObjectInfo.lifeTime = static_cast<float>(rand() & 0b0000'0000'0000'0000'0000'0011'1111'1111) / 500/*-1.f*/;
		current_physicalObjectInfo.type = PO_TYPE::PARTICLE;
		current_physicalObjectInfo.should_be_deleted = false;
	}
}
