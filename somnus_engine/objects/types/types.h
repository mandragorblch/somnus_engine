#pragma once
#include "objects/game_objects/gameObject.h"

enum class TYPE : int
{
	UNDEFINED		= (0 << 0),
	OnGround		= (1 << 0),
	Ground			= (1 << 1),
	Interacting		= (1 << 2),
	Interactable	= (1 << 3),
	Damaging		= (1 << 4),
	Damagable		= (1 << 5)
};

enum class OBJECT_ID : int {
	Location,
	Barrel,
	Bomb,
	Blast,
	Player
};

class Object
{
public:
	int type;
	int mask;
	int local_ID;
	int global_ID;
	int damage;
	int hp;
	enum class OBJECT_ID obj_ID;
	class gameObject* p_GO;
	bool should_be_deleted;
	virtual void takeDamage(int damage);
	Object(int type, int mask, int local_ID, int global_ID, int damage, int hp, gameObject* p_GO);
	virtual ~Object();
};

class Location : public Object {
public:
	Location(class gameObject* p_GO);
	~Location() override;
};

class Blast : public Object {
public:
	Blast(class gameObject* p_GO, int damage);
	~Blast() override;
};

class Bomb : public Object {
public:
	Bomb(class gameObject* p_GO, int damage);
	void explode();
	~Bomb() override;
};

class Barrel : public Object {
public:
	Barrel(class gameObject* p_GO, int hp);
	void takeDamage(int damage) override;
	~Barrel() override;
};

class Player : public Object {
public:
	Player(class gameObject* p_GO, int hp);
	void takeDamage(int damage) override;
	~Player() override;
};