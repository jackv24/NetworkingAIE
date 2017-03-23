#pragma once

#include <glm\glm.hpp>
#include <RakPeerInterface.h>

class GameObject
{
public:
	GameObject();
	GameObject(int clientID, glm::vec3 position, glm::vec4 colour);
	~GameObject();

#ifndef NETWORKING_SERVER
	void Draw();
#endif // !NETWORKING_SERVER

	glm::vec3 m_position;
	glm::vec4 m_colour;

	int m_clientID;

	RakNet::SystemAddress m_networkAddress;
};

