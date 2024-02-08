//
// Created by floweryclover on 2024-02-07.
//

#ifndef BATTLEGAME_SERVER_UNREALTYPES_H
#define BATTLEGAME_SERVER_UNREALTYPES_H


class Vector {
public:
    explicit Vector() noexcept : X(0.0), Y(0.0), Z(0.0) {}
    explicit Vector(double r) noexcept : X(r), Y(r), Z(r) {}
    explicit Vector(double x, double y, double z) noexcept : X(x), Y(y), Z(z) {}
    double X;
    double Y;
    double Z;
};


#endif //BATTLEGAME_SERVER_UNREALTYPES_H
