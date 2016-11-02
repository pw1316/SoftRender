#include "Vertex.h"
#include <cmath>

/* 3D Point/Vector class BEGIN*/
Vertex3F Vertex3F::operator+(const Vertex3F & rhs) const
{
    FLOAT x = this->x_ + rhs.x_;
    FLOAT y = this->y_ + rhs.y_;
    FLOAT z = this->z_ + rhs.z_;
    return Vertex3F(x, y, z);
}

Vertex3F & Vertex3F::operator+=(const Vertex3F & rhs)
{
    this->x_ += rhs.x_;
    this->y_ += rhs.y_;
    this->z_ += rhs.z_;
    return this->zeroFix();
}

Vertex3F Vertex3F::operator-() const
{
    FLOAT x = 0.0f - this->x_;
    FLOAT y = 0.0f - this->y_;
    FLOAT z = 0.0f - this->z_;
    return Vertex3F(x, y, z);
}

Vertex3F Vertex3F::operator-(const Vertex3F & rhs) const
{
    FLOAT x = this->x_ - rhs.x_;
    FLOAT y = this->y_ - rhs.y_;
    FLOAT z = this->z_ - rhs.z_;
    return Vertex3F(x, y, z);
}

Vertex3F & Vertex3F::operator-=(const Vertex3F & rhs)
{
    this->x_ -= rhs.x_;
    this->y_ -= rhs.y_;
    this->z_ -= rhs.z_;
    return this->zeroFix();
}

Vertex3F Vertex3F::operator*(const FLOAT & rhs) const
{
    FLOAT x = this->x_ * rhs;
    FLOAT y = this->y_ * rhs;
    FLOAT z = this->z_ * rhs;
    return Vertex3F(x, y, z);
}

Vertex3F & Vertex3F::operator*=(const FLOAT & rhs)
{
    this->x_ *= rhs;
    this->y_ *= rhs;
    this->z_ *= rhs;
    return this->zeroFix();
}

Vertex3F Vertex3F::operator/(const FLOAT & rhs) const
{
    Assert(rhs != 0.0f);
    FLOAT x = this->x_ / rhs;
    FLOAT y = this->y_ / rhs;
    FLOAT z = this->z_ / rhs;
    return Vertex3F(x, y, z);
}

Vertex3F & Vertex3F::operator/=(const FLOAT & rhs)
{
    Assert(rhs != 0.0f);
    this->x_ /= rhs;
    this->y_ /= rhs;
    this->z_ /= rhs;
    return this->zeroFix();
}

Vertex3F & Vertex3F::operator=(const Vertex3F & rhs)
{
    if (this != &rhs)
    {
        this->x_ = rhs.x_;
        this->y_ = rhs.y_;
        this->z_ = rhs.z_;
    }
    return this->zeroFix();
}

BOOL Vertex3F::operator==(const Vertex3F & rhs)
{
    FLOAT dx = this->x_ - rhs.x_;
    FLOAT dy = this->y_ - rhs.y_;
    FLOAT dz = this->z_ - rhs.z_;
    return (isAndSetZero(dx) && isAndSetZero(dy) && isAndSetZero(dz));
}

BOOL Vertex3F::operator!=(const Vertex3F & rhs)
{
    return !(*this == rhs);
}

FLOAT & Vertex3F::operator[](const int & index)
{
    if (index <= 1)
    {
        return this->x_;
    }
    if (index == 2)
    {
        return this->y_;
    }
    return this->z_;
}

FLOAT Vertex3F::length() const
{
    FLOAT len = sqrt(this->x_ * this->x_ + this->y_ * this->y_ + this->z_ * this->z_);
    isAndSetZero(len);
    return len;
}

Vertex3F Vertex3F::normalize() const
{
    Vertex3F tmp = *this;
    FLOAT len = this->length();
    if (len != 0.0f)
    {
        tmp /= len;
    }
    return tmp.zeroFix();
}

FLOAT Vertex3F::distance(const Vertex3F & rhs) const
{
    return (*this - rhs).length();
}

FLOAT Vertex3F::dotProduct(const Vertex3F & rhs) const
{
    FLOAT tmp = this->x_ * rhs.x_ + this->y_ * rhs.y_ + this->z_ * rhs.z_;
    isAndSetZero(tmp);
    return tmp;
}

Vertex3F Vertex3F::crossProduct(const Vertex3F & rhs) const
{
    FLOAT x = this->y_ * rhs.z_ - this->z_ * rhs.y_;
    FLOAT y = this->z_ * rhs.x_ - this->x_ * rhs.z_;
    FLOAT z = this->x_ * rhs.y_ - this->y_ * rhs.x_;
    return Vertex3F(x, y, z);
}

Vertex4F Vertex3F::toPoint4F() const
{
    return Vertex4F(this->x_, this->y_, this->z_, 1.0f);
}

Vertex4F Vertex3F::toVector4F() const
{
    return Vertex4F(this->x_, this->y_, this->z_, 0.0f);
}

Vertex3F Vertex3F::zero()
{
    return Vertex3F();
}

Vertex3F & Vertex3F::zeroFix()
{
    isAndSetZero(x_);
    isAndSetZero(y_);
    isAndSetZero(z_);
    return *this;
}
/* 3D Point/Vector class END*/

/* 4D Point/Vector class BEGIN*/
Vertex4F & Vertex4F::operator=(const Vertex4F & rhs)
{
    if (this != &rhs)
    {
        this->x_ = rhs.x_;
        this->y_ = rhs.y_;
        this->z_ = rhs.z_;
        this->w_ = rhs.w_;
    }
    return this->zeroFix();
}

Vertex4F Vertex4F::normalize() const
{
    Vertex4F tmp = *this;
    if (tmp.w_ != 0.0f)
    {
        tmp.x_ /= tmp.w_;
        tmp.y_ /= tmp.w_;
        tmp.z_ /= tmp.w_;
        tmp.w_ /= tmp.w_;
    }
    return tmp.zeroFix();
}

Vertex3F Vertex4F::toVertex3F() const
{
    Vertex4F tmp = this->normalize();
    return Vertex3F(tmp.x_, tmp.y_, tmp.z_);
}

Vertex4F & Vertex4F::zeroFix()
{
    isAndSetZero(x_);
    isAndSetZero(y_);
    isAndSetZero(z_);
    isAndSetZero(w_);
    return *this;
}
/* 4D Point/Vector class END*/
