﻿#include "pch.h"
#include "MouseEventListener.h"

using namespace cocos2d;

struct MouseEventListener::MouseData{
	MouseData(){
		pressed = false;
		x = y = 0;
	}

	bool pressed;
	float x,y;
};

MouseEventListener::MouseEventListener() :
	mouseListener(nullptr){
}
MouseEventListener::~MouseEventListener(){
	disableMouseInput();
}

void MouseEventListener::enableMouseInput(
	Node *_target){

	mouseListener = EventListenerMouse::create();
	mouseListener->onMouseMove = [this](Event *event){
		EventMouse *e = (EventMouse*)event;
		MouseData &data = pressed[e->getMouseButton()];

		data.x = e->getCursorX();
		data.y = e->getCursorY();
		onMouseMove(
			e->getMouseButton(),
			e->getCursorX(), e->getCursorY());
	};
	mouseListener->onMouseUp = [this](Event *event){
		EventMouse *e = (EventMouse*)event;
		MouseData &data = pressed[e->getMouseButton()];

		data.pressed = false;
		data.x = e->getCursorX();
		data.y = e->getCursorY();
		onMouseDown(
			e->getMouseButton(),
			e->getCursorX(), e->getCursorY());
	};
	mouseListener->onMouseDown = [this](Event *event){
		EventMouse *e = (EventMouse*)event;
		MouseData &data = pressed[e->getMouseButton()];

		data.pressed = true;
		data.x = e->getCursorX();
		data.y = e->getCursorY();
		onMouseUp(
			e->getMouseButton(),
			e->getCursorX(), e->getCursorY());
	};

	target = _target;
	auto director = Director::getInstance();
	director->getScheduler()->schedule(
		SEL_SCHEDULE(&MouseEventListener::processMouseTurbo), (Ref*)this,
		1.0f / Global::fps, false);
	target->getEventDispatcher()->
		addEventListenerWithSceneGraphPriority(mouseListener, target);
}
void MouseEventListener::disableMouseInput(){
	if(mouseListener){
		target->getEventDispatcher()->
			removeEventListener(mouseListener);
		Director::getInstance()->getScheduler()->unschedule(
			SEL_SCHEDULE(&MouseEventListener::processMouseTurbo), (Ref*)this);
		mouseListener = nullptr;
	}

	auto director = Director::getInstance();
	director->getScheduler()->unschedule(
		SEL_SCHEDULE(&MouseEventListener::processMouseTurbo), (Ref*)this);
}

void MouseEventListener::processMouseTurbo(float dt){
	for(auto &p : pressed){
		MouseData &data = p.second;

		if(data.pressed == true)
			onMousePressed(
				p.first, data.x,data.y);
	}
}

void MouseEventListener::onMouseMove(
	int btn, float x,float y){

}
void MouseEventListener::onMouseDown(
	int btn, float x,float y){

}
void MouseEventListener::onMouseUp(
	int btn, float x,float y){

}
void MouseEventListener::onMousePressed(
	int btn, float x,float y){

}