﻿#include "stdafx.h"
#include "GameRoom.h"
#include "GameRoomManager.h"
#include "ClientManager.h"
#include "Client.h"
#include "CompletionKeyType.h"
#include "Unit.h"
#include "Enemy.h"
#include "Player.h"
#include "TrashPool.h"

#include "Ally.h"

#include "config.h"

typedef std::pair<id_t, id_t> IdPair;

GameRoom::GameRoom(const id_t id)
	:id(id), ready(0), gameRunning(false) {
	for (int i = 0; i < Max::Teams; ++i)
		stage[i] = new Stage(this, i);
}

GameRoom::~GameRoom() {
	for (Unit* u : units) {
		SAFE_DELETE(u);
	}
	units.clear();

	if (config::gui) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	}
}

void GameRoom::update() {
	DWORD now_tick = timeGetTime();

	for (int i = 0; i < Max::Teams; ++i)
		stage[i]->update((now_tick - tick) * 0.001f);

	flush();

	tick = now_tick;

	if (config::gui) {
		SDL_SetRenderDrawColor(renderer, 255, 2552, 255, 255);
		SDL_RenderClear(renderer);

		for (auto unit : units){
			if (unit->stage->id != 0)
				continue;
			SDL_Rect rect = {
				(200 + unit->position.x / 4) - 5, 300 - (150 + unit->position.y / 4) - 5,
				10, 10 };
			SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
			SDL_RenderFillRect(
				renderer, &rect);
		}
		for (auto player : *stage[0]->ally[0]){
			SDL_Rect rect = {
				(200 + player->position.x / 4) - 6, 300 - (150 + player->position.y / 4) - 6,
				12, 12 };
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			SDL_RenderFillRect(
				renderer, &rect);
		}
		for (auto enemy : *stage[0]->ally[1]){
			SDL_Rect rect = {
				(200 + enemy->position.x / 4) - 6, 300 - (150 + enemy->position.y / 4) - 6,
				12, 12 };
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderFillRect(
				renderer, &rect);
		}

		SDL_RenderPresent(renderer);

		SDL_Event event;
		SDL_PollEvent(&event);
	}
}


bool GameRoom::enter(const id_t client_id) {
	if (client_id == INVALID_ID) return false;
	if (!clientIds.insert(IdPair(client_id, INVALID_ID)).second)
		return false;
	ClientManager::getInstance()->getClient(client_id)->setGameRoomId(id);
	return true;
}

bool GameRoom::leave(const id_t client_id) {
	if (client_id == INVALID_ID) return false;
	auto it = clientIds.find(client_id);
	if (it == clientIds.end())
		return false;
	clientIds.erase(it);
	ClientManager::getInstance()->getClient(client_id)->setGameRoomId(INVALID_ID);

	// 모두 나가면 방폭
	if (clientIds.size() == 0)
		GameRoomManager::getInstance()->removeGameRoom(id);
	return true;
}


bool GameRoom::startGame() {
	if (gameRunning || (size_t)ready < clientIds.size()) return false;
	gameRunning = true;

	StartGame packet;
	packet.team = 0;
	packet.seed = rand();
	sendPacket(packet);

	for (auto& id : clientIds) {
		Client* client = ClientManager::getInstance()->getClient(id.first);
		Player* player = new Player();
		id.second = stage[0]->addUnit(player);

		// TODO: 위치 지정
		player->position.x = (float)(rand() % 1600 - 800);
		player->position.y = (float)(rand() % 1200 - 600);

		SpawnUnit noti;
		noti.id = id.second;
		noti.stage = player->stage->id;
		noti.unit_type = UNIT_PLAYER_ME;
		noti.x = player->position.x;
		noti.y = player->position.y;
		client->sendPacket(noti);
	}

	flush();

	for (auto id : clientIds) {
		Unit* player = getUnit(id.second);
		SpawnUnit noti;
		noti.id = id.second;
		noti.stage = player->stage->id;
		noti.unit_type = UNIT_PLAYER;
		noti.x = player->position.x;
		noti.y = player->position.y;
		sendPacket(noti);
	}

	for (int i = 0; i < Max::Teams; ++i)
		stage[i]->trashPool->spawn(clientIds.size() * config::start_trash);

	if (config::gui) {
		window = SDL_CreateWindow(
			"dbgwindow",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			400, 300, 0);
		renderer = SDL_CreateRenderer(
			window, -1, 0);
	}

	return true;
}


id_t GameRoom::addUnit(Unit* unit) {
	if (!unit)
		return INVALID_ID;
	if (!addUnits.insert(unit).second)
		return unit->id;
	id_t id = dispenser.issue();
	unit->id = id;
	return id;
}

void GameRoom::removeUnit(Unit* unit) {
	if (!unit) return;
	removeUnits.insert(unit);
}

void GameRoom::addUnitImmediate(Unit* unit) {
	if (!unit) return;
	
	auto it = std::find(units.begin(), units.end(), unit);
	if (it != units.end())
		return;

	units.push_back(unit);
	unit->stage->addUnitImmediate(unit);

	/*
	SpawnUnit noti;
	noti.id = unit->id;
	noti.unit_type = unit->type;
	if (unit->type == UT_ENEMY) {
		Enemy* enemy = (Enemy*)unit;
		noti.unit_type = 10 + enemy->getEnemyType();
	}
	noti.x = unit->position.x;
	noti.y = unit->position.y;
	sendPacket(noti);
	*/
}

void GameRoom::removeUnitImmediate(Unit* unit) {
	if (!unit) return;

	auto it = std::find(units.begin(), units.end(), unit);
	if (it == units.end())
		return;

	unit->stage->removeUnitImmediate(unit);
	units.erase(it);
	SAFE_DELETE(unit);
}

void GameRoom::flush() {
	for (Unit* u : removeUnits)
		removeUnitImmediate(u);
	removeUnits.clear();

	for (Unit* u : addUnits)
		addUnitImmediate(u);
	addUnits.clear();

}

Unit* GameRoom::getUnit(const id_t id) {
	auto it = std::find_if(units.begin(), units.end(), [id](Unit* u){ return u->id == id; });
	if (it == units.end())
		return nullptr;
	return *it;
}

Unit* GameRoom::getClientUnit(const id_t client_id) {
	return getUnit(GameRoom::getClientUnitId(client_id));
}

id_t GameRoom::getClientUnitId(const id_t client_id) {
	auto it = clientIds.find(client_id);
	if (it == clientIds.end())
		return INVALID_ID;
	return it->second;
}


int GameRoom::send(void* const buf, const size_t size) {
	if (!buf || size < 1)
		return SOCKET_ERROR;
	for (auto id : clientIds) {
		Client* client = ClientManager::getInstance()->getClient(id.first);
		if (client)
		{
			if (client->send(buf, size) == SOCKET_ERROR)
				return SOCKET_ERROR;
		}
	}
	return 0;
}


IdPairMap::const_iterator GameRoom::begin() const {
	return clientIds.begin();
}

IdPairMap::const_iterator GameRoom::end() const {
	return clientIds.end();
}

bool GameRoom::empty() const {
	return clientIds.empty();
}

size_t GameRoom::size() const {
	return clientIds.size();
}

bool GameRoom::isPlaying() {
	return gameRunning;
}
