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
    static constexpr int CTS_REQUEST_MATCHMAKING = 1;
    static constexpr int CTS_MOVE_CHARACTER = 2;
    static constexpr int CTS_ACK_UDP_TOKEN = 3; // ClientManager가 해당 번호의 Unreliable 메시지를 받을 경우 자동 처리

    explicit CtsRpc() = default;
    ~CtsRpc() = default;
    void HandleMessage(const Context& context, const Message& message) const;

private:
    void OnRequestMatchMaking(const Context& context) const;
    void OnMoveCharacter(const Context& context, double x, double y, double z) const;
    void OnEnterNickname(const Context& context, std::string&& nickname) const;
};


#endif //BATTLEGAME_SERVER_CTSRPC_H
