//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_STCRPC_H
#define BATTLEGAME_SERVER_STCRPC_H

#include <functional>
#include "Message.h"

class StcRpc {
public:
    StcRpc(std::function<void(int, Message&&)> sendEnqueueFunction);
    ~StcRpc() = default;

    static constexpr int STC_TEST = 1;
    void Test() const;

private:
    std::function<void(int, Message&&)> SendEnqueue;
};


#endif //BATTLEGAME_SERVER_STCRPC_H
