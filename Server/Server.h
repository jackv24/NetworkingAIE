#pragma once

#include <RakPeerInterface.h>
#include <glm\glm.hpp>

#include <unordered_map>

#include "GameObject.h"
#include "Player.h"
#include "Ball.h"
#include "Brick.h"

class Server
{
public:
	Server();
	~Server();

	void Startup();
	void Run();

	void SendNewClientID(RakNet::RakPeerInterface* pPeerInterface, RakNet::SystemAddress& address);
	void HandleNetworkMessages(RakNet::RakPeerInterface* pPeerInterface);

	void GenerateBricks();
	static void SimulateGame(Server* s, RakNet::RakPeerInterface* pPeerInterface);

	bool playerOneConnected;
	bool playerTwoConnected;

	Player playerOne;
	Player playerTwo;

	std::unordered_map<int, Ball> balls;

	std::unordered_map<int, Brick> bricks;

	bool gameRunning = true;
};

