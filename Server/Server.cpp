#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "Server.h"

#include <MessageIdentifiers.h>
#include <BitStream.h>
#include "GameMessages.h"

Server::Server()
{
}

Server::~Server()
{
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

	HandeNetworkMessages(pPeerInterface);
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

void Server::HandeNetworkMessages(RakNet::RakPeerInterface* pPeerInterface)
{
	RakNet::Packet* packet = nullptr;

	while (true)
	{
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
						bsIn.Read((char*)&playerOne, sizeof(Player));
					else if(clientID == 2)
						bsIn.Read((char*)&playerTwo, sizeof(Player));

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
}