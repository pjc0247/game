#include "stdafx.h"
#include "Slash.h"

#include "Unit.h"
#include "Bullet.h"
#include "Stage.h"

void Slash::use(
	Unit *u,
	Vec2 pos){

	auto stage = u->stage;

	auto bullet = new Bullet();
	bullet->position = u->position;
	bullet->fire(Vec2(pos.x, pos.y), 1);

	bullet->attackData.user = u;
	bullet->attackData.radius = 10;
	bullet->attackData.damage = 1;
	bullet->ally = u->ally;

	stage->addUnit(bullet);
}