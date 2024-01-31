//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_STCRPC_H
#define BATTLEGAME_SERVER_STCRPC_H

#include <functional>

class Message;

class StcRpc {
public:
    StcRpc(const StcRpc& rhs) = delete;
    StcRpc(StcRpc&& rhs) = delete;
    StcRpc& operator=(const StcRpc& rhs) = delete;

    explicit StcRpc(std::function<void(unsigned int, Message&&)> sendEnqueue) noexcept;
    ~StcRpc() = default;

private:
    std::function<void(unsigned int, Message&&)> mSendEnqueue;
};


#endif //BATTLEGAME_SERVER_STCRPC_H
