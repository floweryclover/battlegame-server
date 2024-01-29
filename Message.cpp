//
// Created by floweryclover on 2024-01-27.
//

#include "Message.h"
#include <cstring>

Message::Message(const int headerBodySize, const int headerMessageType, const char* const bodyOnlySource) : mHeaderBodySize(headerBodySize), mHeaderMessageType(headerMessageType)
{
    mBodyBuffer = new char[mHeaderBodySize];
    memcpy(mBodyBuffer, bodyOnlySource, headerBodySize);
}

Message::~Message()
{
    delete[] mBodyBuffer;
}

