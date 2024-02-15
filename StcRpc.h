//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_STCRPC_H
#define BATTLEGAME_SERVER_STCRPC_H

#include "UnrealTypes.h"

class Message;

using SerializedEndpoint = unsigned long long;
using ClientId = SerializedEndpoint;

namespace Level
{
    static constexpr int MAINMENU = 0;
    static constexpr int ONE_VS_ONE = 1;
}

class StcRpc {
public:
    static constexpr int STC_OPEN_LEVEL = 1;
    static constexpr int STC_DISCONNECTED_FROM_GAME = 2; // 이유 등 통지만 하고 레벨 열지는 않음
    static constexpr int STC_SPAWN_ENTITY = 4;
    static constexpr int STC_DESPAWN_ENTITY = 5;
    static constexpr int STC_POSSESS_ENTITY = 6;
    static constexpr int STC_MOVE_ENTITY = 7;

    StcRpc(const StcRpc& rhs) = delete;
    StcRpc(StcRpc&& rhs) = delete;
    StcRpc& operator=(const StcRpc& rhs) = delete;

    StcRpc() = default;
    ~StcRpc() = default;

    void OpenLevel(ClientId to, int level) const noexcept;
    void DisconnectedFromGame(ClientId to) const noexcept;
    void SpawnEntity(ClientId to, int entityId, const Vector& location, double direction) const noexcept;
    void DespawnEntity(ClientId to, int entityId) const noexcept;
    void PossessEntity(ClientId to, int entityId) const noexcept;
    void MoveEntity(ClientId to, int entityId, const Vector& location, double direction) const noexcept;
};


#endif //BATTLEGAME_SERVER_STCRPC_H
