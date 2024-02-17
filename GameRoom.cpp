#include "GameRoom.h"
#include "GameData.h"
#include "GameRoomManager.h"
#include "BattleGameServer.h"
#include "StcRpc.h"
#include <iostream>

void MainMenuRoom::OnPlayerJoined(ClientId clientId) noexcept
{ BattleGameServer::GetInstance().GetConstStcRpc().OpenLevel(clientId, Level::MAINMENU); }

void MainMenuRoom::OnPlayerLeft(ClientId clientId) noexcept {}

void MainMenuRoom::OnPlayerPrepared(ClientId clientId) noexcept {}

// OneVsOneGameRoom 1대1 게임방

OneVsOneGameRoom::OneVsOneGameRoom(GameRoomId roomId) noexcept
: BaseRoom(roomId),
mTimePoint(std::chrono::steady_clock::now()),
mTimeSet(TIME_PREPARE_GAME),
mGameState(STATE_PREPARE_GAME),
mBlueScore(0),
mRedScore(0)
{}

void OneVsOneGameRoom::OnPlayerJoined(ClientId clientId) noexcept
{ BattleGameServer::GetInstance().GetConstStcRpc().OpenLevel(clientId, Level::ONE_VS_ONE); }

void OneVsOneGameRoom::OnPlayerLeft(ClientId clientId) noexcept
{
    if (mBluePlayer.has_value() && mBluePlayer.value() == clientId)
    {mBluePlayer = std::nullopt;}
    else if (mRedPlayer.has_value() && mRedPlayer.value() == clientId)
    {mRedPlayer = std::nullopt;}
}

void OneVsOneGameRoom::OnPlayerPrepared(ClientId clientId) noexcept
{
    if (mBluePlayer.has_value() && mRedPlayer.has_value())
    {
        std::cerr << GetId() << " 번 방에 자리가 없는데도 새 플레이어가 입장했습니다." << std::endl;
        BattleGameServer::GetInstance()
        .GetGameData()
        .GetGameRoomManager()
        .DestroyRoom(GetId());
        mGameState = STATE_PENDING_DESTROY;
        return;
    }
    if (!mBluePlayer.has_value())
    {
        mBluePlayer = clientId;
        BattleGameServer::GetConstInstance()
        .GetConstStcRpc()
        .AssignTeamId(clientId, TEAM_ID_BLUE);
    }
    else
    {
        mRedPlayer = clientId;
        BattleGameServer::GetConstInstance()
        .GetConstStcRpc()
        .AssignTeamId(clientId, TEAM_ID_RED);
    }

    SetRemainTimeTo(clientId, "게임 시작까지");

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .SignalGameState(clientId, STATE_PREPARE_GAME);

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .SetScore(clientId, 0, WIN_SCORE);

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .SetScore(clientId, TEAM_ID_BLUE, 0);

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .SetScore(clientId, TEAM_ID_RED, 0);
}

void OneVsOneGameRoom::Tick() noexcept
{
    auto currentTime = std::chrono::steady_clock::now();
    switch (mGameState)
    {
        case STATE_PREPARE_GAME:
        {
            if (currentTime > mTimePoint + mTimeSet)
            {
                // 플레이어들이 접속해있는지 확인하고, 아닐 경우 방 폭파
                if (!mBluePlayer.has_value() || !mRedPlayer.has_value())
                {
                    mGameState = STATE_PENDING_DESTROY;
                    BattleGameServer::GetInstance()
                    .GetGameData()
                    .GetGameRoomManager()
                    .DestroyRoom(GetId());
                    return;
                }

                auto spawnPlayerCharacter = [](ClientId to, EntityId whose)
                {
                    BattleGameServer::GetConstInstance()
                    .GetConstStcRpc()
                    .SpawnEntity(
                            to,
                            whose,
                            whose == ENTITY_ID_PLAYER_BLUE ? Vector { 100, 0, 2000 } : Vector { -100, 0, 2000 },
                            whose == ENTITY_ID_PLAYER_BLUE ? 180.0 : 0.0);
                };

                spawnPlayerCharacter(mBluePlayer.value(), ENTITY_ID_PLAYER_BLUE);
                spawnPlayerCharacter(mBluePlayer.value(), ENTITY_ID_PLAYER_RED);
                spawnPlayerCharacter(mRedPlayer.value(), ENTITY_ID_PLAYER_BLUE);
                spawnPlayerCharacter(mRedPlayer.value(), ENTITY_ID_PLAYER_RED);

                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .PossessEntity(mBluePlayer.value(), ENTITY_ID_PLAYER_BLUE);

                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .PossessEntity(mRedPlayer.value(), ENTITY_ID_PLAYER_RED);

                mTimePoint = std::chrono::steady_clock::now();
                mTimeSet = std::chrono::seconds(TIME_PLAY_GAME);
                mGameState = STATE_PLAY_GAME;

                SetRemainTimeTo(mBluePlayer.value(), "게임 종료까지");
                SetRemainTimeTo(mRedPlayer.value(), "게임 종료까지");

                auto signalGameStart = [](ClientId to)
                {
                    BattleGameServer::GetConstInstance()
                    .GetConstStcRpc()
                    .SignalGameState(to, STATE_PLAY_GAME);
                };
                signalGameStart(mBluePlayer.value());
                signalGameStart(mRedPlayer.value());
            }
            return;
        }
        case STATE_PLAY_GAME:
        {
            // 누가 탈주하면 게임 종료
            // 시간 지나서 게임 종료
            if (!mBluePlayer.has_value() || !mRedPlayer.has_value()
            || currentTime > mTimePoint + mTimeSet)
            {
                mTimePoint = std::chrono::steady_clock::now();
                mTimeSet = std::chrono::seconds(TIME_GAME_END);
                mGameState = STATE_GAME_END;

                SetRemainTimeTo(mBluePlayer.value(), "메인 화면으로 복귀");
                SetRemainTimeTo(mRedPlayer.value(), "메인 화면으로 복귀");

                auto signalGameEnd = [](ClientId to)
                {
                    BattleGameServer::GetConstInstance()
                            .GetConstStcRpc()
                            .SignalGameState(to, STATE_GAME_END);
                };
                signalGameEnd(mBluePlayer.value());
                signalGameEnd(mRedPlayer.value());
            }
            return;
        }
        case STATE_GAME_END:
        {
            // 시간 지나서 방 폭파
            if (currentTime > mTimePoint + mTimeSet)
            {
                mGameState = STATE_PENDING_DESTROY;
                BattleGameServer::GetInstance()
                .GetGameData()
                .GetGameRoomManager()
                .DestroyRoom(GetId());
            }
            return;
        }
        case STATE_PENDING_DESTROY:
        {return;}
    }
}

bool OneVsOneGameRoom::IsEmpty() const noexcept
{return !mBluePlayer.has_value() && !mRedPlayer.has_value();}

bool OneVsOneGameRoom::IsFull() const noexcept
{return mBluePlayer.has_value() && mRedPlayer.has_value();}

bool OneVsOneGameRoom::IsPlayerJoined(ClientId clientId) const noexcept
{
    return
    ((mBluePlayer.has_value() && mBluePlayer.value() == clientId)
    || (mRedPlayer.has_value() && mRedPlayer.value() == clientId));
}

std::vector<ClientId> OneVsOneGameRoom::GetAllPlayers() const noexcept
{
    if (!mRedPlayer.has_value() && !mBluePlayer.has_value())
    {return {};}
    else if (mRedPlayer.has_value() && mBluePlayer.has_value())
    {return {mBluePlayer.value(), mRedPlayer.value()};}
    else
    {return {(mBluePlayer.has_value() ? mBluePlayer.value() : mRedPlayer.value())};}
}

void OneVsOneGameRoom::OnEntityMove(EntityId entityId, const Vector& location, double direction) noexcept
{}

void OneVsOneGameRoom::OnPlayerMove(ClientId clientId, const Vector& location, double direction) noexcept
{
    if (!mBluePlayer.has_value() || !mRedPlayer.has_value())
    {return;}

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .MoveEntity(
            clientId == mBluePlayer.value() ? mRedPlayer.value() : mBluePlayer.value(),
            clientId == mBluePlayer.value() ? ENTITY_ID_PLAYER_BLUE : ENTITY_ID_PLAYER_RED,
            location,
            direction
            );
}

void OneVsOneGameRoom::SetRemainTimeTo(ClientId clientId, const char* text) const noexcept
{
    auto remainTime = std::chrono::duration_cast<std::chrono::seconds>
            (mTimePoint+mTimeSet-std::chrono::steady_clock::now())
            .count();
    BattleGameServer::GetConstInstance()
            .GetConstStcRpc()
            .SetTimer(clientId, remainTime, text);
}

void OneVsOneGameRoom::OnOwningCharacterDestroyed(ClientId clientId) noexcept
{
    if (!IsPlayerJoined(clientId) || !IsFull() || mGameState != STATE_PLAY_GAME)
    {return;}

    EntityId destroyed = (clientId == mBluePlayer.value() ? ENTITY_ID_PLAYER_BLUE : ENTITY_ID_PLAYER_RED);

    IncrementScore(clientId == mBluePlayer.value() ? TEAM_ID_RED : TEAM_ID_BLUE);

    auto respawnTo = [destroyed](ClientId to)
    {
        BattleGameServer::GetConstInstance()
        .GetConstStcRpc()
        .RespawnEntity(to,
                       destroyed,
                       destroyed == ENTITY_ID_PLAYER_BLUE ? Vector { 100, 0, 2000 } : Vector {-100, 0, 2000},
                       destroyed == ENTITY_ID_PLAYER_BLUE ? 180.0 : 0.0);
    };
    respawnTo(mBluePlayer.value());
    respawnTo(mRedPlayer.value());
}

void OneVsOneGameRoom::IncrementScore(int teamId) noexcept
{
    if (!mBluePlayer.has_value() || !mRedPlayer.has_value() || mGameState != STATE_PLAY_GAME)
    {return;}

    int scoreToSend;
    if (teamId == TEAM_ID_BLUE)
    {scoreToSend = ++mBlueScore;}
    else
    {scoreToSend = ++mRedScore;}

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .SetScore(mBluePlayer.value(), teamId, scoreToSend);

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .SetScore(mRedPlayer.value(), teamId, scoreToSend);
}