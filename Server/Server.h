#pragma once

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

	bool playerOneConnected = false;
	bool playerTwoConnected = false;

	GameObject playerOne;
	GameObject playerTwo;
};

