#include <BitStream.h>

#include "GameMessages.h"
#include "Player.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::SendData(int clientID, RakNet::RakPeerInterface* pPeerInterface)
{
	//Create new bitstream with client data ID
	RakNet::BitStream bs;
	bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_PLAYER_DATA);

	//Write client ID and data to stream
	bs.Write(clientID);
	bs.Write((char*)this, sizeof(Player));

	//Broadcast packet (should be picked up by the server)
	pPeerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}