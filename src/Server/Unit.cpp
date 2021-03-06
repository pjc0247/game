﻿#include "stdafx.h"
#include "Unit.h"

#include "Stage.h"
#include "GameRoom.h"
#include "CollisionDetector.h"

#include "AttackData.h"
#include "PacketType.h"

#include "shared/skill/SkillPool.h"
#include "shared/skill/ActiveSkill.h"
#include "shared/skill/PassiveSkill.h"

struct Unit::PassiveData{
	float remaining;
	float update;
	float interval;
};

Unit::Unit()
	:stage(nullptr), removed(false)
	, id(INVALID_ID), type(UT_NONE), ally(0)
	, friction(0), radius(0)
	, usingSkill(nullptr), skillTimer(0), skillPhase(0) {
	init();
}
Unit::~Unit(){
}

bool Unit::init(){
	if (!initPhysics())
		return false;
	if (!initAttrs())
		return false;
	return true;
}

bool Unit::initAttrs(){
	_INIT_ATTR(hp, 1);
	_INIT_ATTR(speed, 0);
	_INIT_ATTR(hpRegen, 0);
	_INIT_ATTR(range, 0);
	_INIT_ATTR(attack, 0);
	_INIT_ATTR(radius, 1);

	return true;
}

bool Unit::initPhysics(){
	return true;
}

void Unit::update(float dt) {
	updateGen(dt);
	updatePassives(dt);
	updatePhysics(dt);
	updateSkill(dt);
}

void Unit::updateGen(float dt){
	__ATTR(hp).increase(_ATTR(hpRegen) * dt);
}
void Unit::updatePassives(float dt){

	std::vector<int> removeList;

	for (auto &pair : passives){
		auto id = pair.first;
		auto &data = pair.second;
		auto skill = SkillPool::getInstance()->get(id);

		data.remaining -= dt;
		data.update -= dt;

		/* 지속 시간이 끝났으면 지우기 */
		if (skill->duration != Skill::Infinite &&
			data.remaining <= 0)
			removeList.push_back(id);
		/* interval마다 update 호출해주기 */
		else if (data.interval != 0){
			if (data.update <= 0){
				auto pool = SkillPool::getInstance();
				auto skill = (PassiveSkill*)pool->get(id);

				if (!skill->update(this, data.remaining))
					removeList.push_back(id);

				data.update = data.interval;
			}
		}
	}

	for (auto id : removeList)
		removePassive(id);
}

void Unit::updatePhysics(float dt){
	velocity += acceleration * dt;
	velocity = velocity.getNormalized() * max(0, velocity.getLength() - friction * dt);
	position += velocity * dt;
}

void Unit::updateSkill(float dt) {
	if (!usingSkill)
		return;

	skillTimer += dt;
	usingSkill->update(this, dt);
	if (dt > usingSkill->duration)
		usingSkill = nullptr;
}


bool Unit::onDamage(const AttackData &attackData){
	return true;
}

bool Unit::onDeath(){
	return true;
}

bool Unit::damage(const AttackData& attackData){
	if (onDamage(attackData)){
		__ATTR(hp).increase(-attackData.damage);
	}

	if (_ATTR(hp) <= 0) {
		if (onDeath()){
			stage->removeUnit(this);
			return true;
		}
	}
	return false;
}

bool Unit::useSkill(int skill_id, Vec2 pos){

	auto pool = SkillPool::getInstance();
	auto skill = (ActiveSkill*)pool->get(skill_id);

	if (!skill)
		return false;

	usingSkill = skill;
	skillTimer = 0;
	skillPhase = 0;
	skillTarget = pos;
	skill->use(this, pos);

	UseSkillNoti noti;
	noti.id = id;
	noti.skill_id = skill_id;
	noti.x = pos.x;
	noti.y = pos.y;

	stage->gameroom->sendPacket(noti);

	return true;
}
void Unit::addPassive(int skill_id){

	auto pool = SkillPool::getInstance();
	auto skill = (PassiveSkill*)pool->get(skill_id);

	/* 이미 가지고 있는 패시브 */
	if (passives.find(skill_id) != passives.end())
		return;

	AddPassiveNoti noti;
	noti.id = id;
	noti.skill_id = skill_id;
	stage->gameroom->sendPacket(noti);

	/* TODO : 패시브 중복 적용이면
	남은 시간 더하기 or 초기화할건지 */
	PassiveData data;
	data.update = data.interval = skill->interval;
	data.remaining = skill->duration;
	passives[skill_id] = data;

	for (auto pair : skill->bonusList){
		std::string name = pair.first;
		Attribute attr = pair.second;
		Attribute &target = attrs[name];

		target.getBonusValue() += attr.getBonusValue();
		target.getBonusRate() += attr.getBonusRate();
	}
}

void Unit::removePassive(int skill_id){

	auto pool = SkillPool::getInstance();
	auto skill = (PassiveSkill*)pool->get(skill_id);

	_ASSERT(passives.find(skill_id) != passives.end());

	RemovePassiveNoti noti;
	noti.id = id;
	noti.skill_id = skill_id;
	stage->gameroom->sendPacket(noti);

	passives.erase(skill_id);

	for (auto pair : skill->bonusList){
		std::string name = pair.first;
		Attribute attr = pair.second;
		Attribute &target = attrs[name];

		target.getBonusValue() -= attr.getBonusValue();
		target.getBonusRate() -= attr.getBonusRate();
	}
}

Attribute &Unit::getAttribute(const std::string &name){

	_ASSERT(attrs.find(name) != attrs.end());

	return attrs[name];
}

void Unit::collide(Vec2 delta, Unit* other){
	position += delta * 0.5f;

	/*
	MoveNoti noti;
	noti.id = id;
	noti.start_x = position.x;
	noti.start_y = position.y;
	noti.velocity_x = velocity.x;
	noti.velocity_y = velocity.y;
	stage->gameroom->sendPacket(noti);
	*/
}
