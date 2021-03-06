﻿#include "pch.h"
#include "Network.h"

#include "GameRoom.h"

#include "common/resource.h"

#include "objects/Stage.h"
#include "objects/Unit.h"

#include "scene/GameScene.h"
#include "scene/LobbyScene.h"
#include "scene/SelectScene.h"

using namespace cocos2d;

void Network::handleRoomResponse(
	RoomResponse *pkt) {
	printf("room list : \n");
	for (int i = 0; i < pkt->room_num; ++i) {
		printf("%d\n", pkt->room_list[i]);
	}
	auto scene = Director::getInstance()->getRunningScene();
	auto lobby = (LobbyScene*)scene->getChildByTag(1);
	lobby->addRooms(pkt->room_num, pkt->room_list);
}
void Network::handleEnterResponse(
	EnterResponse *pkt) {
	if (pkt->room_id == 0)
		printf("enter room failed\n");
	else {
		printf("enter success : %d\n", pkt->room_id);
		GameRoom::getInstance()->setRoomId(pkt->room_id);

		auto scene = SelectScene::scene();
		Director::getInstance()
			->replaceScene(scene);
		/*
		auto network = Network::getInstance();
		network->sendReadyRequest();
		printf("ready\n");
		*/
	}
}

/* TODO : 수정, 스테이지 id, 유닛 id 등 */
void Network::handleEnterNoti(
	EnterNoti *pkt){
	GameRoom::Client client;
	client.id = pkt->client_id;
	client.nickname = pkt->nickname;
	client.team = pkt->team_id;
	client.robot = pkt->robot_id;
	client.ready = pkt->ready != 0;
	printf("enter noti : %d %s\n", pkt->client_id, pkt->nickname);
	GameRoom::getInstance()->addClient(client);

	auto scene = Director::getInstance()->getRunningScene();
	auto select = (SelectScene*)scene->getChildByTag(2);
	if (select) select->refreshRoom();
}
void Network::handleLeaveNoti(
	LeaveNoti *pkt){

	printf("leave noti : %d\n", pkt->client_id);
	GameRoom::getInstance()->removeClient(pkt->client_id);

	auto scene = Director::getInstance()->getRunningScene();
	auto select = (SelectScene*)scene->getChildByTag(2);
	if (select) select->refreshRoom();

}
void Network::handleSelectRobotNoti(
	SelectRobotNoti *pkt) {
	
	printf("client %d selected robot %d.\n", pkt->client_id, pkt->robot_id);

	auto& client = GameRoom::getInstance()->getClient(pkt->client_id);
	if (client.id == 0)
		return;
	client.robot = pkt->robot_id;

	auto scene = Director::getInstance()->getRunningScene();
	auto select = (SelectScene*)scene->getChildByTag(2);
	if (select) select->refreshRoom();

}
void Network::handleSelectTeamNoti(
	SelectTeamNoti *pkt) {
	printf("client %d selected team %d.\n", pkt->client_id, pkt->team_id);

	auto& client = GameRoom::getInstance()->getClient(pkt->client_id);
	if (client.id == 0)
		return;
	client.team = pkt->team_id;

	auto scene = Director::getInstance()->getRunningScene();
	auto select = (SelectScene*)scene->getChildByTag(2);
	if (select) select->refreshRoom();

}

void Network::handleReadyNoti(
	ReadyNoti *pkt){
	auto& client = GameRoom::getInstance()->getClient(pkt->client_id);
	if (client.id == 0)
		return;
	client.ready = pkt->ready != 0;

	auto scene = Director::getInstance()->getRunningScene();
	auto select = (SelectScene*)scene->getChildByTag(2);
	if (select) select->refreshRoom();

}
void Network::handleStartGame(
	StartGame *pkt){

	printf("start game\n");
	auto scene = GameScene::scene();
	Director::getInstance()
		->replaceScene(scene);
}

void Network::handleGameOver(
	GameOver *pkt) {
	printf("game over. team %d win.\n", pkt->win_team);
	GameRoom::getInstance()->setWinTeam(pkt->win_team);
	Stage::getInstance(0)->gameOver();
	Unit::clearInstanceMap();
}
