#include "Player.h"

#include "EnemyPool.h"
#include "Bullet.h"

#include "common/resource.h"
#include "common/PhysicsFactory.h"

using namespace std;
using namespace cocos2d;

static Player *instance = nullptr;

Player::Player() : 
	speed(8){
}
Player::~Player(){
}

Player *Player::create(){
	CC_ASSERT(instance == nullptr);

	instance = new Player();
	
	if(instance && instance->init()){
		instance->autorelease();
		return instance;
	}
	CC_SAFE_DELETE(instance);
	return nullptr;
}
Player *Player::getInstance(){
	CC_ASSERT(instance != nullptr);

	return instance;
}

bool Player::init(){
	if (!Unit::initWithPartedBody(R::PlayerBody))
        return false;

	enableMouseInput(this);
	enableKeyboardInput(this);
	enableDebug();

	lay = DrawNode::create();
	addChild(lay);

	scarf = Scarf::create();
	addChild(scarf);

    return true;
}
bool Player::initPhysics(){
	auto factory = PhysicsFactory::getInstance();
	//auto pbody = factory->make("player");

	//if(pbody){
		/* temporary disalbed */
		//setPhysicsBody(pbody);
		return true;
	//}
	
	return false;
}

void Player::processRotation(
	float x,float y){

	auto screenSize = Director::getInstance()->getWinSize();
	auto worldPos =
		getParent()->convertToWorldSpace(getPosition());
	auto half = worldPos / 2;
	
	double dx = worldPos.x - x;
	double dy = worldPos.y - y;
	double rad = atan2(dy, dx);
	double degree = (rad*180)/M_PI;

	body->setRotation(-degree + 90);
	scarf->pushRotation(degree + 90);
}
void Player::processEyeline(
	float x,float y){

	/* disabled */
	/*
	Node *contactObject = nullptr;
	Vec2 contactPoint = Vec2(x,y);
	getPhysicsBody()->getWorld()->rayCast(
		[&](PhysicsWorld &world, const PhysicsRayCastInfo& info, void* data){
			if(info.shape == getPhysicsBody()->getShape(0))
				return true;

			contactPoint = info.contact;
			contactObject = info.shape->getBody()->getNode();
			return false;
		},
		getPosition(), Vec2(x,y),
		nullptr);

	lay->clear();
	lay->drawSegment(
		Vec2(0,0), contactPoint - getPosition(), 3, Color4F(1,0,0,1));

	auto pool = EnemyPool::getInstance();
	if(contactObject){
		pool->focus((Enemy*)contactObject);
	}
	else{
		pool->focus(nullptr);
	}
	*/
}
void Player::processMove(
	EventKeyboard::KeyCode keycode){

	Vec2 moveBy(0,0);

	switch(keycode){
	/* UP */
	case EventKeyboard::KeyCode::KEY_W:
		moveBy.set(0, speed); break;
	/* RIGHT */
	case EventKeyboard::KeyCode::KEY_D:
		moveBy.set(speed, 0); break;
	/* LEFT */
	case EventKeyboard::KeyCode::KEY_A:
		moveBy.set(-speed, 0); break;
	/* DOWN */
	case EventKeyboard::KeyCode::KEY_S:
		moveBy.set(0, -speed); break;
	}

	float frameRate =
		Director::getInstance()->getFrameRate();
	runAction(
		MoveBy::create(1.0/frameRate, moveBy))
		->setTag(actionMove);
}
void Player::processAttack(
	int btn, float x,float y){

	if(btn == MOUSE_BUTTON_LEFT){
		skills[skillMouseLeft]->use(this, Vec2(x,y));
	}
	else if(btn == MOUSE_BUTTON_RIGHT){
	}
}

void Player::onKeyboardDown(
	EventKeyboard::KeyCode keycode){
}
void Player::onKeyboardUp(
	EventKeyboard::KeyCode keycode){
}
void Player::onKeyboardPressed(
	EventKeyboard::KeyCode keycode){

	processEyeline(cursor.x, cursor.y);
	processMove(keycode);
}

void Player::onMouseMove(
	int btn, float x,float y){

	processEyeline(x,y);
	processRotation(x,y);

	cursor.set(x,y);
}
void Player::onMouseDown(
	int btn, float x,float y){

	processAttack(btn, x,y);
}
void Player::onMouseUp(
	int btn, float x,float y){
}