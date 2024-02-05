//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_STCRPC_H
#define BATTLEGAME_SERVER_STCRPC_H

class Message;

using SerializedEndpoint = unsigned long long;
using ClientId = SerializedEndpoint;

class StcRpc {
public:
    static constexpr int STC_JOINED_GAME_ROOM = 1;
    static constexpr int STC_DISCONNECTED_FROM_GAME = 2;
    static constexpr int STC_ASSIGN_UDP_TOKEN = 3;

    StcRpc(const StcRpc& rhs) = delete;
    StcRpc(StcRpc&& rhs) = delete;
    StcRpc& operator=(const StcRpc& rhs) = delete;

    StcRpc() = default;
    ~StcRpc() = default;

    void JoinedGameRoom(ClientId to) const noexcept;
    void DisconnectedFromGame(ClientId to) const noexcept;
    void AssignUdpToken(ClientId to) const noexcept;
};


#endif //BATTLEGAME_SERVER_STCRPC_H
