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

bool Brick::CheckCollision(glm::vec2 ballPos, glm::vec2 ballVelocity)
{
	if (!m_isAlive)
		return false;

	/*glm::vec2 pt = ballPos;

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
	{
		if (glm::distance(pt.x, ballPos.x) <= glm::distance(pt.y, ballPos.y))
			return Side;
		else
			return Top;
	}*/

	float ballXMin = ballPos.x - BALL_RADIUS;
	float ballXMax = ballPos.x + BALL_RADIUS;
	float ballYMin = ballPos.y - BALL_RADIUS;
	float ballYMax = ballPos.y + BALL_RADIUS;

	float brickXMin = m_position.x - BRICK_WIDTH;
	float brickXMax = m_position.x + BRICK_WIDTH;
	float brickYMin = m_position.y - BRICK_HEIGHT;
	float brickYMax = m_position.y + BRICK_HEIGHT;

	if (ballXMin < brickXMax && ballXMax > brickXMin &&
		ballYMin < brickYMax && ballYMax > brickYMin)
	{
		return true;
	}

	return false;
}

void Brick::Break()
{
	m_isAlive = false;

	std::cout << "Brick broken" << std::endl;
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
	bs.Write((char*)&m_isAlive, sizeof(bool));

	std::cout << "Sent brick data" << brickID << std::endl;

	//Broadcast packet (should be picked up by the server)
	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}
#endif