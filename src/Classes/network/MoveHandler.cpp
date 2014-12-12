﻿#include "pch.h"
#include "Network.h"

#include "objects/tags.h"
#include "objects/Unit.h"
#include "objects/Player.h"

using namespace cocos2d;

void Network::handleMoveStart(
	MoveStartNoti *pkt){

	auto unit = Unit::getInstanceByID(
		pkt->id);

	unit->velocity.x = pkt->velocity_x;
	unit->velocity.y = pkt->velocity_y;

	printf("move %d / %f %f\n",
		pkt->id,
		pkt->velocity_x, pkt->velocity_y);
}
void Network::handleMoveEnd(
	MoveEndNoti *pkt){

	auto unit = Unit::getInstanceByID(
		pkt->id);

	unit->velocity.set(0,0);
	unit->stopAllActionsByTag(
		ActionType::Move);

	printf("adj %f %f TO %f %f\n",
		unit->getPositionX(), unit->getPositionY(),
		pkt->end_x, pkt->end_y);

	unit->runAction(
		MoveTo::create(
			1.0f / Global::fps, Vec2(pkt->end_x, pkt->end_y)));

	printf("move end %d\n", pkt->id);
}

void Network::handleSyncRotationNoti(
	SyncRotationNoti *pkt){

	auto unit = Unit::getInstanceByID(
		pkt->id);
	auto player = Player::getInstance();

	/* 자신 플레이어 회전 패킷은 무시 */
	if(pkt->id == player->getID())
		return;

	unit->runAction(
		RotateTo::create(0.1, pkt->angle));
}