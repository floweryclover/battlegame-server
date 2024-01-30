//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_STCRPC_H
#define BATTLEGAME_SERVER_STCRPC_H

#include <functional>
#include "Message.h"

class StcRpc {
public:
    explicit StcRpc(std::function<void(unsigned int, Message&&)> sendEnqueueFunction);
    ~StcRpc() = default;

    static constexpr int STC_REQUEST_ENTER_NICKNAME = 1;
    void RequestEnterNickname(unsigned int connectionId) const;

private:
    std::function<void(unsigned int, Message&&)> SendEnqueue;
};


#endif //BATTLEGAME_SERVER_STCRPC_H
