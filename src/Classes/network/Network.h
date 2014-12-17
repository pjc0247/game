﻿#pragma once

#include <string>
#include <thread>
#include <functional>
#include <map>

#include <WinSock2.h>
#include <Windows.h>

#include "Packet.h"

class NetworkSender{
};
class NetworkHandler{
};

class Network{
public:
	typedef std::function<void(int)> OpenHandler;

public:
	static Network *create();
	static void purge();
	static Network *getInstance();

	void open(
		const std::string &host, int port,
		OpenHandler handler);
	void close();

	bool isConnected();

	void sendLoginRequest(
		const char *user_id, const char *user_pw);
	void sendMove(
		float direction_x, float direction_y);
	void sendSyncRotation(
		float angle);
	void sendEnterRoom();
	void sendLeaveRoom();
	void sendReady(
		int robot_id);

	void sendSpawnRequest(
		int unitType);

	void sendUseSkill(
		int skillID,
		float x,float y);

	void sendUpgrade(
		UpgradeType upgradeType);

	void sendChatMessage(
		const char *msg);

	template <typename T>
	void route(
		int pid,
		std::function<void(T*)> callback){

		handlers[pid] = std::bind(
			&Network::invoker<T>,
			this,
			callback, std::placeholders::_1);
	};

protected:
	Network();
	virtual ~Network();

	void initHandlers();

	void handleEnterRoomNoti(
		EnterNoti *pkt);
	void handleLeaveRoomNoti(
		LeaveNoti *pkt);
	void handleReadyRequest(
		ReadyRequest *pkt);
	void handleStartGame(
		StartGame *pkt);
	void handleSpawn(
		SpawnUnit *pkt);
	void handleRemoveUnit(
		RemoveUnit *pkt);

	void handleUseSkillNoti(
		UseSkillNoti *pkt);
	void handleUpgrade(
		UpgradeNoti *pkt);

	void handleMove(
		MoveNoti *pkt);
	void handleSyncRotationNoti(
		SyncRotationNoti *pkt);
	void handleChatNoti(
		ChatNoti *pkt);

	int recv(
		void *dst, int len);
	int send(
		void *src, int len);

	template <typename T>
	int send(
		T &packet){
		return send(&packet, sizeof(packet));
	}

	void worker(
		const std::string &host, int port,
		OpenHandler handler);
	void recvLoop();

	void throwTaskToGameThread(
		std::function<void()> task);

	template <typename T>
	void invoker(
		std::function<void(T*)> f,
		PacketHeader* arg){

		f((T*)arg);
		free(arg);
	}

private:
	std::thread *trd;
	std::map<int,std::function<void(PacketHeader*)>> handlers;

	SOCKET sock;

	bool connected;
};