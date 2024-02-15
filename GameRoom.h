//
// Created by floweryclover on 2024-01-30.
//

#ifndef BATTLEGAME_SERVER_GAMEROOM_H
#define BATTLEGAME_SERVER_GAMEROOM_H

#include <set>
#include <map>
#include <memory>
#include <chrono>
#include "UnrealTypes.h"

#define INTERFACE class
#define IMPLEMENTS

using SerializedEndpoint = unsigned long long;
using ClientId = SerializedEndpoint;
using GameRoomId = unsigned int;
using EntityId = int;

static constexpr GameRoomId ROOM_MAINMENU = 0;

class BaseRoom
{
public:
    BaseRoom(const BaseRoom&) noexcept = delete;
    BaseRoom(BaseRoom&&) noexcept = delete;
    BaseRoom& operator=(const BaseRoom&) noexcept = delete;
    BaseRoom& operator=(BaseRoom&&) noexcept = delete;

    explicit BaseRoom(GameRoomId roomId) noexcept : mRoomId(roomId) {}
    inline GameRoomId GetId() const noexcept { return mRoomId; }

    virtual ~BaseRoom() = default;
    virtual void OnPlayerJoined(ClientId clientId) noexcept = 0;
    virtual void OnPlayerLeft(ClientId clientId) noexcept = 0;
    virtual void OnPlayerPrepared(ClientId clientId) noexcept = 0;

private:
    const GameRoomId mRoomId;
};

INTERFACE IPlayerCountable
{
public:
    virtual bool IsEmpty() const noexcept = 0;
    virtual bool IsFull() const noexcept = 0;
    virtual bool IsPlayerJoined(ClientId clientId) const noexcept = 0;
    virtual std::vector<ClientId> GetAllPlayers() const noexcept = 0;
};

INTERFACE IEntityMoveable
{
public:
    virtual void OnEntityMove(EntityId entityId, const Vector& location, double direction) noexcept = 0;
    virtual void OnPlayerMove(ClientId clientId, const Vector& location, double direction) noexcept = 0;
};

INTERFACE ITickable
{
public:
    virtual void Tick() noexcept = 0;
};

class MainMenuRoom : public BaseRoom
{
public:
    explicit MainMenuRoom(GameRoomId roomId) noexcept : BaseRoom(roomId) {}
    ~MainMenuRoom() override = default;

    void OnPlayerJoined(ClientId clientId) noexcept override;
    void OnPlayerLeft(ClientId clientId) noexcept override;
    void OnPlayerPrepared(ClientId clientId) noexcept override;
};

class OneVsOneGameRoom : public BaseRoom, IMPLEMENTS public ITickable, public IPlayerCountable, public IEntityMoveable
{
public:
    explicit OneVsOneGameRoom(GameRoomId roomId) noexcept;
    ~OneVsOneGameRoom() override = default;

    void OnPlayerJoined(ClientId clientId) noexcept override;
    void OnPlayerLeft(ClientId clientId) noexcept override;
    void OnPlayerPrepared(ClientId clientId) noexcept override;
    void Tick() noexcept override;
    bool IsEmpty() const noexcept override;
    bool IsFull() const noexcept override;
    bool IsPlayerJoined(ClientId clientId) const noexcept override;
    std::vector<ClientId> GetAllPlayers() const noexcept override;
    void OnEntityMove(EntityId entityId, const Vector& location, double direction) noexcept override;
    void OnPlayerMove(ClientId clientId, const Vector& location, double direction) noexcept override;

private:
    enum class GameState
    {
        PREPARE_GAME,
        PLAY_GAME,
        GAME_END,
        PENDING_DESTROY,
    };
    static constexpr int TIME_PREPARE_GAME = 10;
    static constexpr int TIME_PLAY_GAME = 10;
    static constexpr int TIME_GAME_END = 10;
    static constexpr EntityId ENTITY_ID_PLAYER_BLUE = 1;
    static constexpr EntityId ENTITY_ID_PLAYER_RED = 2;
    std::chrono::time_point<std::chrono::steady_clock> mTimePoint;
    std::chrono::seconds mTimeSet;
    GameState mGameState;

    std::optional<ClientId> mBluePlayer;
    std::optional<ClientId> mRedPlayer;
};

#endif //BATTLEGAME_SERVER_GAMEROOM_H
