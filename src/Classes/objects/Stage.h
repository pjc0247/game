﻿#pragma once

#include "cocos2d.h"

#include "MouseEventListener.h"
#include "KeyboardEventListener.h"

class Ally;
class CollisionDetector;

class Stage : public cocos2d::Node,
	public MouseEventListener{
public:
	static bool create();
	static Stage *getInstance(
		int id);

	void setObserverMode();
	void gameOver();
	CollisionDetector* getCollisionDetector();
	Ally* getAlly(int type);

protected:
	virtual bool init(
		int id);
	virtual bool initExternalData();
	virtual bool initObject(
		const std::string &name,
		int x,int y);
	virtual void update(
		float dt);

	virtual void onMouseMove(
		int btn, float x,float y);

	virtual void onExit();

protected:
	Stage();
	virtual ~Stage();

protected:
	int id;

	cocos2d::Sprite *floor;
	cocos2d::Vec2 mousePos;
	Ally *ally[2];
	CollisionDetector *collisionDetector;

};
