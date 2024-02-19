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

const Vector OneVsOneGameRoom::CENTER_LOCATION = Vector {0, 0, 0};

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

                mBlueLocation = Vector {180,0,1000};
                mRedLocation = Vector {-180,0,1000};
                mBlueAngle = 180;
                mRedAngle = 0;
                auto spawnPlayerCharacter = [this](ClientId to, EntityId whose)
                {
                    BattleGameServer::GetConstInstance()
                    .GetConstStcRpc()
                    .SpawnEntity(
                            to,
                            whose,
                            whose == ENTITY_ID_PLAYER_BLUE ? mBlueLocation : mRedLocation,
                            whose == ENTITY_ID_PLAYER_BLUE ? mBlueAngle : mRedAngle);
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

                auto blueNickname = *BattleGameServer::GetConstInstance().GetConstGameData().GetPlayerNickname(mBluePlayer.value());
                auto redNickname = *BattleGameServer::GetConstInstance().GetConstGameData().GetPlayerNickname(mRedPlayer.value());

                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .AssignEntityNickname(mBluePlayer.value(), ENTITY_ID_PLAYER_BLUE, blueNickname);
                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .AssignEntityNickname(mBluePlayer.value(), ENTITY_ID_PLAYER_RED, redNickname);
                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .AssignEntityNickname(mRedPlayer.value(), ENTITY_ID_PLAYER_BLUE, blueNickname);
                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .AssignEntityNickname(mRedPlayer.value(), ENTITY_ID_PLAYER_RED, redNickname);

                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .SendGameData(mBluePlayer.value(), blueNickname, redNickname);
                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .SendGameData(mRedPlayer.value(), redNickname, blueNickname);

                mTimePoint = std::chrono::steady_clock::now();
                mTimeSet = std::chrono::seconds(TIME_GAME_ONE_ROUND);
                mGameState = STATE_PLAY_GAME;

                SetRemainTimeTo(mBluePlayer.value(), "라운드 종료까지");
                SetRemainTimeTo(mRedPlayer.value(), "라운드 종료까지");

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
            if (!mBluePlayer.has_value() || !mRedPlayer.has_value() // 누가 탈주하면 게임 바로 종료
            || mBlueScore >= 5 || mRedScore >= 5) // 5점 이상 획득한 플레이어가 있다면 종료
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

            // 게임 질질 끄는 것 방지.
            // 일정 시간이 지나면 중앙에서 더 가까운 사람이 1점 획득
            if (currentTime > mTimePoint + mTimeSet)
            {
                double blueDistance = mBlueLocation.DistanceTo(CENTER_LOCATION);
                double redDistance = mBlueLocation.DistanceTo(CENTER_LOCATION);

                bool isBlueWin = blueDistance < redDistance;

                IncrementScore(isBlueWin ? TEAM_ID_BLUE : TEAM_ID_RED);
                RespawnPlayer(TEAM_ID_BLUE);
                RespawnPlayer(TEAM_ID_RED);

                mTimePoint = std::chrono::steady_clock::now();
                mTimeSet = std::chrono::seconds(TIME_GAME_ONE_ROUND);

                SetRemainTimeTo(mBluePlayer.value(), "라운드 종료까지");
                SetRemainTimeTo(mRedPlayer.value(), "라운드 종료까지");
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

    if (clientId == mBluePlayer.value())
    {
        mBlueLocation = location;
        mBlueAngle = direction;
    }
    else
    {
        mRedLocation = location;
        mRedAngle = direction;
    }

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .MoveEntity(
            clientId == mBluePlayer.value() ? mRedPlayer.value() : mBluePlayer.value(),
            clientId == mBluePlayer.value() ? ENTITY_ID_PLAYER_BLUE : ENTITY_ID_PLAYER_RED,
            location,
            direction
            );
}

void OneVsOneGameRoom::SetRemainTimeTo(ClientId clientId, const std::string& text) const noexcept
{
    auto remainTime = std::chrono::duration_cast<std::chrono::seconds>
            (mTimePoint+mTimeSet-std::chrono::steady_clock::now())
            .count();
    BattleGameServer::GetConstInstance()
            .GetConstStcRpc()
            .SetTimer(clientId, remainTime, std::string(text));
}

void OneVsOneGameRoom::OnOwningCharacterDestroyed(ClientId clientId) noexcept
{
    if (!IsPlayerJoined(clientId) || !IsFull() || mGameState != STATE_PLAY_GAME)
    {return;}

    EntityId destroyed = (clientId == mBluePlayer.value() ? ENTITY_ID_PLAYER_BLUE : ENTITY_ID_PLAYER_RED);

    IncrementScore(clientId == mBluePlayer.value() ? TEAM_ID_RED : TEAM_ID_BLUE);
    RespawnPlayer(clientId == mBluePlayer.value() ? TEAM_ID_BLUE : TEAM_ID_RED);

    mTimePoint = std::chrono::steady_clock::now();
    mTimeSet = std::chrono::seconds(TIME_GAME_ONE_ROUND);

    SetRemainTimeTo(mBluePlayer.value(), "라운드 종료까지");
    SetRemainTimeTo(mRedPlayer.value(), "라운드 종료까지");
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

void OneVsOneGameRoom::RespawnPlayer(int teamId) noexcept
{
    auto respawnTo = [teamId, this](ClientId to)
    {
        BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .RespawnEntity(to,
                               teamId == TEAM_ID_BLUE ? ENTITY_ID_PLAYER_BLUE : ENTITY_ID_PLAYER_RED,
                               teamId == TEAM_ID_BLUE ? (mBlueLocation = Vector {100, 0, 1000}) : (mRedLocation = Vector {-100, 0, 1000}),
                               teamId == TEAM_ID_BLUE ? (mBlueAngle = 180) : (mRedAngle = 0));
    };
    respawnTo(mBluePlayer.value());
    respawnTo(mRedPlayer.value());
}