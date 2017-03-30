#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "Server.h"

#include <MessageIdentifiers.h>
#include <BitStream.h>
#include "GameMessages.h"

const float SERVER_UPDATE_INTERVAL = 0.001f;

Server::Server()
{
}

Server::~Server()
{
}

void Server::Startup()
{
	playerOneConnected = false;
	playerTwoConnected = false;

	playerOne.yPos = 0;
	playerTwo.yPos = 0;

	ballOne = Ball(1, glm::vec2(-5, 0), glm::vec2(-5, 5));
	ballTwo = Ball(2, glm::vec2(5, 0), glm::vec2(5, -5));
}

void Server::Run()
{
	const unsigned short PORT = 5456;
	RakNet::RakPeerInterface* pPeerInterface = nullptr;

	//Startup server and start it listening for clients
	std::cout << "Starting up the server..." << std::endl;

	//Initialize the Raknet peer interface first
	pPeerInterface = RakNet::RakPeerInterface::GetInstance();

	//Create a socket descriptor to describe this connection
	RakNet::SocketDescriptor sd(PORT, 0);

	//Call startup - max of 32 connections, on the assigned port
	pPeerInterface->Startup(32, &sd, 1);
	pPeerInterface->SetMaximumIncomingConnections(32);

	//Set peer interface to send data for balls
	ballOne.pPeerInterface = pPeerInterface;
	ballTwo.pPeerInterface = pPeerInterface;

	//Balls "have bounced" to start, so they send initial data
	ballOne.m_hasBounced = true;
	ballTwo.m_hasBounced = true;

	std::thread tickThread(SimulateGame, this, pPeerInterface);

	while (true)
	{
		HandleNetworkMessages(pPeerInterface);
	}
}

void Server::SendNewClientID(RakNet::RakPeerInterface* pPeerInterface, RakNet::SystemAddress& address)
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_SET_CLIENT_ID);
	
	//If there are no player slots available
	int id = 0;

	//New ID is for available slot
	if (!playerOneConnected)
	{
		id = 1;
		playerOneConnected = true;
	}
	else if (!playerTwoConnected)
	{
		id = 2;
		playerTwoConnected = true;
	}

	bs.Write(id);

	//Send Set ID packet to specified address
	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
}

void Server::HandleNetworkMessages(RakNet::RakPeerInterface* pPeerInterface)
{
	RakNet::Packet* packet = nullptr;

	for (packet = pPeerInterface->Receive(); packet; pPeerInterface->DeallocatePacket(packet), packet = pPeerInterface->Receive())
	{
		switch (packet->data[0])
		{
		case ID_NEW_INCOMING_CONNECTION:
			std::cout << "A connection is incoming." << std::endl;
			SendNewClientID(pPeerInterface, packet->systemAddress);

			//Send initial data
			for (int i = 1; i <= 2; i++)
			{
				if (i == 1 && playerOneConnected)
					playerOne.SendData(i, pPeerInterface);
				else if (i == 2 && playerTwoConnected)
					playerTwo.SendData(i, pPeerInterface);
				else
					continue;

			}
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "A client has disconnected." << std::endl;
			break;
		case ID_CONNECTION_LOST:
		{
			std::cout << "A client lost the connection." << std::endl;
			break;
		}
		case ID_CLIENT_PLAYER_DATA:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			//Message was identified, so remove message ID from packet
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			//Get client ID
			int clientID = 0;
			bsIn.Read(clientID);

			//Make sure this client is valid
			if (clientID > 0)
			{
				//Read data into either player one or two
				if(clientID == 1)
					bsIn.Read(playerOne.yPos);
				else if(clientID == 2)
					bsIn.Read(playerTwo.yPos);

				//Relay data
				RakNet::BitStream bsOut(packet->data, packet->length, false);
				pPeerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
				
				//Print data to console for debugging
				Player client = clientID == 1 ? playerOne : playerTwo;
				std::cout << "Client " << clientID <<
					" at: " << client.yPos << std::endl;
			}
			break;
		}
		default:
			std::cout << "Received a message with an unknown ID: " << packet->data[0];
			break;
		}
	}
}

void Server::SimulateGame(Server* s, RakNet::RakPeerInterface* pPeerInterface)
{
	while (true)
	{
		//Update balls
		if (s->playerOneConnected && s->playerTwoConnected)
		{
			//Update balls on server, send data when they bounce
			s->ballOne.Update(SERVER_UPDATE_INTERVAL, s->playerOne.yPos, s->playerTwo.yPos);
			s->ballTwo.Update(SERVER_UPDATE_INTERVAL, s->playerOne.yPos, s->playerTwo.yPos);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds((int)(SERVER_UPDATE_INTERVAL * 1000)));
	}
}