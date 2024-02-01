//
// Created by floweryclover on 2024-01-27.
//

#include "StcRpc.h"
#include "Message.h"

StcRpc::StcRpc(std::function<void(unsigned int, Message&&)> sendEnqueue) noexcept : mSendEnqueue(std::move(sendEnqueue))
{

}

void StcRpc::JoinedGameRoom(ConnectionId to) const noexcept
{
    Message message(0, STC_JOINED_GAME_ROOM, nullptr);
    mSendEnqueue(to, std::move(message));
}

void StcRpc::DisconnectedFromGame(ConnectionId to) const noexcept
{
    Message message(0, STC_DISCONNECTED_FROM_GAME, nullptr);
    mSendEnqueue(to, std::move(message));
}