#pragma once

#include <unordered_map>

#include <RakPeerInterface.h>
#include "GameObject.h"

class Server
{
public:
	Server();
	~Server();

	void Run();

	void SendNewClientID(RakNet::RakPeerInterface* pPeerInterface, RakNet::SystemAddress& address);
	void HandeNetworkMessages(RakNet::RakPeerInterface* pPeerInterface);

	int nextClientID = 1;

	std::unordered_map<int, GameObject> clientGameObjects;
};

