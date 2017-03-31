#pragma once

#include <RakPeerInterface.h>
#include <glm\glm.hpp>

class Brick
{
public:
	Brick();
	~Brick();

	bool CheckCollision(glm::vec2 ballPos);

#ifdef NETWORKING_SERVER
	void SendData(int brickID, RakNet::RakPeerInterface* pPeerInterface);
#endif

	glm::vec2 m_position;
	glm::vec4 m_colour;
};

