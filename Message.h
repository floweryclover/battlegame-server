//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_MESSAGE_H
#define BATTLEGAME_SERVER_MESSAGE_H

#include <memory>

class Message {
public:
    explicit Message(const int headerBodySize, const int headerMessageType, const char* const bodyOnlySource) noexcept;
    explicit Message() noexcept;
    Message(Message&& rhs) noexcept;
    ~Message() = default;
    int mHeaderBodySize;
    int mHeaderMessageType;
    std::unique_ptr<char> mpBodyBuffer;
};


#endif //BATTLEGAME_SERVER_MESSAGE_H
