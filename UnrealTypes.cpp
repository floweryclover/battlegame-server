//
// Created by floweryclover on 2024-02-07.
//

#include "UnrealTypes.h"
#include <cmath>

double Vector::DistanceTo(const Vector &rhs) const noexcept
{
    auto vector = (*this)-rhs;
    return sqrt(vector.X*vector.X + vector.Y+vector.Y + vector.Z+vector.Z);
}