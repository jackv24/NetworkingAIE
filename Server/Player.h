#pragma once

#include <RakPeerInterface.h>

class Player
{
public:
	Player();
	~Player();

	void SendData(int clientID, RakNet::RakPeerInterface* pPeerInterface);

	float yPos;

	int score;
};

