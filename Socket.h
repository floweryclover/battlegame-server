//
// Created by floweryclover on 2024-01-25.
//

#ifndef BATTLEGAME_SERVER_SOCKET_H
#define BATTLEGAME_SERVER_SOCKET_H


class Socket {
public:
    Socket(int hSocket);
    Socket(const Socket& rhs) = delete;
    ~Socket();

    inline operator int() { return this->hSocket_; }
private:
    int hSocket_;
};


#endif //BATTLEGAME_SERVER_SOCKET_H
