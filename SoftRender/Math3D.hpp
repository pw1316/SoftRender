#pragma once
#include "stdafx.h"
#include <cmath>
#include <cfloat>

const PWdouble EPSILON = 1e-7;

inline PWbool equal(PWdouble x, PWdouble y)
{
    return std::fabs(x - y) < EPSILON;
}

class Vector3d;
class Vector4d;
class Matrix44d;

class Vector3d
{
public:
    Vector3d() :m_x_(0), m_y_(0), m_z_(0) {}
    Vector3d(PWdouble x, PWdouble y, PWdouble z) : m_x_(x), m_y_(y), m_z_(z) {}

    /* Arithmetic */
    Vector3d operator- () const { return Vector3d(-m_x_, -m_y_, -m_z_); }
    Vector3d operator+ (const Vector3d &rhs) const { return Vector3d(m_x_ + rhs.m_x_, m_y_ + rhs.m_y_, m_z_ + rhs.m_z_); }
    Vector3d &operator+= (const Vector3d &rhs) { m_x_ += rhs.m_x_; m_y_ += rhs.m_y_; m_z_ += rhs.m_z_; return *this; }
    Vector3d operator- (const Vector3d &rhs) const { return Vector3d(m_x_ - rhs.m_x_, m_y_ - rhs.m_y_, m_z_ - rhs.m_z_); }
    Vector3d &operator-= (const Vector3d &rhs) { m_x_ -= rhs.m_x_; m_y_ -= rhs.m_y_; m_z_ -= rhs.m_z_; return *this; }
    Vector3d operator* (PWdouble a) const { return Vector3d(m_x_*a, m_y_*a, m_z_*a); }
    Vector3d &operator*= (PWdouble a) { m_x_ *= a; m_y_ *= a; m_z_ *= a; return *this; }
    Vector3d operator/ (PWdouble a) const { return Vector3d(m_x_ / a, m_y_ / a, m_z_ / a); }
    Vector3d &operator/= (PWdouble a) { m_x_ /= a; m_y_ /= a; m_z_ /= a; return *this; }

    /* Logic */
    PWbool operator== (const Vector3d &rhs) { return equal(m_x_, rhs.m_x_) && equal(m_y_, rhs.m_y_) && equal(m_z_, rhs.m_z_); }
    PWbool operator!= (const Vector3d &rhs) { return !operator==(rhs); }

    /* Vector */
    PWdouble lengthSquare() const { return m_x_ * m_x_ + m_y_ * m_y_ + m_z_ * m_z_; }
    PWdouble length() const { return std::sqrt(lengthSquare()); }
    Vector3d normal() const { return *this / std::fmax(length(), DBL_MIN); }
    PWdouble dot(const Vector3d &rhs) const { return m_x_ * rhs.m_x_ + m_y_ * rhs.m_y_ + m_z_* rhs.m_z_; }
    Vector3d cross(const Vector3d &rhs) const
    {
        return Vector3d(m_y_ * rhs.m_z_ - m_z_ * rhs.m_y_,
            m_z_ * rhs.m_x_ - m_x_ * rhs.m_z_,
            m_x_ * rhs.m_y_ - m_y_ * rhs.m_x_);
    }
    void normalize() { *this /= std::fmax(length(), DBL_MIN); }

    /* getter/setter */
    PWdouble getX() const { return m_x_; }
    PWdouble getY() const { return m_y_; }
    PWdouble getZ() const { return m_z_; }
    void setX(PWdouble v) { m_x_ = v; }
    void setY(PWdouble v) { m_y_ = v; }
    void setZ(PWdouble v) { m_z_ = v; }
    void set(PWdouble x, PWdouble y, PWdouble z) { m_x_ = x; m_y_ = y; m_z_ = z; }

    /* Deprecated */
    PWdouble &operator= (const Vector3d &ths);
    PWdouble operator[] (const PWint idx) const;
    PWdouble distance(const Vector3d &rhs) const;
    Vector4d toVector4d1() const;
    Vector4d toVector4d0() const;
    void zeroFix();
private:
    PWdouble m_x_, m_y_, m_z_;
};

class Vector4d
{
public:
    Vector4d() :m_x_(0), m_y_(0), m_z_(0), m_w_(0) {}
    Vector4d(PWdouble x, PWdouble y, PWdouble z, PWdouble w) : m_x_(x), m_y_(y), m_z_(z), m_w_(w) {}

    /* Arithmetic */
    Vector4d operator/ (PWdouble w) const { return Vector4d(m_x_ / w, m_y_ / w, m_z_ / w, m_w_ / w); }
    Vector4d &operator/= (PWdouble w) { m_x_ /= w; m_y_ /= w; m_z_ /= w; m_w_ /= w; return *this; }

    /* Vector */
    void normalize()
    {
        if (!equal(m_w_, 0))
        {
            *this /= m_w_;
        }
    }

    /* getter/setter */
    PWdouble getX() const { return m_x_; }
    PWdouble getY() const { return m_y_; }
    PWdouble getZ() const { return m_z_; }
    PWdouble getW() const { return m_w_; }
    void setX(PWdouble v) { m_x_ = v; }
    void setY(PWdouble v) { m_y_ = v; }
    void setZ(PWdouble v) { m_z_ = v; }
    void setW(PWdouble v) { m_w_ = v; }
    void set(PWdouble x, PWdouble y, PWdouble z, PWdouble w) { m_x_ = x; m_y_ = y; m_z_ = z; m_w_ = w; }

    /* Deprecated */
    PWdouble &operator= (const Vector4d &ths);
    PWdouble operator[] (const PWint idx) const;
    void zeroFix();

    /* TODO */
    Vector4d product(const Matrix44d &rhs) const;
    Vector3d toVertex3d() const;
    void zeroFix();
private:
    PWdouble m_x_, m_y_, m_z_, m_w_;
};
