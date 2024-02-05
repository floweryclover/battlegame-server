//
// Created by floweryclover on 2024-01-27.
//

#include "Message.h"
#include <cstring>
#include <cassert>

Message::Message(const int headerBodySize, const int headerMessageType, const char* const bodyOnlySource) noexcept : mHeaderBodySize(headerBodySize), mHeaderMessageType(headerMessageType)
{
    if (headerBodySize > 0)
    {
        assert(bodyOnlySource != nullptr);
        char* pBodyBuffer = new char[mHeaderBodySize];
        memcpy(pBodyBuffer, bodyOnlySource, headerBodySize);
        mpBodyBuffer = std::unique_ptr<char>(pBodyBuffer);
    }
    else
    {
        assert(bodyOnlySource == nullptr);
        mpBodyBuffer = nullptr;
    }
}

Message::Message() noexcept : mHeaderBodySize(0), mHeaderMessageType(0), mpBodyBuffer(nullptr) {}

Message::Message(Message&& rhs) noexcept : mHeaderBodySize(rhs.mHeaderBodySize), mHeaderMessageType(rhs.mHeaderMessageType), mpBodyBuffer(std::move(rhs.mpBodyBuffer)) {}