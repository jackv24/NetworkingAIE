#include "GameObject.h"
#include <Gizmos.h>

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

#ifndef NETWORKING_SERVER
void GameObject::Draw()
{
	aie::Gizmos::addSphere(m_position, 1.0f, 28, 32, m_colour);
}
#endif