#pragma once

#include <RakPeerInterface.h>
#include <glm\glm.hpp>

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

	CollisionDirection CheckCollision(glm::vec2 ballPos);

	void Break();

#ifdef NETWORKING_SERVER
	void SendData(int brickID, RakNet::RakPeerInterface* pPeerInterface);

	int scoreWorth = 0;
#endif

	glm::vec2 m_position;
	glm::vec4 m_colour;

	bool m_isAlive;
};

