//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEROOM_H
#define BATTLEGAME_SERVER_GAMEROOM_H

#include <set>
#include <map>
#include "UnrealTypes.h"

using SerializedEndpoint = unsigned long long;
using ClientId = SerializedEndpoint;
using GameRoomId = unsigned int;
using EntityId = int;

class GameRoom
{
public:
    static constexpr GameRoomId ROOM_MAINMENU = 0;
    static constexpr GameRoomId ROOM_MATCHMAKING = 1;

    GameRoom(const GameRoom& rhs) = delete;
    GameRoom& operator=(const GameRoom& rhs) = delete;
    explicit GameRoom(unsigned int roomId) noexcept;
    GameRoom(GameRoom&& rhs) noexcept;
    ~GameRoom() = default;

    inline bool IsEmpty() const { return mJoinedPlayers.size() == 0; }
    inline bool IsFull() const { return mJoinedPlayers.size() == mMaxPlayerCount; }
    inline bool IsPlayerJoined(ClientId clientId) const { return mJoinedPlayers.find(clientId) != mJoinedPlayers.end(); }
    inline unsigned short GetMaxPlayerCount() const { return mMaxPlayerCount; }
    inline unsigned short GetCurrentPlayerCount() const { return mJoinedPlayers.size(); }

    void Tick() noexcept;
    void InvokeOnPlayerJoined(ClientId clientId) noexcept;
    void InvokeOnPlayerLeft(ClientId clientId) noexcept;
    void InvokeOnPlayerPrepared(ClientId clientId) noexcept;
    void InvokeOnPlayerMove(ClientId clientId, const Vector& location, double direction) noexcept;
    void InvokeOnEntityMove(EntityId entityId, const Vector& location, double direction) noexcept;

private:
    GameRoomId mRoomId;
    unsigned short mMaxPlayerCount;
    std::set<ClientId> mJoinedPlayers;
    std::set<ClientId> mOnlinePlayers; // 접속되고 Prepared된 클라이언트들
    std::set<EntityId> mEntities;
    std::map<EntityId, Vector> mEntityLocations;
    std::map<EntityId, double> mEntityDirections;
    std::map<ClientId, EntityId> mPlayerControllingPawnIds;
    int mNewEntityId;
};


#endif //BATTLEGAME_SERVER_GAMEROOM_H
