#include <BitStream.h>

#include "GameMessages.h"
#include "GameConstants.h"
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
void Ball::Update(float deltaTime, float leftPaddlePos, float rightPaddlePos)
{
	m_position += m_velocity * deltaTime;

	//Bounce off stage top and bottom
	if (m_position.y + BALL_RADIUS > STAGE_HEIGHT && m_velocity.y > 0)
		m_velocity.y *= -1;
	else if (m_position.y - BALL_RADIUS < -STAGE_HEIGHT && m_velocity.y < 0)
		m_velocity.y *= -1;

	//Bounce off paddles
	//Right paddle horizontal check
	if (m_position.x - BALL_RADIUS < -PADDLE_DISTANCE + PADDLE_WIDTH &&
		m_position.x - BALL_RADIUS > -PADDLE_DISTANCE &&
		m_velocity.x < 0)
	{
		//Vertical check
		if (m_position.y > leftPaddlePos - PADDLE_HEIGHT && m_position.y < leftPaddlePos + PADDLE_HEIGHT)
			m_velocity.x *= -1;
	}
	//Left paddle horizontal check
	else if (
		m_position.x + BALL_RADIUS > PADDLE_DISTANCE - PADDLE_WIDTH &&
		m_position.x + BALL_RADIUS < PADDLE_DISTANCE &&
		m_velocity.x > 0)
	{
		//Vertical check
		if (m_position.y > rightPaddlePos - PADDLE_HEIGHT && m_position.y < rightPaddlePos + PADDLE_HEIGHT)
			m_velocity.x *= -1;
	}
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