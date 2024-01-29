//
// Created by floweryclover on 2024-01-27.
//

#include "Message.h"
#include <cstring>
#include <cassert>

Message::Message(const int headerBodySize, const int headerMessageType, const char* const bodyOnlySource) : mHeaderBodySize(headerBodySize), mHeaderMessageType(headerMessageType)
{
    if (headerBodySize > 0)
    {
        assert(bodyOnlySource != nullptr);
        mBodyBuffer = new char[mHeaderBodySize];
        memcpy(mBodyBuffer, bodyOnlySource, headerBodySize);
    }
    else
    {
        assert(bodyOnlySource == nullptr);
        mBodyBuffer = nullptr;
    }
}

Message::~Message()
{
    if (mBodyBuffer != nullptr)
    {
        delete[] mBodyBuffer;
    }
}

