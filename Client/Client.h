#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <RakPeerInterface.h>

#include <unordered_map>

#include <Player.h>
#include <Ball.h>
#include <Brick.h>

class Client : public aie::Application {
public:

	Client();
	virtual ~Client();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void HandleNetworkConnection();
	void InitialiseClientConnection();

	void HandleNetworkMessages();

	void OnSetClientIDPacket(RakNet::Packet* packet);
	void OnReceivedClientDataPacket(RakNet::Packet* packet);
	void OnReceivedBallDataPacket(RakNet::Packet* packet);
	void OnReceivedBrickDataPacket(RakNet::Packet* packet);

	const char* IP = "127.0.0.1";

protected:
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	RakNet::RakPeerInterface* m_pPeerInterface;

	const unsigned short PORT = 5456;

	int m_clientID = 0;
	Player m_myPlayer;

	Player m_otherPlayer;

	std::unordered_map<int, Ball> m_balls;

	std::unordered_map<int, Brick> m_bricks;

	bool m_gameRunning = true;
};