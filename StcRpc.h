//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_STCRPC_H
#define BATTLEGAME_SERVER_STCRPC_H

#include "UnrealTypes.h"
#include <string>

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
    static constexpr int STC_SET_TIMER = 8;
    static constexpr int STC_SIGNAL_GAME_STATE = 9;
    static constexpr int STC_RESPAWN_ENTITY = 10;
    static constexpr int STC_SET_SCORE = 11;
    static constexpr int STC_ASSIGN_TEAM_ID = 12;
    static constexpr int STC_ASSIGN_ENTITY_NICKNAME = 13;
    static constexpr int STC_GET_MY_NICKNAME = 14;
    static constexpr int STC_SEND_GAME_DATA = 15;
    static constexpr int STC_SEND_GAME_RESULT = 16;

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
    void SetTimer(ClientId to, unsigned short seconds, const std::string& text) const noexcept;
    void SignalGameState(ClientId to, int signal) const noexcept;
    void RespawnEntity(ClientId to, int entityId, const Vector& location, double direction) const noexcept;
    void SetScore(ClientId to, int team, int score) const noexcept;
    void AssignTeamId(ClientId to, int teamId) const noexcept;
    void AssignEntityNickname(ClientId to, int entityId, const std::string& nickname) const noexcept;
    void GetMyNickname(ClientId to, const std::string& nickname) const noexcept;
    void SendGameData(ClientId to, const std::string& yourNickname, const std::string& opponentNickname) const noexcept;
    void SendGameResult(ClientId to, bool isGoodGame, bool isWinner, int myScore, int opponentScore) const noexcept;
};


#endif //BATTLEGAME_SERVER_STCRPC_H
