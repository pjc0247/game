﻿#include "pch.h"
#include "Network.h"
#include "GameRoom.h"

void Network::sendRoomRequest(){
	RoomRequest pkt;
	send(pkt);
}
void Network::sendEnterRoom(
	int room_id){
	EnterRoom pkt;
	pkt.room_id = room_id;
	send(pkt);
}
void Network::sendLeaveRoom(){
	LeaveRoom pkt;
	send(pkt);
	GameRoom::getInstance()->clear();
}
void Network::sendSelectRobot(
	int robot_id) {
	SelectRobot pkt;
	pkt.robot_id = robot_id;
	send(pkt);
}
void Network::sendSelectTeam(
	int team_id) {
	SelectTeam pkt;
	pkt.team_id = team_id;
	send(pkt);
}
void Network::sendReadyRequest(
	bool ready){

	ReadyRequest pkt;
	pkt.ready = ready ? 1 : 0;
	send(pkt);
}
