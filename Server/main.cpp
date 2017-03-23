#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unordered_map>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include "GameMessages.h"
#include "GameObject.h"

int nextClientID = 1;

std::unordered_map<int, GameObject> clientGameObjects;

void SendNewClientID(RakNet::RakPeerInterface* pPeerInterface, RakNet::SystemAddress& address)
{
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_SET_CLIENT_ID);
	bs.Write(nextClientID);

	clientGameObjects[nextClientID] = GameObject();

	nextClientID++;

	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, address, false);
}

void SendClientPing(RakNet::RakPeerInterface* pPeerInterface)
{
	while (true)
	{
		RakNet::BitStream bs;
		bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_TEXT_MESSAGE);
		bs.Write("Ping!");

		pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void HandeNetworkMessages(RakNet::RakPeerInterface* pPeerInterface)
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
				for (auto& client : clientGameObjects)
				{
					RakNet::BitStream bs;
					bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_CLIENT_DATA);
					bs.Write(client.first);
					bs.Write((char*)&client.second, sizeof(GameObject));

					pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
				}
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "A client has disconnected." << std::endl;
				break;
			case ID_CONNECTION_LOST:
				std::cout << "A client lost the connection." << std::endl;
				break;
			case ID_CLIENT_CLIENT_DATA:
			{
				RakNet::BitStream bs(packet->data, packet->length, false);
				pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);

				bs.IgnoreBytes(sizeof(RakNet::MessageID));

				int clientID;
				bs.Read(clientID);

				GameObject clientData;
				bs.Read((char*)&clientData, sizeof(GameObject));

				clientGameObjects[clientID] = clientData;

				//Output gameobject information to the console
				std::cout << "Client " << clientID <<
					" at: " << clientData.m_position.x << " " << clientData.m_position.z << std::endl;

				break;
			}
			default:
				std::cout << "Received a message with an unknown ID: " << packet->data[0];
				break;
			}
		}
	}
}

int main()
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

	//Startup a thread to ping clients every second
	//std::thread pingThread(SendClientPing, pPeerInterface);

	HandeNetworkMessages(pPeerInterface);

	return 0;
}