//
// Created by floweryclover on 2024-01-27.
//

#ifndef BATTLEGAME_SERVER_CLIENT_H
#define BATTLEGAME_SERVER_CLIENT_H

#include <array>
#include <expected>
#include <optional>
#include <memory>
#include "Socket.h"
#include "Constants.h"
#include "IoResult.h"

using ErrorCode = int;
using SerializedEndpoint = unsigned long long;
using ClientId = SerializedEndpoint;

class Client {
public:
    Client(const Client& rhs) = delete;
    Client& operator=(const Client& rhs) = delete;
    explicit Client(ClientId clientId, Socket&& socket) noexcept;
    Client(Client&& rhs) noexcept;
    ~Client() = default;

    inline ClientId GetClientId() const { return this->mClientId; }
    inline const Socket& GetSocket() const { return this->mSocket; }

    /***
     * 현재 상태의 스트림에서 한 번 읽습니다.
     * 최대로 읽을 수 있는 양은, 현재 헤더를 읽어야 할 경우 헤더만큼, 이외의 경우 헤더에서 읽은 바디 크기만큼입니다.
     * @return 스트림이 유효할 경우\<T\>: 하나의 온전한 메세지를 모두 수신한 경우, 헤더의 메세지를 반환합니다.
     * @return 스트림이 유효하지 않은 경우\<E\>: 에러 코드를 반환하며, 정상 종료인 경우 값이 없습니다.
     */
    std::expected<std::optional<std::unique_ptr<class Message>>, std::optional<ErrorCode>> Receive();
private:
    const ClientId mClientId;
    const Socket mSocket;
    int mCurrentReceived;
    int mTotalSizeToReceive;
    int mLastReceivedHeaderType;
    bool mIsReceivingHeader;
    std::array<char, MAX_MESSAGE_SIZE> mReceiveBuffer;
};


#endif //BATTLEGAME_SERVER_CLIENT_H
