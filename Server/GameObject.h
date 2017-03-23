#pragma once

#include <glm\glm.hpp>
#include <RakPeerInterface.h>

class GameObject
{
public:
	GameObject();
	GameObject(glm::vec3 position, glm::vec4 colour);
	~GameObject();

#ifndef NETWORKING_SERVER
	void Draw();
#endif // !NETWORKING_SERVER

	glm::vec3 m_position;
	glm::vec4 m_colour;

	RakNet::SystemAddress m_networkAddress;
};

