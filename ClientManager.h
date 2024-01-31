//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_CLIENTMANAGER_H
#define BATTLEGAME_SERVER_CLIENTMANAGER_H

#include <map>
#include <queue>
#include <memory>
#include "Client.h"
#include "Message.h"
#include "CtsRpc.h"

using ConnectionId = unsigned int;

class ClientManager {
public:
    ClientManager(ClientManager&&) = delete;
    ClientManager(const ClientManager&) = delete;
    ClientManager& operator=(const ClientManager&) = delete;

    explicit ClientManager(const char* listenIpAddress, unsigned short listenPort);
    ~ClientManager() = default;

    inline ConnectionId IsClientExists(ConnectionId id) const { return mClients.find(id) != mClients.end(); }

    void ProcessNetworkIoOnce();

    inline std::queue<std::pair<ConnectionId, Message>>& GetSendQueue() { return mSendQueue; }
private:
    CtsRpc mCtsRpc;
    std::unique_ptr<Socket> mpListenSocket;
    std::queue<std::pair<ConnectionId, Message>> mSendQueue;
    std::map<ConnectionId, Client> mClients;
    unsigned int mCurrentSent;
    unsigned int mNewClientNumber;
};


#endif //BATTLEGAME_SERVER_CLIENTMANAGER_H
