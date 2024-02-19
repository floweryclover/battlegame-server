//
// Created by floweryclover on 2024-01-27.
//

#include "CtsRpc.h"
#include "StcRpc.h"
#include "GameData.h"
#include "BattleGameServer.h"
#include "ClientManager.h"
#include "GameRoomManager.h"
#include <iostream>

void CtsRpc::HandleMessage(const Context& context, const Message& message) const noexcept
{
    switch (message.mHeaderMessageType)
    {
        case CtsRpc::CTS_REQUEST_MATCHMAKING:
        {
            OnRequestMatchMaking(context);
            break;
        }
        case CtsRpc::CTS_MOVE_CHARACTER:
        {
            double x, y, z, direction;
            memcpy(&x, message.mpBodyBuffer.get(), 8);
            memcpy(&y, message.mpBodyBuffer.get() + 8, 8);
            memcpy(&z, message.mpBodyBuffer.get() + 16, 8);
            memcpy(&direction, message.mpBodyBuffer.get() + 24, 8);
            OnMoveCharacter(context, Vector{x, y, z}, direction);
            break;
        }
        case CtsRpc::CTS_NOTIFY_BATTLEGAME_PREPARED:
        {
            OnNotifyBattleGamePrepared(context);
            break;
        }
        case CtsRpc::CTS_NOTIFY_OWNING_CHARACTER_DESTROYED:
        {
            OnNotifyOwningCharacterDestroyed(context);
            break;
        }
        case CtsRpc::CTS_SET_NICKNAME:
        {
            if (message.mHeaderBodySize > 24)
            {return;}

            char nicknameChars[24];
            memset(nicknameChars, 0, 24);
            memcpy(nicknameChars, message.mpBodyBuffer.get(), message.mHeaderBodySize);
            OnSetNickname(context, std::string(nicknameChars, &nicknameChars[message.mHeaderBodySize]));
            break;
        }
        case CtsRpc::CTS_REQUEST_MY_NICKNAME:
        {
            OnRequestMyNickname(context);
            break;
        }
    }
}

void CtsRpc::OnRequestMatchMaking(const Context& context) const noexcept
{
    BattleGameServer::GetInstance()
    .GetGameData()
    .GetGameRoomManager()
    .StartMatchMaking(context.GetClientId());
}

void CtsRpc::OnMoveCharacter(const Context &context, const Vector& position, double direction) const noexcept
{
    auto roomIdOption = BattleGameServer::GetInstance()
    .GetGameData()
    .GetGameRoomManager()
    .GetPlayerJoinedRoomId(context.GetClientId());
    if (!roomIdOption.has_value())
    {
        std::cerr << "[에러] 클라이언트 " << context.GetClientId() << " 가 속한 방 ID를 검색했으나 존재하지 않았습니다." << std::endl;
        return;
    }

    auto pGameRoom = dynamic_cast<IHasEntity*>(BattleGameServer::GetInstance()
    .GetGameData()
    .GetGameRoomManager()
    .GetGameRoom(roomIdOption.value()));

    if (pGameRoom != nullptr)
    {pGameRoom->OnPlayerMove(context.GetClientId(), position, direction);}
}

void CtsRpc::OnNotifyBattleGamePrepared(const Context &context) const noexcept
{
    auto gameRoomOption = BattleGameServer::GetInstance().GetGameData().GetGameRoomManager().GetPlayerJoinedRoomId(context.GetClientId());
    if (!gameRoomOption.has_value())
    {return;}

    auto pBaseRoom = BattleGameServer::GetInstance()
    .GetGameData()
    .GetGameRoomManager()
    .GetGameRoom(gameRoomOption.value());

    if (pBaseRoom == nullptr)
    {
        std::cerr << "클라이언트 " << context.GetClientId() << ": 유효하지 않은 방에 OnPrepared() 호출" << std::endl;
        return;
    }

    pBaseRoom->OnPlayerPrepared(context.GetClientId());
}

void CtsRpc::OnNotifyOwningCharacterDestroyed(const Context &context) const noexcept
{
    auto gameRoomOption = BattleGameServer::GetInstance().GetConstGameData().GetConstGameRoomManager().GetPlayerJoinedRoomId(context.GetClientId());
    if (!gameRoomOption.has_value())
    {return;}

    auto pBaseRoom = BattleGameServer::GetInstance()
    .GetGameData()
    .GetGameRoomManager()
    .GetGameRoom(gameRoomOption.value());
    if (pBaseRoom == nullptr)
    {
        std::cerr << "클라이언트 " << context.GetClientId() << ": 유효하지 않은 방에 OnOwningCharacterDestroyed() 호출" << std::endl;
        return;
    }

    auto pHasEntityRoom = dynamic_cast<IHasEntity*>(pBaseRoom);
    if (pHasEntityRoom == nullptr)
    {return;}

    pHasEntityRoom->OnOwningCharacterDestroyed(context.GetClientId());
}

void CtsRpc::OnSetNickname(const Context &context, const std::string &nickname) const noexcept
{
    BattleGameServer::GetInstance().GetGameData().SetPlayerNickname(context.GetClientId(), nickname);
    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .GetMyNickname(context.GetClientId(), *BattleGameServer::GetConstInstance().GetConstGameData().GetPlayerNickname(context.GetClientId()));
}

void CtsRpc::OnRequestMyNickname(const Context& context) const noexcept
{
    auto pClientNickname = BattleGameServer::GetConstInstance()
    .GetConstGameData()
    .GetPlayerNickname(context.GetClientId());

    if (pClientNickname == nullptr)
    {return;}

    BattleGameServer::GetConstInstance()
    .GetConstStcRpc()
    .GetMyNickname(context.GetClientId(), *pClientNickname);
}