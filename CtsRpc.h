//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_CTSRPC_H
#define BATTLEGAME_SERVER_CTSRPC_H

#include <string>
#include "Client.h"
#include "Message.h"

class CtsRpc {
public:
    void HandleMessage(const Client& whose) const;

    static constexpr int LOGIN = 1;
private:
    void OnLogin(const Client& from, std::string&& nickname) const;
};


#endif //BATTLEGAME_SERVER_CTSRPC_H
