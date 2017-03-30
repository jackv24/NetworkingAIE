#pragma once

#include <RakPeerInterface.h>
#include <glm\glm.hpp>

#include "GameObject.h"
#include "Player.h"
#include "Ball.h"

class Server
{
public:
	Server();
	~Server();

	void Startup();
	void Run();

	void SendNewClientID(RakNet::RakPeerInterface* pPeerInterface, RakNet::SystemAddress& address);
	void HandleNetworkMessages(RakNet::RakPeerInterface* pPeerInterface);

	static void SimulateGame(Server* s, RakNet::RakPeerInterface* pPeerInterface);

	bool playerOneConnected;
	bool playerTwoConnected;

	Player playerOne;
	Player playerTwo;

	Ball ballOne;
	Ball ballTwo;
};

