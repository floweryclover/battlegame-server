//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_STCRPC_H
#define BATTLEGAME_SERVER_STCRPC_H

#include <functional>

class Message;

using ConnectionId = unsigned int;

class StcRpc {
public:
    static constexpr int STC_JOINED_GAME_ROOM = 1;
    static constexpr int STC_DISCONNECTED_FROM_GAME = 2;

    StcRpc(const StcRpc& rhs) = delete;
    StcRpc(StcRpc&& rhs) = delete;
    StcRpc& operator=(const StcRpc& rhs) = delete;

    explicit StcRpc(std::function<void(unsigned int, Message&&)> sendEnqueue) noexcept;
    ~StcRpc() = default;

    void JoinedGameRoom(ConnectionId to) const noexcept;
    void DisconnectedFromGame(ConnectionId to) const noexcept;
private:
    std::function<void(unsigned int, Message&&)> mSendEnqueue;
};


#endif //BATTLEGAME_SERVER_STCRPC_H
