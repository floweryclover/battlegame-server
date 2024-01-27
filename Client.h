//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_CLIENT_H
#define BATTLEGAME_SERVER_CLIENT_H

#include "Socket.h"
#include "Constants.h"

class Client {
public:
    Client(const Client& rhs) = delete;
    Client& operator=(const Client& rhs) = delete;
    Client(int connectionId, Socket&& socket) noexcept;
    Client(Client&& rhs) noexcept;
    ~Client();

    inline int GetConnectionId() const { return this->connectionId_; }
    inline SOCKET GetSocketHandle() { return this->socket_.operator SOCKET(); }

    int currentReceived;
    int totalSizeToReceive;
    int receivingHeaderType;
    char receiveBuffer[MAX_MESSAGE_SIZE];
private:
    int connectionId_;
    Socket socket_;
};


#endif //BATTLEGAME_SERVER_CLIENT_H
