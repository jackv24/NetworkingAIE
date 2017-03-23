#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::GameObject(int clientID, glm::vec3 position, glm::vec4 colour)
{
	m_clientID = clientID;
	m_position = position;
	m_colour = colour;
}

GameObject::~GameObject()
{
}
