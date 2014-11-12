#include "pch.h"
#include "SkillIcon.h"

#include "skill/SkillPool.h"
#include "skill/ActiveSkill.h"

using namespace cocos2d;

SkillIcon *SkillIcon::create(
	int id){

	SkillIcon *s = new SkillIcon();

	if(s && s->init(id)){
		s->autorelease();
		return s;
	}
	CC_SAFE_DELETE(s);
	return nullptr;
}
bool SkillIcon::init(
	int _id){

	id = _id;

	char tmp[128];
	sprintf(tmp, "%d.png", id);
	if(!Sprite::initWithFile(tmp))
		return false;

	overlay = ProgressTimer::create(this);
	overlay->setType(ProgressTimer::Type::RADIAL);
	overlay->setPosition(getContentSize()/2);
	overlay->setOpacity(255);
	overlay->setPercentage(100);
	addChild(overlay);

	setOpacity(128);

	return true;
}

void SkillIcon::use(){
	auto pool = SkillPool::getInstance();
	auto skill = (ActiveSkill*)pool->get(id);
	auto cooltime = skill->cooltime;

	runAction(
		ProgressFromTo::create(cooltime, 0, 100));
}
