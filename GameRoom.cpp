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

OneVsOneGameRoom::OneVsOneGameRoom(GameRoomId roomId, ClientId blueClientId, ClientId redClientId) noexcept
: BaseRoom(roomId),
mTimePoint(std::chrono::steady_clock::now()),
mTimeSet(TIME_PREPARE_GAME),
mGameState(STATE_PREPARE_GAME),
mBlueScore(0),
mRedScore(0),
mBluePlayer(blueClientId),
mRedPlayer(redClientId),
mIsBlueValid(false),
mIsRedValid(false)
{}

void OneVsOneGameRoom::OnPlayerJoined(ClientId clientId) noexcept
{ BattleGameServer::GetInstance().GetConstStcRpc().OpenLevel(clientId, Level::ONE_VS_ONE); }

void OneVsOneGameRoom::OnPlayerLeft(ClientId clientId) noexcept
{
    if (mIsBlueValid && clientId == mBluePlayer)
    {mIsBlueValid = false;}
    else if (mIsRedValid && clientId == mRedPlayer)
    {mIsRedValid = false;}
}

void OneVsOneGameRoom::OnPlayerPrepared(ClientId clientId) noexcept
{
    if (clientId != mBluePlayer && clientId != mRedPlayer)
    {
        std::cerr << GetId() << " 번 방에 실제 참여한 클라이언트 이외의 클라이언트가 게임 준비 메시지를 보냈습니다." << std::endl;
        BattleGameServer::GetInstance()
        .GetGameData()
        .GetGameRoomManager()
        .DestroyRoom(GetId());
        mGameState = STATE_PENDING_DESTROY;
        return;
    }

    auto pBlueNickname = BattleGameServer::GetConstInstance().GetConstGameData().GetPlayerNickname(mBluePlayer);
    auto pRedNickname = BattleGameServer::GetConstInstance().GetConstGameData().GetPlayerNickname(mRedPlayer);
    auto blueNickname = pBlueNickname != nullptr ? *pBlueNickname : std::string("알 수 없음");
    auto redNickname = pRedNickname != nullptr ? *pRedNickname : std::string("알 수 없음");
    if (clientId == mBluePlayer)
    {
        mIsBlueValid = true;
        BattleGameServer::GetConstInstance()
        .GetConstStcRpc()
        .AssignTeamId(clientId, TEAM_ID_BLUE);

        BattleGameServer::GetConstInstance()
        .GetConstStcRpc()
        .SendGameData(clientId, blueNickname, redNickname);
    }
    else
    {
        mIsRedValid = true;
        BattleGameServer::GetConstInstance()
        .GetConstStcRpc()
        .AssignTeamId(clientId, TEAM_ID_RED);

        BattleGameServer::GetConstInstance()
        .GetConstStcRpc()
        .SendGameData(clientId, redNickname, blueNickname);
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
                if (!mIsBlueValid || !mIsRedValid)
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

                spawnPlayerCharacter(mBluePlayer, ENTITY_ID_PLAYER_BLUE);
                spawnPlayerCharacter(mBluePlayer, ENTITY_ID_PLAYER_RED);
                spawnPlayerCharacter(mRedPlayer, ENTITY_ID_PLAYER_BLUE);
                spawnPlayerCharacter(mRedPlayer, ENTITY_ID_PLAYER_RED);

                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .PossessEntity(mBluePlayer, ENTITY_ID_PLAYER_BLUE);

                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .PossessEntity(mRedPlayer, ENTITY_ID_PLAYER_RED);

                auto pBlueNickname = BattleGameServer::GetConstInstance().GetConstGameData().GetPlayerNickname(mBluePlayer);
                auto pRedNickname = BattleGameServer::GetConstInstance().GetConstGameData().GetPlayerNickname(mRedPlayer);
                auto blueNickname = pBlueNickname != nullptr ? *pBlueNickname : std::string("알 수 없음");
                auto redNickname = pRedNickname != nullptr ? *pRedNickname : std::string("알 수 없음");

                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .AssignEntityNickname(mBluePlayer, ENTITY_ID_PLAYER_BLUE, blueNickname);
                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .AssignEntityNickname(mBluePlayer, ENTITY_ID_PLAYER_RED, redNickname);
                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .AssignEntityNickname(mRedPlayer, ENTITY_ID_PLAYER_BLUE, blueNickname);
                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .AssignEntityNickname(mRedPlayer, ENTITY_ID_PLAYER_RED, redNickname);

                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .SendGameData(mBluePlayer, blueNickname, redNickname);
                BattleGameServer::GetConstInstance()
                .GetConstStcRpc()
                .SendGameData(mRedPlayer, redNickname, blueNickname);

                mTimePoint = std::chrono::steady_clock::now();
                mTimeSet = std::chrono::seconds(TIME_GAME_ONE_ROUND);
                mGameState = STATE_PLAY_GAME;

                SetRemainTimeTo(mBluePlayer, "라운드 종료까지");
                SetRemainTimeTo(mRedPlayer, "라운드 종료까지");

                auto signalGameStart = [](ClientId to)
                {
                    BattleGameServer::GetConstInstance()
                    .GetConstStcRpc()
                    .SignalGameState(to, STATE_PLAY_GAME);
                };
                signalGameStart(mBluePlayer);
                signalGameStart(mRedPlayer);
            }
            return;
        }
        case STATE_PLAY_GAME:
        {
            if (!mIsBlueValid || !mIsRedValid // 누가 탈주하면 게임 바로 종료
            || mBlueScore >= 5 || mRedScore >= 5) // 5점 이상 획득한 플레이어가 있다면 종료
            {
                if (!mIsBlueValid || !mIsRedValid)
                {
                    auto sendBadGameResult = [](ClientId to) {BattleGameServer::GetConstInstance()
                    .GetConstStcRpc()
                    .SendGameResult(to, false, false, 0, 0);};
                    if (mIsBlueValid)
                        sendBadGameResult(mBluePlayer);
                    else if (mIsRedValid)
                        sendBadGameResult(mRedPlayer);
                }
                else
                {
                    auto sendGoodGameResult = [this](ClientId to)
                    {
                        auto winner = mBlueScore > mRedScore ? mBluePlayer : mRedPlayer;
                        BattleGameServer::GetConstInstance()
                        .GetConstStcRpc()
                        .SendGameResult(
                                to,
                                true,
                                winner == to,
                                to == mBluePlayer ? mBlueScore : mRedScore,
                                to == mBluePlayer ? mRedScore : mBlueScore);
                    };
                    sendGoodGameResult(mBluePlayer);
                    sendGoodGameResult(mRedPlayer);
                }
                mTimePoint = std::chrono::steady_clock::now();
                mTimeSet = std::chrono::seconds(TIME_GAME_END);
                mGameState = STATE_GAME_END;

                SetRemainTimeTo(mBluePlayer, "메인 화면으로 복귀");
                SetRemainTimeTo(mRedPlayer, "메인 화면으로 복귀");

                auto signalGameEnd = [](ClientId to)
                {
                    BattleGameServer::GetConstInstance()
                            .GetConstStcRpc()
                            .SignalGameState(to, STATE_GAME_END);
                };
                signalGameEnd(mBluePlayer);
                signalGameEnd(mRedPlayer);
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

                SetRemainTimeTo(mBluePlayer, "라운드 종료까지");
                SetRemainTimeTo(mRedPlayer, "라운드 종료까지");
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
{return !mIsBlueValid && !mIsRedValid;}

bool OneVsOneGameRoom::IsFull() const noexcept
{return mIsBlueValid && mIsRedValid;}

bool OneVsOneGameRoom::IsPlayerJoined(ClientId clientId) const noexcept
{
    return
    ((mIsBlueValid && clientId == mBluePlayer)
    || (mIsRedValid && clientId == mRedPlayer));
}

std::vector<ClientId> OneVsOneGameRoom::GetAllPlayers() const noexcept
{
    if (!mIsBlueValid && !mIsRedValid)
    {return {};}
    else if (mIsBlueValid && mIsRedValid)
    {return {mBluePlayer, mRedPlayer};}
    else
    {return {(mIsBlueValid ? mBluePlayer : mRedPlayer)};}
}

void OneVsOneGameRoom::OnEntityMove(EntityId entityId, const Vector& location, double direction) noexcept
{}

void OneVsOneGameRoom::OnPlayerMove(ClientId clientId, const Vector& location, double direction) noexcept
{
    if (!mIsBlueValid || !mIsRedValid)
    {return;}

    if (clientId == mBluePlayer)
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
            clientId == mBluePlayer ? mRedPlayer : mBluePlayer,
            clientId == mBluePlayer ? ENTITY_ID_PLAYER_BLUE : ENTITY_ID_PLAYER_RED,
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

    EntityId destroyed = (clientId == mBluePlayer ? ENTITY_ID_PLAYER_BLUE : ENTITY_ID_PLAYER_RED);

    IncrementScore(clientId == mBluePlayer ? TEAM_ID_RED : TEAM_ID_BLUE);
    RespawnPlayer(clientId == mBluePlayer ? TEAM_ID_BLUE : TEAM_ID_RED);

    mTimePoint = std::chrono::steady_clock::now();
    mTimeSet = std::chrono::seconds(TIME_GAME_ONE_ROUND);

    SetRemainTimeTo(mBluePlayer, "라운드 종료까지");
    SetRemainTimeTo(mRedPlayer, "라운드 종료까지");
}

void OneVsOneGameRoom::IncrementScore(int teamId) noexcept
{
    if (!mIsBlueValid || !mIsRedValid || mGameState != STATE_PLAY_GAME)
    {return;}

    int scoreToSend;
    if (teamId == TEAM_ID_BLUE)
    {scoreToSend = ++mBlueScore;}
    else
    {scoreToSend = ++mRedScore;}

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .SetScore(mBluePlayer, teamId, scoreToSend);

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .SetScore(mRedPlayer, teamId, scoreToSend);
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
    respawnTo(mBluePlayer);
    respawnTo(mRedPlayer);
}