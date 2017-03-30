#pragma once

#include <RakPeerInterface.h>
#include <glm\glm.hpp>

class Ball
{
public:
	Ball();
	Ball(int id, glm::vec2 position, glm::vec2 initialVelocity);
	~Ball();

#ifdef NETWORKING_SERVER
	void Update(float deltaTime);
	void SendData(RakNet::RakPeerInterface* pPeerInterface);
#endif

	glm::vec2 m_velocity;

	//Data to send/recieve
	int m_id;
	glm::vec2 m_position;
};

