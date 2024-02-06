//
// Created by floweryclover on 2024-01-25.
//

#ifndef BATTLEGAME_SERVER_SOCKET_H
#define BATTLEGAME_SERVER_SOCKET_H

#ifdef _WIN32
using UINT_PTR = unsigned long long;
using SOCKET = UINT_PTR;
#elifdef linux
using SOCKET = int;
#endif

class Socket {
public:
    Socket(const Socket& rhs) = delete;
    Socket& operator=(const Socket& rhs) = delete;

    explicit Socket() noexcept;
    explicit Socket(SOCKET hSocket) noexcept;
    Socket(Socket&& rhs) noexcept;
    ~Socket();

    void Shutdown();
    inline SOCKET GetRawHandle() const { return this->mSocketHandle; }
private:
    SOCKET mSocketHandle;
};


#endif //BATTLEGAME_SERVER_SOCKET_H
