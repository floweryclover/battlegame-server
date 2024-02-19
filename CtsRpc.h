//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_CTSRPC_H
#define BATTLEGAME_SERVER_CTSRPC_H

#include <string>
#include "Context.h"
#include "Client.h"
#include "Message.h"
#include "UnrealTypes.h"

class StcRpc;

class CtsRpc {
public:
    static constexpr int CTS_REQUEST_MATCHMAKING = 1;
    static constexpr int CTS_MOVE_CHARACTER = 2;
    static constexpr int CTS_NOTIFY_BATTLEGAME_PREPARED = 3;
    static constexpr int CTS_NOTIFY_OWNING_CHARACTER_DESTROYED = 4;
    static constexpr int CTS_SET_NICKNAME = 5;
    static constexpr int CTS_REQUEST_MY_NICKNAME = 6;

    explicit CtsRpc() = default;
    ~CtsRpc() = default;
    void HandleMessage(const Context& context, const Message& message) const noexcept;

private:
    void OnRequestMatchMaking(const Context& context) const noexcept;
    void OnMoveCharacter(const Context& context, const Vector& position, double direction) const noexcept;
    void OnNotifyBattleGamePrepared(const Context& context) const noexcept;
    void OnNotifyOwningCharacterDestroyed(const Context& context) const noexcept;
    void OnSetNickname(const Context& context, const std::string& nickname) const noexcept;
    void OnRequestMyNickname(const Context& context) const noexcept;
};


#endif //BATTLEGAME_SERVER_CTSRPC_H
