#pragma once

#include <unordered_map>

#include "Application.h"
#include <glm/glm.hpp>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

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

	void SendClientGameObject();

protected:
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	RakNet::RakPeerInterface* m_pPeerInterface;

	const char* IP = "127.0.0.1";
	const unsigned short PORT = 5456;

	struct GameObject
	{
		glm::vec3 position;
		glm::vec4 colour;
	};

	GameObject m_myGameObject;

	int m_myClientID;

	std::unordered_map<int, GameObject> m_otherClientGameObjects;
};