﻿#pragma once

#include "cocos2d.h"

#include <string>

#include "skill/ActiveSkill.h"

class FrontDash : public ActiveSkill{
public:
	virtual void use(
		Unit *u,
		cocos2d::Vec2 pos);

protected:
	virtual bool initExternalData(
		const std::string &dstPath);

protected:
	float distance;
};
