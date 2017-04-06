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

void Ball::Update(float deltaTime, Player &leftPlayer, Player &rightPlayer, std::unordered_map<int, Brick>* bricks)
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
	//left paddle horizontal check
	if (m_position.x - BALL_RADIUS < -PADDLE_DISTANCE + PADDLE_WIDTH &&
		m_position.x - BALL_RADIUS > -PADDLE_DISTANCE &&
		m_velocity.x < 0 && m_position.x < 0)
	{
		//Vertical check
		if (m_position.y > leftPlayer.yPos - PADDLE_HEIGHT && m_position.y < leftPlayer.yPos + PADDLE_HEIGHT)
		{
			m_velocity.x *= -1;
			m_hasBounced = true;
			SetOwner(leftPlayer);
		}
	}
	//right paddle horizontal check
	else if (
		m_position.x + BALL_RADIUS > PADDLE_DISTANCE - PADDLE_WIDTH &&
		m_position.x + BALL_RADIUS < PADDLE_DISTANCE &&
		m_velocity.x > 0 && m_position.x > 0)
	{
		//Vertical check
		if (m_position.y > rightPlayer.yPos - PADDLE_HEIGHT && m_position.y < rightPlayer.yPos + PADDLE_HEIGHT)
		{
			m_velocity.x *= -1;
			m_hasBounced = true;
			SetOwner(rightPlayer);
		}
	}

	//Brick check collision
	for (auto &brick : *bricks)
	{
		Brick::CollisionDirection dir = brick.second.CheckCollision(m_position, m_velocity);

		if (dir != Brick::CollisionDirection::None)
		{
			m_hasBounced = true;

			//TODO: Bounce ball off of bricks
			switch (dir)
			{
			case Brick::CollisionDirection::Side:
				m_velocity.x *= -1;
				break;
			case Brick::CollisionDirection::Top:
				m_velocity.y *= -1;
				break;
			}

			brick.second.Break();

#ifdef NETWORKING_SERVER
			AddScore(brick.second.scoreWorth);

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

void Ball::SetOwner(Player &owner)
{
	m_ownerID = owner.m_id;
	m_colour = m_ownerID == 1 ? glm::vec4(0.26f, 0.55f, 0.8f, 1) : glm::vec4(0.78f, 0.23f, 0.25f, 1);

#ifdef NETWORKING_SERVER
	AddScore(BALL_BOUNCE_SCORE);
#endif
}

void Ball::SetOwner(int id)
{
	m_ownerID = id;
	m_colour = m_ownerID == 1 ? glm::vec4(0.26f, 0.55f, 0.8f, 1) : glm::vec4(0.78f, 0.23f, 0.25f, 1);
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
	bs.Write((char*)&m_colour, sizeof(glm::vec4));

	std::cout << "Sent ball data" << m_id << std::endl;

	//Broadcast packet (should be picked up by the server)
	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void Ball::AddScore(int amount)
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_SCORE_DATA);

	bs.Write(m_ownerID);
	bs.Write(amount);

	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}
#endif