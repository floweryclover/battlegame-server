//
// Created by floweryclover on 2024-02-07.
//

#ifndef BATTLEGAME_SERVER_UNREALTYPES_H
#define BATTLEGAME_SERVER_UNREALTYPES_H

class Vector2D
{
public:
    explicit Vector2D() noexcept : X(0.0), Y(0.0) {}
    explicit Vector2D(double x, double y) noexcept : X(x), Y(y) {}
    static Vector2D FromDirection(double r) noexcept;
    double X;
    double Y;
    inline const Vector2D operator*(double multiplier) const noexcept { return Vector2D { this->X*multiplier, this->Y*multiplier }; }
    inline const Vector2D& operator*=(double multiplier) noexcept { this->X *= multiplier; this->Y *= multiplier; return *this; }
    inline const Vector2D operator-(const Vector2D& rhs) const noexcept {return Vector2D {this->X - rhs.X, this->Y - rhs.Y};}
    double GetDirectionTo(const Vector2D to) noexcept;
};

class Vector3D
{
public:
    explicit Vector3D() noexcept : X(0.0), Y(0.0), Z(0.0) {}
    explicit Vector3D(double r) noexcept : X(r), Y(r), Z(r) {}
    explicit Vector3D(double x, double y, double z) noexcept : X(x), Y(y), Z(z) {}
    double X;
    double Y;
    double Z;
    inline const Vector3D operator+(const Vector3D& rhs) const noexcept {return Vector3D {this->X + rhs.X, this->Y + rhs.Y, this->Z + rhs.Z };}
    inline const Vector3D operator-(const Vector3D& rhs) const noexcept {return Vector3D {this->X - rhs.X, this->Y - rhs.Y, this->Z - rhs.Z };}

    inline Vector2D GetXyVector() const noexcept { return Vector2D {this->X, this->Y}; }
    double DistanceTo(const Vector3D& rhs) const noexcept;
};








#endif //BATTLEGAME_SERVER_UNREALTYPES_H
