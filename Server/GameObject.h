#pragma once

#include <glm\glm.hpp>

class GameObject
{
public:
	GameObject();
	GameObject(int clientID, glm::vec3 position, glm::vec4 colour);
	~GameObject();

	glm::vec3 m_position;
	glm::vec4 m_colour;

	int m_clientID;
};

