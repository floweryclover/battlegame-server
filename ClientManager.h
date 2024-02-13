//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_CLIENTMANAGER_H
#define BATTLEGAME_SERVER_CLIENTMANAGER_H

#include <map>
#include <queue>
#include <memory>
#include <array>
#include "Client.h"
#include "Message.h"
#include "SocketAddress.h"

enum class MessageReliability
{
    RELIABLE,
    UNRELIABLE
};

class ClientManager {
public:
    ClientManager(ClientManager&&) = delete;
    ClientManager(const ClientManager&) = delete;
    ClientManager& operator=(const ClientManager&) = delete;

    explicit ClientManager(const char* listenIpAddress, unsigned short listenPort);
    ~ClientManager() = default;

    inline bool IsClientExists(ClientId clientId) const { return mClients.find(clientId) != mClients.end(); }
    void Tick();
    void InvokeOnPlayerDisconnected(ClientId clientId);
    bool KickPlayer(ClientId clientId);
    void RequestSendMessage(MessageReliability reliability, ClientId targetClientId, Message&& message);
private:
    void InvokeOnPlayerConnected(ClientId clientId);
    std::unique_ptr<Socket> mpListenSocket;
    std::queue<std::pair<ClientId, Message>> mSendQueue;
    std::map<ClientId, Client> mClients;
    unsigned int mCurrentSent;
    std::unique_ptr<Socket> mpUdpSocket;
    std::array<char, MAX_MESSAGE_SIZE> mUdpReceiveBuffer;
    std::array<char, MAX_MESSAGE_SIZE> mUdpSendBuffer;
};


#endif //BATTLEGAME_SERVER_CLIENTMANAGER_H
