#pragma once

#include "Application.h"
#include <glm/glm.hpp>
#include <RakPeerInterface.h>
#include <map>
#include <MessageIdentifiers.h>
#include <BitStream.h>

enum MESSAGE_ID
{
	ID_USER_NEW_CLIENT = ID_USER_PACKET_ENUM,
	ID_USER_CLIENT_DISCONNECTED,
	ID_USER_ID,
	ID_USER_CUSTOM_DATA,
	ID_USER_POSITION = ID_USER_CUSTOM_DATA
};
const int SERVER_PORT = 12001;
// derived application class that wraps up all globals neatly
class NetworkingGame : public Application
{
public:

	NetworkingGame();
	virtual ~NetworkingGame();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	RakNet::RakPeerInterface * m_raknet;
	RakNet::SystemAddress m_serverAddress;
	int m_myID;
	
	std::map<int, glm::vec3> m_players;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};