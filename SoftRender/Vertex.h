#pragma once
#include <windows.h>

/* Define assertion */
#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

inline BOOL isAndSetZero(FLOAT &value);
/* Class Declaration */
class Vertex3F;
class Vertex4F;

/* 3D Point/Vector class BEGIN*/
class Vertex3F
{
public:
    Vertex3F() :x_(0.0f), y_(0.0f), z_(0.0f) {}
    Vertex3F(FLOAT x, FLOAT y, FLOAT z) :x_(x), y_(y), z_(z) { this->zeroFix(); }
    ~Vertex3F() {}
    /* Operator */
    Vertex3F operator+(const Vertex3F & rhs) const;//向量加法
    Vertex3F & operator+=(const Vertex3F & rhs);
    Vertex3F operator-() const;//反向量
    Vertex3F operator-(const Vertex3F & rhs) const;//向量减法
    Vertex3F & operator-=(const Vertex3F & rhs);
    Vertex3F operator*(const FLOAT & rhs) const;//标量乘
    Vertex3F & operator*=(const FLOAT & rhs);
    Vertex3F operator/(const FLOAT & rhs) const;//标量除
    Vertex3F & operator/=(const FLOAT & rhs);
    Vertex3F & operator=(const Vertex3F & rhs);//赋值
    BOOL operator==(const Vertex3F & rhs);//相等
    BOOL operator!=(const Vertex3F & rhs);//不相等
    /* Other operation*/
    FLOAT length() const;//向量模
    Vertex3F normalize() const;//归一化
    FLOAT dotProduct(const Vertex3F & rhs) const;//点乘
    Vertex3F crossProduct(const Vertex3F & rhs) const;//叉乘
    Vertex4F toPoint4F() const;//齐次坐标点
    Vertex4F toVector4F() const;//齐次向量
private:
    Vertex3F & zeroFix();//把接近0.0f的值改成0.0f，本类的成员函数的返回值全可以直接和0.0f比较
    float x_, y_, z_;
};

inline FLOAT length(Vertex3F & lhs)
{
    return lhs.length();
}

inline Vertex3F normalize(Vertex3F & lhs)
{
    return lhs.normalize();
}

inline FLOAT dotProduct(const Vertex3F & lhs, const Vertex3F & rhs)
{
    return lhs.dotProduct(rhs);
}

inline Vertex3F crossProduct(const Vertex3F & lhs, const Vertex3F & rhs)
{
    return lhs.crossProduct(rhs);
}
/* 3D Point/Vector class END*/

/* 4D Point/Vector class BEGIN*/
class Vertex4F
{
public:
    Vertex4F() {}
    Vertex4F(FLOAT x, FLOAT y, FLOAT z, FLOAT w) :x_(x), y_(y), z_(z), w_(w) { this->zeroFix(); }
    ~Vertex4F() {}/* Operator */
    Vertex4F & operator=(const Vertex4F & rhs);
    /* Other operation*/
    Vertex4F normalize() const;//乘投影矩阵后把w变成1来缩放
    Vertex3F toVertex3F() const;//3D向量
private:
    Vertex4F & zeroFix();//把接近0.0f的值改成0.0f，本类的成员函数的返回值全可以直接和0.0f比较
    FLOAT x_, y_, z_, w_;
};
/* 4D Point/Vector class END*/
