#pragma once

#include <RakPeerInterface.h>

class Player
{
public:
	Player();
	~Player();

	void Move(float deltaTime);

	void SendData(int clientID, RakNet::RakPeerInterface* pPeerInterface);

	int m_id;

	int moveDir = 0;
	float yPos;

	int score = 0;

	bool m_isReady = false;
};

