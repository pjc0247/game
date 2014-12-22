﻿#include "stdafx.h"
#include "GameRoomManager.h"
#include "Client.h"
#include "ClientManager.h"
#include "GameRoom.h"
#include "Scheduler.h"
#include "PacketType.h"

static GameRoomManager* instance;

void GameRoomManager::create() {
	instance = new GameRoomManager();
}

GameRoomManager::GameRoomManager() :
	available(INVALID_ID){
}

GameRoomManager* GameRoomManager::getInstance() {
	return instance;
}


GameRoom* GameRoomManager::getGameRoom(const id_t id) {
	std::lock_guard<std::mutex> lock(mutex);
	auto it = rooms.find(id);
	if (it == rooms.end())
		return nullptr;
	return it->second;
}
GameRoom *GameRoomManager::getAvailableGameRoom(){
	/* TODO : 4 교체 */
	auto room = getGameRoom(available);
	if (!room || room->clientIds.size() >= 2) {
		available = INVALID_ID;
	}

	if (available == INVALID_ID){
		/* ISSUE : id로 관리하면 매번 map::find가 실행됨 */ 
		/* SOLUTION : createGameRoom이 GameRoom*을 반환하게 */
		available = createGameRoom();
	}

	return getGameRoom(available);
}

id_t GameRoomManager::createGameRoom() {
	std::lock_guard<std::mutex> lock(mutex);
	id_t id = dispenser.issue();
	GameRoom* room = new GameRoom(id);
	rooms[id] = room;
	Scheduler::getInstance()->schedule(id);
	printf("Room %d Created.\n", id);
	return id;
}

bool GameRoomManager::removeGameRoom(const id_t id) {
	std::lock_guard<std::mutex> lock(mutex);
	auto it = rooms.find(id);
	if (it == rooms.end())
		return false;
	rooms.erase(it);
	printf("Room %d Exploded.\n", id);
	SAFE_DELETE(it->second);
	return true;
}
