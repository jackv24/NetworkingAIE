#include <iostream>
#include <BitStream.h>

#include "GameMessages.h"
#include "GameConstants.h"
#include "Brick.h"

Brick::Brick()
{
}

Brick::~Brick()
{
}

bool Brick::CheckCollision(glm::vec2 ballPos)
{
	//NOT SURE IF THIS WORKS YET
	glm::vec2 pt = ballPos;

	float rectRight = m_position.x + BRICK_WIDTH;
	float rectLeft = m_position.x - BRICK_WIDTH;
	float rectTop = m_position.y + BRICK_HEIGHT;
	float rectBottom = m_position.y - BRICK_HEIGHT;

	if (ballPos.x > rectRight)
		ballPos.x = rectRight;
	if (ballPos.x < rectLeft)
		ballPos.x = rectLeft;
	if (ballPos.y > rectBottom)
		ballPos.y = rectBottom;
	if (ballPos.y < rectTop)
		ballPos.y = rectTop;

	if (glm::distance(pt, ballPos) < BALL_RADIUS)
		return true;

	return false;
}

#ifdef NETWORKING_SERVER
void Brick::SendData(int brickID, RakNet::RakPeerInterface * pPeerInterface)
{
	//Create new bitstream with client data ID
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_BRICK_DATA);

	//Write client ID and data to stream
	bs.Write(brickID);
	bs.Write((char*)&m_position, sizeof(glm::vec2));
	bs.Write((char*)&m_colour, sizeof(glm::vec4));

	std::cout << "Sent brick data" << brickID << std::endl;

	//Broadcast packet (should be picked up by the server)
	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}
#endif