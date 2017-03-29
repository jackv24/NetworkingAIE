#include <BitStream.h>

#include <Gizmos.h>

#include "GameMessages.h"
#include "GameObject.h"

GameObject::GameObject()
{
}

GameObject::GameObject(glm::vec3 position, glm::vec4 colour)
{
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

void GameObject::SendData(int clientID, RakNet::RakPeerInterface* pPeerInterface)
{
	//Create new bitstream with client data ID
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CLIENT_DATA);

	//Write client ID and data to stream
	bs.Write(clientID);
	bs.Write((char*)this, sizeof(GameObject));

	//Broadcast packet (should be picked up by the server)
	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}