#pragma once

#include <RakPeerInterface.h>
#include <glm\glm.hpp>
#include <unordered_map>

#include "Brick.h"
#include "Player.h"

class Ball
{
public:
	Ball();
	Ball(int id, glm::vec2 position, glm::vec2 initialVelocity);
	~Ball();

	void Update(float deltaTime, Player &leftPlayer, Player &rightPlayer, std::unordered_map<int, Brick>* bricks);
	void SetOwner(Player &owner);
	void SetOwner(int id);

#ifdef NETWORKING_SERVER
	void SendData();
	void AddScore(int amount);

	RakNet::RakPeerInterface* pPeerInterface;
#endif

	//Data to send/recieve
	int m_id;
	glm::vec2 m_position;
	glm::vec2 m_velocity;

	bool m_hasBounced;

	int m_ownerID;
	glm::vec4 m_colour = glm::vec4(1,1,1,1);
};

