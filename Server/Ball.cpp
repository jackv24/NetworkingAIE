#include <iostream>
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
	m_id = id;
	m_position = position;
	m_velocity = initialVelocity;
}

Ball::~Ball()
{
}

void Ball::Update(float deltaTime, float leftPaddlePos, float rightPaddlePos, std::unordered_map<int, Brick>* bricks)
{
	m_position += m_velocity * deltaTime;

	//Bounce off stage top and bottom
	if (m_position.y + BALL_RADIUS > STAGE_HEIGHT && m_velocity.y > 0)
	{
		m_velocity.y *= -1;
		m_hasBounced = true;
	}
	else if (m_position.y - BALL_RADIUS < -STAGE_HEIGHT && m_velocity.y < 0)
	{
		m_velocity.y *= -1;
		m_hasBounced = true;
	}

	//Bounce off paddles
	//Right paddle horizontal check
	if (m_position.x - BALL_RADIUS < -PADDLE_DISTANCE + PADDLE_WIDTH &&
		m_position.x - BALL_RADIUS > -PADDLE_DISTANCE &&
		m_velocity.x < 0)
	{
		//Vertical check
		if (m_position.y > leftPaddlePos - PADDLE_HEIGHT && m_position.y < leftPaddlePos + PADDLE_HEIGHT)
		{
			m_velocity.x *= -1;
			m_hasBounced = true;
		}
	}
	//Left paddle horizontal check
	else if (
		m_position.x + BALL_RADIUS > PADDLE_DISTANCE - PADDLE_WIDTH &&
		m_position.x + BALL_RADIUS < PADDLE_DISTANCE &&
		m_velocity.x > 0)
	{
		//Vertical check
		if (m_position.y > rightPaddlePos - PADDLE_HEIGHT && m_position.y < rightPaddlePos + PADDLE_HEIGHT)
		{
			m_velocity.x *= -1;
			m_hasBounced = true;
		}
	}

	//Brick check collision
	for (auto brick : *bricks)
	{
		if (brick.second.CheckCollision(m_position, m_velocity))
		{
			m_hasBounced = true;

			//TODO: Bounce ball off of bricks

			brick.second.Break();

#ifdef NETWORKING_SERVER
			brick.second.SendData(brick.first, pPeerInterface);
#endif
		}
	}

	if (m_hasBounced)
	{
		m_hasBounced = false;
#ifdef NETWORKING_SERVER
		SendData();
#endif
	}
}

#ifdef NETWORKING_SERVER
void Ball::SendData()
{
	//Create new bitstream with client data ID
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_BALL_DATA);

	//Write client ID and data to stream
	bs.Write(m_id);
	bs.Write((char*)&m_position, sizeof(glm::vec2));
	bs.Write((char*)&m_velocity, sizeof(glm::vec2));

	std::cout << "Sent ball data" << m_id << std::endl;

	//Broadcast packet (should be picked up by the server)
	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}
#endif