#include "types.h"
#include "objects/particles/particle.h"
//#include "objects/particles/effects.h"
#include <iostream>

extern void sphericalExplosion(const glm::vec3& pos, uint32_t amount, class particle& p_particle, class vk* p_vk);

Object::Object(int type, int mask, int local_ID, int global_ID, int damage, int hp, gameObject* p_GO): type(type), mask(mask), local_ID(local_ID), global_ID(global_ID), damage(damage), hp(hp), p_GO(p_GO), should_be_deleted(false) {

}

Object::~Object()
{
	p_GO->removeInstance(this);
}

void Object::takeDamage(int damage)
{
	printf("trying to deal damage to non-damagable\n");
	assert(0);
}

Location::Location(class gameObject* p_GO): Object(static_cast<int>(TYPE::Ground), static_cast<int>(TYPE::OnGround), p_GO->getLocal_ID(), p_GO->getGlobal_ID(), 0, 0, p_GO) {

}

Location::~Location() {
}

Blast::Blast(class gameObject* p_GO, int damage): Object(static_cast<int>(TYPE::Damaging), static_cast<int>(TYPE::Damagable), p_GO->getLocal_ID(), p_GO->getGlobal_ID(), damage, 0, p_GO) {
	obj_ID = OBJECT_ID::Blast;
}

Blast::~Blast() {

}

Bomb::Bomb(class gameObject* p_GO, int damage): Object(static_cast<int>(TYPE::UNDEFINED), static_cast<int>(TYPE::UNDEFINED), p_GO->getLocal_ID(), p_GO->getGlobal_ID(), damage, 0, p_GO) {
	obj_ID = OBJECT_ID::Bomb;
}

void Bomb::explode()
{
	Blast* blast = new Blast(p_GO->p_storage->gameObjects[3], 2);
	p_GO->p_storage->objects.push_back(blast);
	p_GO->p_storage->gameObjects[3]->addDynamicInstance(blast, p_GO->actors[local_ID]->getGlobalPose().p, physx::PxQuat(0, 0, 0, 1), physx::PxVec3(0), physx::PxVec3(0), -999.0f);

	sphericalExplosion(p_GO->p_vk->physicalObjectsInfo[global_ID].pos_index, 10000, *p_GO->p_storage->particles[0], p_GO->p_vk);
}

Bomb::~Bomb()
{

}

Barrel::Barrel(class gameObject* p_GO, int hp): Object(static_cast<int>(TYPE::Damagable), static_cast<int>(TYPE::Damaging), p_GO->getLocal_ID(), p_GO->getGlobal_ID(), 0, hp, p_GO) {
	obj_ID = OBJECT_ID::Barrel;
}

void Barrel::takeDamage(int damage)
{
	hp -= damage;
	if (hp <= 0) {
		should_be_deleted = true;
	}
}

Barrel::~Barrel() {

}

Player::Player(class gameObject* p_GO, int hp) : Object(static_cast<int>(TYPE::Damagable), static_cast<int>(TYPE::Damaging), p_GO->getLocal_ID(), p_GO->getGlobal_ID(), 0, hp, p_GO) {
	obj_ID = OBJECT_ID::Player;
}

Player::~Player() {

}

void Player::takeDamage(int damage)
{
	hp -= damage;
	if (hp <= 0) {
		std::cout << "u'r dead!\n";
		//should_be_deleted = true;
	}
}
