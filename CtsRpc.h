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
    static constexpr int CTS_NOTIFY_BATTLEGAME_PREPARED = 4;

    explicit CtsRpc() = default;
    ~CtsRpc() = default;
    void HandleMessage(const Context& context, const Message& message) const noexcept;

private:
    void OnRequestMatchMaking(const Context& context) const noexcept;
    void OnMoveCharacter(const Context& context, double x, double y, double z) const noexcept;
    void OnEnterNickname(const Context& context, std::string&& nickname) const noexcept;
    void OnNotifyBattleGamePrepared(const Context& context) const noexcept;
};


#endif //BATTLEGAME_SERVER_CTSRPC_H
