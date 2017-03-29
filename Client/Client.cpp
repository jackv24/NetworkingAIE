#include <iostream>

#include <BitStream.h>
#include "Gizmos.h"
#include "Input.h"

#include "Client.h"
#include <GameMessages.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

const float WIDTH = 50.0f;
const float HEIGHT = 30.0f;

const float STAGE_HEIGHT = 25.0f;

const float PADDLE_HEIGHT = 5.0f;

Client::Client()
{
}

Client::~Client()
{
}

bool Client::startup()
{
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(0, 0, 50), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	srand((unsigned int)time(NULL));

	//Connect to server
	HandleNetworkConnection();

	return true;
}

void Client::shutdown()
{

	Gizmos::destroy();
}

void Client::update(float deltaTime)
{

	// query time since application started
	float time = getTime();

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	
	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	bool hasMoved = false;

	if (input->isKeyDown(aie::INPUT_KEY_UP))
	{
		m_myPlayer.yPos += 20.0f * deltaTime;

		if (m_myPlayer.yPos > STAGE_HEIGHT - PADDLE_HEIGHT)
			m_myPlayer.yPos = STAGE_HEIGHT - PADDLE_HEIGHT;

		hasMoved = true;
	}
	if (input->isKeyDown(aie::INPUT_KEY_DOWN))
	{
		m_myPlayer.yPos -= 20.0f * deltaTime;

		if (m_myPlayer.yPos < -STAGE_HEIGHT + PADDLE_HEIGHT)
			m_myPlayer.yPos = -STAGE_HEIGHT + PADDLE_HEIGHT;

		hasMoved = true;
	}

	if(hasMoved)
		SendClientGameObject();

	//Draw stage
	Gizmos::addAABB(
		glm::vec3(0),
		glm::vec3(WIDTH, STAGE_HEIGHT, 1),
		glm::vec4(1, 1, 1, 1));

	//Draw players
	int swap = 0;
	glm::vec4 colour1;
	glm::vec4 colour2;

	if (m_clientID == 1)
	{
		swap = 1;
		colour1 = glm::vec4(1, 1, 0, 1);
		colour2 = glm::vec4(0, 1, 1, 1);
	}
	else if (m_clientID == 2)
	{
		swap = -1;
		colour1 = glm::vec4(0, 1, 1, 1);
		colour2 = glm::vec4(1, 1, 0, 1);
	}

	Gizmos::addAABBFilled(
		glm::vec3(-45 * swap, m_myPlayer.yPos, 0),
		glm::vec3(1, PADDLE_HEIGHT, 1),
		colour1);
	Gizmos::addAABBFilled(
		glm::vec3(45 * swap, m_otherPlayer.yPos, 0),
		glm::vec3(1, PADDLE_HEIGHT, 1),
		colour2);

	//Update network
	HandleNetworkMessages();
}

void Client::draw()
{

	// wipe the screen to the background colour
	clearScreen();

	// update perspective in case window resized
	/*m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);*/

	m_projectionMatrix = glm::ortho(-WIDTH, WIDTH, -HEIGHT, HEIGHT, 0.0f, 1000.0f);

	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}

void Client::HandleNetworkConnection()
{
	//Initialise the raknet peer interface first
	m_pPeerInterface = RakNet::RakPeerInterface::GetInstance();
	InitialiseClientConnection();
}

void Client::InitialiseClientConnection()
{
	//Create a socket descripor to describe this connection. No data needed, since we're connecting to a server
	RakNet::SocketDescriptor sd;

	//Call startup - max of 1 connections (1 connection to the server)
	m_pPeerInterface->Startup(1, &sd, 1);

	std::cout << "Connecting to server at: " << IP << std::endl;

	//Now call connect to attempt to connect to the given server
	RakNet::ConnectionAttemptResult res = m_pPeerInterface->Connect(IP, PORT, nullptr, 0);

	//Check to see if connected, otherwise throw an error
	if (res != RakNet::CONNECTION_ATTEMPT_STARTED)
		std::cout << "Unable to start connection, Error number: " << res << std::endl;
}

void Client::HandleNetworkMessages()
{
	RakNet::Packet* packet = nullptr;

	for (packet = m_pPeerInterface->Receive(); packet; m_pPeerInterface->DeallocatePacket(packet), packet = m_pPeerInterface->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			std::cout << "Another client has disconnected." << std::endl;
			break;
		case ID_REMOTE_CONNECTION_LOST:
			std::cout << "Another client has lost the connection." << std::endl;
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			std::cout << "Another client has connected." << std::endl;
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			std::cout << "Our connection request has been accepted." << std::endl;
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			std::cout << "The server is full." << std::endl;
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			std::cout << "We have been disconnected." << std::endl;
			break;
		case ID_CONNECTION_LOST:
			std::cout << "Connection lost." << std::endl;
			break;
		case ID_SERVER_TEXT_MESSAGE:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			RakNet::RakString str;
			bsIn.Read(str);
			std::cout << str.C_String() << std::endl;

			break;
		}
		case ID_SERVER_SET_CLIENT_ID:
			OnSetClientIDPacket(packet);
			SendClientGameObject();
			break;
		case ID_CLIENT_PLAYER_DATA:
			OnReceivedClientDataPacket(packet);
			break;
		default:
			std::cout << "Received a message with an unknown ID: " << packet->data[0];
			break;
		}
	}
}

void Client::OnSetClientIDPacket(RakNet::Packet* packet)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	bsIn.Read(m_clientID);

	std::cout << "Set my client ID to: " << m_clientID << std::endl;
	
	if (m_clientID == 0)
		std::cout << "ID was zero - player not accepted" << std::endl;
}

void Client::OnReceivedClientDataPacket(RakNet::Packet* packet)
{
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

	int clientID;
	bsIn.Read(clientID);

	//Make sure packet was received from one of the two players
	if (clientID != 0)
	{
		//If the client ID does not match our ID, update our client GameObject information
		if (clientID != m_clientID)
		{
			bsIn.Read((char*)&m_otherPlayer, sizeof(Player));

			std::cout << "Client " << clientID <<
				" at: " << m_otherPlayer.yPos << std::endl;
		}
	}
}

void Client::SendClientGameObject()
{
	m_myPlayer.SendData(m_clientID, m_pPeerInterface);
}
