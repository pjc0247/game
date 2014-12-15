﻿#include "pch.h"
#include "Network.h"

#include "common/resource.h"

#include "objects/Stage.h"
#include "objects/Unit.h"
#include "objects/Player.h"
#include "objects/Ally.h"
#include "objects/CollisionDetector.h"

void Network::handleSpawn(
	SpawnUnit *pkt){

	printf("ID %d / TYPE %d\n", pkt->id, pkt->unit_type);

	if(pkt->unit_type){
		auto player = Player::getInstance();
		player->setID(pkt->id);
		player->setPosition(pkt->x, pkt->y);
	}
	else if(Unit::getInstanceByID(pkt->id) == nullptr){
		auto stage = Stage::getInstance(0);
		auto players = Ally::getInstance(
			Ally::allyPlayer);
		auto unit = Unit::create(R::PlayerBody);
	
		stage->getCollisionDetector()
			->addUnit(unit);
		unit->radius = 30;
		unit->setAllyID(Ally::Type::allyPlayer);
		unit->setID(pkt->id);
		unit->setPosition(pkt->x, pkt->y);
		stage->addChild(unit);
		players->push(unit);
	}
}
void Network::handleRemoveUnit(
	RemoveUnit *pkt){

	auto unit = Unit::getInstanceByID(pkt->id);

	if(unit != nullptr){
		auto stage = Stage::getInstance(0);
		auto players = Ally::getInstance(
			Ally::Type::allyPlayer);

		players->remove(unit);
		unit->setOpacity(128);
		//stage->removeChild(unit);
	}
}