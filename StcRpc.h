//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_STCRPC_H
#define BATTLEGAME_SERVER_STCRPC_H

#include <functional>
#include "Message.h"

class StcRpc {
public:
    StcRpc(const StcRpc& rhs) = delete;
    StcRpc(StcRpc&& rhs) = delete;
    StcRpc& operator=(const StcRpc& rhs) = delete;

    static void RegisterSendEnqueueFunction(std::function<void(unsigned int, Message&&)> sendEnqueue);
    static inline StcRpc& GetInstance() { static StcRpc instance; return instance; }
    ~StcRpc() = default;

private:
    explicit StcRpc() = default;
    std::function<void(unsigned int, Message&&)> SendEnqueue;
};


#endif //BATTLEGAME_SERVER_STCRPC_H
