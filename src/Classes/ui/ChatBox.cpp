﻿#include "pch.h"
#include "ChatBox.h"
#include "InputBox.h"

#include "common/resource.h"

using namespace cocos2d;

static ChatBox *instance = nullptr;

ChatBox::ChatBox() :
	input(nullptr){
}
ChatBox::~ChatBox(){
}

ChatBox *ChatBox::create(){
	instance = new ChatBox();

	if(instance && instance->init()){
		instance->autorelease();
		return instance;
	}
	CC_SAFE_DELETE(instance);
	return nullptr;
}
ChatBox *ChatBox::getInstance(){
	return instance;
}

bool ChatBox::init(){
	if(!ClippingRectangleNode::init())
		return false;

	setClippingRegion(Rect(0,0,400,300));

	auto bgi = Sprite::create("how_to_play.png");
	bgi->setAnchorPoint(Vec2(0,0));
	addChild(bgi);
	setColor(Color3B::RED);

	input = InputBox::create(
		"", "arial", 20);
	input->setColor(Color3B::BLACK);
	input->setAnchorPoint(Vec2(0,0));
	addChild(input);

	enableKeyboardInput(this);

	return true;
}

void ChatBox::output(
	const std::string &msg){

	shift();
	addLine(msg);
}
void ChatBox::shift(){
	for(auto line : lines){
		line->runAction(
			MoveBy::create(0.0, Vec2(0,LineSize)));
	}
	if(lines.size() >= MaxLines){
		lines.front()->runAction(
			Sequence::create(
				FadeOut::create(0.5),
				RemoveSelf::create(),
				nullptr
			));
		lines.pop_front();
	}
}
void ChatBox::addLine(
	const std::string &msg){

	auto label = LabelTTF::create(
		msg, "arial", LineSize);
	label->setColor(Color3B::RED);
	label->setOpacity(0);
	label->setAnchorPoint(Vec2(0,0));
	label->setPosition(
		Vec2(0, MaxLines*5));
	label->runAction(
		Spawn::create(
			MoveBy::create(0.0, Vec2(0,LineSize)),
			FadeIn::create(0.0),
			nullptr
		));

	addChild(label);
	lines.push_back(label);
}

void ChatBox::onKeyboardDown(
	EventKeyboard::KeyCode key){

	if(key == EventKeyboard::KeyCode::KEY_KP_ENTER)
		input->beginInput();
}
void ChatBox::onKeyboardUp(
	EventKeyboard::KeyCode key){

	if(key == EventKeyboard::KeyCode::KEY_KP_ENTER){
		output(input->getString());
		input->clear();
	}
}