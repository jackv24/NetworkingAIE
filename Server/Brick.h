#pragma once

#include <RakPeerInterface.h>
#include <glm\glm.hpp>

class Brick
{
public:
	Brick();
	~Brick();


	bool CheckCollision(glm::vec2 ballPos, glm::vec2 ballVelocity);

	void Break();

#ifdef NETWORKING_SERVER
	void SendData(int brickID, RakNet::RakPeerInterface* pPeerInterface);
#endif

	glm::vec2 m_position;
	glm::vec4 m_colour;

	bool m_isAlive;
};

