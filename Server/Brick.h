#pragma once

#include <RakPeerInterface.h>
#include <glm\glm.hpp>

class Server;

class Brick
{
public:
	Brick();
	~Brick();

	enum CollisionDirection
	{
		None,
		Side,
		Top
	};

	CollisionDirection CheckCollision(glm::vec2 ballPos, int ballOwner);

	void Break();

#ifdef NETWORKING_SERVER
	void SendData(int brickID, RakNet::RakPeerInterface* pPeerInterface);
	
	void(*UsePowerup)(const Brick&, Server*) = nullptr;

	int scoreWorth = 0;

	glm::vec2 newBallPosition;
	glm::vec2 newBallVelocity;

	Server* server = nullptr;
#endif

	glm::vec2 m_position;
	glm::vec4 m_colour;

	bool m_isAlive;
};

