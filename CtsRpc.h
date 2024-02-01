//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_CTSRPC_H
#define BATTLEGAME_SERVER_CTSRPC_H

#include <string>
#include "Context.h"
#include "Client.h"
#include "Message.h"

class StcRpc;

class CtsRpc {
public:
    explicit CtsRpc() = default;
    ~CtsRpc() = default;
    void HandleMessage(const Context& context, const Message& message) const;

    static constexpr int CTS_REQUEST_MATCHMAKING = 1;
    static constexpr int CTS_ENTER_NICKNAME = 2;
private:
    void OnRequestMatchMaking(const Context& context) const;
    void OnEnterNickname(const Context& context, std::string&& nickname) const;
};


#endif //BATTLEGAME_SERVER_CTSRPC_H
