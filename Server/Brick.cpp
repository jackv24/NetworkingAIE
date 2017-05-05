#include <iostream>
#include <BitStream.h>

#include "GameMessages.h"
#include "GameConstants.h"
#include "Server.h"

Brick::Brick()
{
}

Brick::~Brick()
{
}

Brick::CollisionDirection Brick::CheckCollision(glm::vec2 ballPos, int ballOwner)
{
	if (!m_isAlive)
		return None;

	//If Collision detected
	if (ballPos.x - BALL_RADIUS < m_position.x + BRICK_WIDTH &&
		ballPos.x + BALL_RADIUS > m_position.x - BRICK_WIDTH &&
		ballPos.y - BALL_RADIUS < m_position.y + BRICK_HEIGHT &&
		ballPos.y + BALL_RADIUS > m_position.y - BRICK_HEIGHT)
	{
		//Determine direction
		//Get distance to each side
		float distanceRight = abs((ballPos.x - BALL_RADIUS) - (m_position.x + BRICK_WIDTH));
		float distanceLeft = abs((ballPos.x + BALL_RADIUS) - (m_position.x - BRICK_WIDTH));
		float distanceUp = abs((ballPos.y - BALL_RADIUS) - (m_position.y + BRICK_HEIGHT));
		float distanceDown = abs((ballPos.y + BALL_RADIUS) - (m_position.y - BRICK_HEIGHT));

		float distX = 0;
		float distY = 0;

		//Determine which distance to use for top and side (the smallest)
		if (ballPos.x > m_position.x)
			distX = distanceRight;
		else
			distX = distanceLeft;

		if (ballPos.y > m_position.y)
			distY = distanceUp;
		else
			distY = distanceDown;

#ifdef NETWORKING_SERVER
		newBallPosition = ballPos;

		if (ballOwner == 1)
			newBallVelocity = glm::vec2(-BALL_RETURN_SPEED, 0);
		else if (ballOwner == 2)
			newBallVelocity = glm::vec2(BALL_RETURN_SPEED, 0);
#endif

		//Which out of top and side distances is smallest?
		if (distX < distY)
			return Side;
		else
			return Top;
	}

	return None;
}

void Brick::Break()
{
	m_isAlive = false;

	std::cout << "Brick broken" << std::endl;

#ifdef NETWORKING_SERVER
	if (UsePowerup)
		UsePowerup(*this, server);
#endif
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