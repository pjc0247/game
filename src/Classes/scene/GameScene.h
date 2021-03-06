﻿#pragma once

#include "cocos2d.h"

#include "objects/KeyboardEventListener.h"

class Player;
class Stage;
class Cursor;

class GameScene : public cocos2d::Layer,
	public KeyboardEventListener{
public:
	CREATE_FUNC(GameScene);

	static cocos2d::Scene* scene();

protected:
	GameScene();
	virtual ~GameScene();

	virtual bool init();
	virtual bool initUI();

	virtual void onEnter();
	virtual void onExit();

	virtual void onInitGlobalObjects();
	virtual void onReleaseGlobalObjects();

	virtual void update(
		float dt);

	virtual void onKeyboardDown(
		cocos2d::EventKeyboard::KeyCode key);

};
