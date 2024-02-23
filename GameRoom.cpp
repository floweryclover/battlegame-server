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

const Vector3D OneVsOneGameRoom::CENTER_LOCATION = Vector3D {0, 0, 0};

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
mIsRedValid(false),
mBlueLastCommand(0),
mRedLastCommand(0),
mBlueLastCommandTime(std::chrono::steady_clock::now()),
mRedLastCommandTime(std::chrono::steady_clock::now()),
mBlueLastDelay(std::chrono::milliseconds(0)),
mRedLastDelay(std::chrono::milliseconds(0))
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

                mBlueLocation = Vector3D {180, 0, 1000};
                mRedLocation = Vector3D {-180, 0, 1000};
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

            // 공격 충전 중인 플레이어가 있는 경우, 최대 차징시 자동 공격
            auto attackIfFullCharged = [this, &currentTime](ClientId clientId)
            {
                bool isBlue = clientId == mBluePlayer;
                int lastCommand = isBlue ? mBlueLastCommand : mRedLastCommand;
                if (lastCommand != 1)
                    return;

                auto lastCommandTime = isBlue ? mBlueLastCommandTime : mRedLastCommandTime;
                if (currentTime > lastCommandTime + std::chrono::milliseconds(1000))
                    HandleCommand(isBlue, 2);
            };
            attackIfFullCharged(mBluePlayer);
            attackIfFullCharged(mRedPlayer);

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

void OneVsOneGameRoom::OnEntityMove(EntityId entityId, const Vector3D& location, double direction) noexcept
{}

void OneVsOneGameRoom::OnPlayerMove(ClientId clientId, const Vector3D& location, double direction) noexcept
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
    RespawnPlayer(TEAM_ID_BLUE);
    RespawnPlayer(TEAM_ID_RED);

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
                               teamId == TEAM_ID_BLUE ? (mBlueLocation = Vector3D {100, 0, 1000}) : (mRedLocation = Vector3D {-100, 0, 1000}),
                               teamId == TEAM_ID_BLUE ? (mBlueAngle = 180) : (mRedAngle = 0));
    };
    respawnTo(mBluePlayer);
    respawnTo(mRedPlayer);
}

void OneVsOneGameRoom::OnPlayerEnterCommand(ClientId clientId, int commandId) noexcept
{
    if (mGameState != STATE_PLAY_GAME
    || (clientId != mBluePlayer && clientId != mRedPlayer))
        return;

    HandleCommand(clientId == mBluePlayer, commandId);
}

void OneVsOneGameRoom::HandleCommand(bool isBlue, int commandId)
{
    // 알 수 없는 커맨드면 반환
    if (commandId < 0 || commandId > 3)
        return;

    // 후딜레이 중이면 반환
    auto currentTime = std::chrono::steady_clock::now();
    auto& lastCommandTime = isBlue ? mBlueLastCommandTime : mRedLastCommandTime;
    auto& lastDelay = isBlue ? mBlueLastDelay : mRedLastDelay;
    auto& lastCommand = isBlue ? mBlueLastCommand : mRedLastCommand;
    if (currentTime < lastCommandTime + lastDelay)
        return;

    // 참고: case에 해당하는 커맨드만이 switch에 들어올 것이 보장됨
    switch (commandId)
    {
        case 0: // 커맨드 비우기
        {
            lastDelay = std::chrono::milliseconds(0);
            break;
        }
        case 1: // 공격 충전
        {
            if (lastCommand == 1)
                return;

            lastDelay = std::chrono::milliseconds(0); // 공격 커맨드를 받아야 하니 딜레이 0초 주기
            break;
        }
        case 2: // 공격 (0.5초 미만 충전 공격은 일반공격으로 약한 넉백, 0.5초 이상 충전 공격은 강한 넉백 충전 후 1초 경과시 자동 공격)
        {
            // 공격 충전 중에만 공격할 수 있음
            if (lastCommand != 1)
                return;

            std::cout << isBlue << std::endl;
            // 공격 범위 내에 있는지 검증
            auto attackerLocation = isBlue ? mBlueLocation : mRedLocation;
            auto targetLocation = isBlue ? mRedLocation : mBlueLocation;
            double attackerAngle = isBlue ? mBlueAngle : mRedAngle;
            double toTargetAngle =  attackerLocation.GetXyVector().GetDirectionTo(targetLocation.GetXyVector());
            double absDeltaAngle = std::abs(toTargetAngle-attackerAngle);
            absDeltaAngle = 360-absDeltaAngle < absDeltaAngle ? 360-absDeltaAngle : absDeltaAngle;
            double distance = attackerLocation.DistanceTo(targetLocation);
            if (absDeltaAngle < 30.0 && distance < 150.0)
            {
                auto impulse = Vector2D::FromDirection(isBlue ? mBlueAngle : mRedAngle);
                if (currentTime > lastCommandTime + std::chrono::milliseconds(500)) // 강한 공격
                {
                    impulse *= 500.0;
                }
                else // 일반 공격
                {
                    impulse *= 200.0;
                }

                auto knockBackTo = [this, impulse, isBlue](ClientId to)
                {
                    BattleGameServer::GetConstInstance()
                            .GetConstStcRpc()
                            .KnockbackEntity(
                                    to,
                                    isBlue ? ENTITY_ID_PLAYER_RED : ENTITY_ID_PLAYER_BLUE,
                                    isBlue ? mRedLocation : mBlueLocation,
                                    impulse);
                };
                knockBackTo(mBluePlayer);
                knockBackTo(mRedPlayer);
            }

            lastDelay = std::chrono::milliseconds(1000); // 1초의 딜레이 주기
            break;
        }
        case 3: // 패링 (0.5초 지속 및 1초의 대기시간, 지속시간 내 상대방의 공격시  상대방에게 공격을 되돌려 줌)
        {
            // 공격 충전 중에는 패링을 할 수 없음
            if (lastCommand == 1)
                return;
            // 미구현...

            lastDelay = std::chrono::milliseconds(1000); // 1초의 딜레이 주기
            break;
        }
    }

    lastCommandTime = currentTime;
    lastCommand = commandId;
}