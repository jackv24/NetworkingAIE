#pragma once

#include <RakPeerInterface.h>
#include <glm\glm.hpp>
#include <unordered_map>

#include "Brick.h"

class Ball
{
public:
	Ball();
	Ball(int id, glm::vec2 position, glm::vec2 initialVelocity);
	~Ball();

	void Update(float deltaTime, float leftPaddlePos, float rightPaddlePos, std::unordered_map<int, Brick>* bricks);

#ifdef NETWORKING_SERVER
	void SendData();

	RakNet::RakPeerInterface* pPeerInterface;
#endif

	//Data to send/recieve
	int m_id;
	glm::vec2 m_position;
	glm::vec2 m_velocity;

	bool m_hasBounced;
};

