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

INTERFACE IHasEntity
{
public:
    virtual void OnOwningCharacterDestroyed(ClientId clientId) noexcept = 0;
    virtual void OnEntityMove(EntityId entityId, const Vector3D& location, double direction) noexcept = 0;
    virtual void OnPlayerMove(ClientId clientId, const Vector3D& location, double direction) noexcept = 0;
};

INTERFACE ITickable
{
public:
    virtual void Tick() noexcept = 0;
};

INTERFACE ITakesCommand
{
public:
    virtual void OnPlayerEnterCommand(ClientId clientId, int commandId) noexcept = 0;
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

class OneVsOneGameRoom
        :public BaseRoom,
        IMPLEMENTS
        public ITickable,
        public IPlayerCountable,
        public IHasEntity,
        public ITakesCommand
{
public:
    explicit OneVsOneGameRoom(GameRoomId roomId, ClientId blueClientId, ClientId redClientId) noexcept;
    ~OneVsOneGameRoom() override = default;

    void OnPlayerJoined(ClientId clientId) noexcept override;
    void OnPlayerLeft(ClientId clientId) noexcept override;
    void OnPlayerPrepared(ClientId clientId) noexcept override;
    void Tick() noexcept override;
    bool IsEmpty() const noexcept override;
    bool IsFull() const noexcept override;
    bool IsPlayerJoined(ClientId clientId) const noexcept override;
    std::vector<ClientId> GetAllPlayers() const noexcept override;
    void OnEntityMove(EntityId entityId, const Vector3D& location, double direction) noexcept override;
    void OnPlayerMove(ClientId clientId, const Vector3D& location, double direction) noexcept override;
    void OnOwningCharacterDestroyed(ClientId clientId) noexcept override;
    void OnPlayerEnterCommand(ClientId clientId, int commandId) noexcept override;

private:
    static constexpr int STATE_PREPARE_GAME = 1;
    static constexpr int STATE_PLAY_GAME = 2;
    static constexpr int STATE_GAME_END = 3;
    static constexpr int STATE_PENDING_DESTROY = 4;

    static constexpr int TIME_PREPARE_GAME = 10;
    static constexpr int TIME_GAME_ONE_ROUND = 20;
    static constexpr int TIME_GAME_END = 10;

    static constexpr EntityId ENTITY_ID_PLAYER_BLUE = 1;
    static constexpr EntityId ENTITY_ID_PLAYER_RED = 2;

    static constexpr int TEAM_ID_BLUE = 1;
    static constexpr int TEAM_ID_RED = 2;

    static constexpr int WIN_SCORE = 5;
    static const Vector3D CENTER_LOCATION;
    int mBlueScore;
    int mRedScore;

    void SetRemainTimeTo(ClientId clientId, const std::string& text) const noexcept;
    void IncrementScore(int teamId) noexcept;
    void RespawnPlayer(int teamId) noexcept;
    void HandleCommand(bool isBlue, int commandId);

    std::chrono::time_point<std::chrono::steady_clock> mTimePoint;
    std::chrono::seconds mTimeSet;
    int mGameState;

    const ClientId mBluePlayer;
    const ClientId mRedPlayer;
    bool mIsBlueValid;
    bool mIsRedValid;

    Vector3D mBlueLocation;
    Vector3D mRedLocation;
    double mBlueAngle;
    double mRedAngle;

    std::chrono::time_point<std::chrono::steady_clock> mBlueLastCommandTime;
    std::chrono::time_point<std::chrono::steady_clock> mRedLastCommandTime;
    std::chrono::milliseconds mBlueLastDelay;
    std::chrono::milliseconds mRedLastDelay;
    int mBlueLastCommand;
    int mRedLastCommand;
};

#endif //BATTLEGAME_SERVER_GAMEROOM_H
