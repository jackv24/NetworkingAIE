#pragma once

#include <RakPeerInterface.h>
#include "GameObject.h"
#include "Player.h"

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

	Player playerOne;
	Player playerTwo;
};

