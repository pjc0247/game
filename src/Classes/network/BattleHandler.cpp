﻿#include "pch.h"
#include "Network.h"

#include "objects/ControlablePlayer.h"
#include "objects/Unit.h"
#include "objects/Bullet.h"
#include "objects/Stage.h"

using namespace cocos2d;

void Network::handleUseSkillNoti(
	UseSkillNoti *pkt){

	auto unit = Unit::getInstanceByID(pkt->id);
	auto player = ControlablePlayer::getInstance();

	if (!unit)
		return;

	if(unit == player)
		return;

	unit->useSkill(
		pkt->skill_id,
		Vec2(pkt->x, pkt->y));
}
void Network::handleAddPassiveNoti(
	AddPassiveNoti *pkt){

	auto unit = Unit::getInstanceByID(pkt->id);
	if (!unit)
		return;

	unit->addPassive(pkt->skill_id);
}
void Network::handleRemovePassiveNoti(
	RemovePassiveNoti *pkt){

	auto unit = Unit::getInstanceByID(pkt->id);
	if (!unit)
		return;

	unit->removePassive(pkt->skill_id);
}
void Network::handleAttack(
	Attack *pkt){
	auto attacker = Unit::getInstanceByID(pkt->attacker_id);
	auto target = Unit::getInstanceByID(pkt->target_id);

	if (!attacker || !target)
		return;

//	target->setPosition(pkt->target_position_x, pkt->target_position_y);
	target->runAction(
		EaseOut::create(
		MoveTo::create(0.25f, Vec2(pkt->knockback_x, pkt->knockback_y))
		, 10.f));

	AttackData attack_data;
	attack_data.position.set(pkt->attack_position_x, pkt->attack_position_y);
	attack_data.user = attacker;
	attack_data.damage = pkt->damage;
	attack_data.aggro = pkt->aggro;
	if (target->damage(attack_data)) {
		auto ally = Stage::getInstance(target->getStageID())->getAlly(target->getAllyID());
		ally->remove(target);
	}
}
