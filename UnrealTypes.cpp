//
// Created by floweryclover on 2024-02-07.
//

#include "UnrealTypes.h"
#include <cmath>
#ifdef _WIN32
constexpr double M_PI = 3.1415926;
#endif
double Vector3D::DistanceTo(const Vector3D &rhs) const noexcept
{
    auto vector = (*this)-rhs;
    return sqrt(vector.X*vector.X + vector.Y+vector.Y + vector.Z+vector.Z);
}

Vector2D Vector2D::FromDirection(double r) noexcept
{
    return Vector2D { std::cos(r*(M_PI/180)), std::sin(r*(M_PI/180)) };
}

double Vector2D::GetDirectionTo(const Vector2D to) noexcept
{
    auto directionVector = to-*this;
    double angle = std::atan2(directionVector.Y, directionVector.X);
    if (angle < 0)
    {
        angle += 2 * M_PI;
    }

    return angle * (180.0 / M_PI);
}