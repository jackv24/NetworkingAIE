#include <BitStream.h>

#include "GameMessages.h"
#include "Ball.h"

Ball::Ball()
{
	m_velocity = glm::vec2(0);
}

Ball::Ball(int id, glm::vec2 position, glm::vec2 initialVelocity)
{
	m_velocity = initialVelocity;

	m_id = id;
	m_position = position;
}

Ball::~Ball()
{
}

#ifdef NETWORKING_SERVER
void Ball::Update(float deltaTime)
{
	m_position += m_velocity * deltaTime;
}

void Ball::SendData(RakNet::RakPeerInterface* pPeerInterface)
{
	//Create new bitstream with client data ID
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_BALL_DATA);

	//Write client ID and data to stream
	bs.Write(m_id);
	bs.Write((char*)&m_position, sizeof(glm::vec2));

	//Broadcast packet (should be picked up by the server)
	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}
#endif