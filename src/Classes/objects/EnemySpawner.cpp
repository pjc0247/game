﻿#include "pch.h"
#include "EnemySpawner.h"
#include "EnemyFactory.h"

#include "common/resource.h"

#include "Enemy.h"
#include "Stage.h"
#include "Ally.h"

using namespace cocos2d;

EnemySpawner::EnemySpawner(){
}
EnemySpawner::~EnemySpawner(){
}

EnemySpawner *EnemySpawner::create(){
	EnemySpawner *e = new EnemySpawner();

	if(e && e->init()){
		e->autorelease();
		return e;
	}
	CC_SAFE_DELETE(e);
	return nullptr;
}
bool EnemySpawner::init(){
	if(!Node::init())
		return false;

	scheduleUpdate();

	return true;
}
void EnemySpawner::update(
	float dt){
	/*
	if(rand()%1000 == 1){
//		spawn(enemySpiral);
//		spawn((EnemyType)(rand() % enemyMax));
	}
	*/
}
 
void EnemySpawner::spawn(
	EnemyType type){

	auto stage = Stage::getInstance(0);
	auto ally = Stage::getInstance(0)->getAlly(
		Ally::Type::allyEnemy);
	auto factory = EnemyFactory::getInstance();

	Enemy *e = factory->createEnemy(0, type);
	e->setPosition(
		getPosition());
	e->resetAggro();

	ally->push(e);
	stage->addChild(e, Z::unit);
}