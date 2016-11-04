#pragma once
#include "common.h"
class Vertex3F;
class Vertex4F;
class Matrix4x4;

/* 三维坐标 BEGIN*/
class Vertex3F
{
public:
    Vertex3F() :x_(0.0f), y_(0.0f), z_(0.0f) {}
    Vertex3F(FLOAT x, FLOAT y, FLOAT z) :x_(x), y_(y), z_(z) { this->zeroFix(); }
    ~Vertex3F() {}
    /* 运算符重载 */
    Vertex3F operator+(const Vertex3F & rhs) const;//向量加法
    Vertex3F & operator+=(const Vertex3F & rhs);//保持语法一致
    Vertex3F operator-() const;//反向量
    Vertex3F operator-(const Vertex3F & rhs) const;//向量减法
    Vertex3F & operator-=(const Vertex3F & rhs);//保持语法一致
    Vertex3F operator*(const FLOAT & rhs) const;//标量乘
    Vertex3F & operator*=(const FLOAT & rhs);//保持语法一致
    Vertex3F operator/(const FLOAT & rhs) const;//标量除
    Vertex3F & operator/=(const FLOAT & rhs);//保持语法一致
    Vertex3F & operator=(const Vertex3F & rhs);//赋值
    BOOL operator==(const Vertex3F & rhs);//相等
    BOOL operator!=(const Vertex3F & rhs);//不相等
    FLOAT operator[](const int & index) const;//获得分量从1开始,用set改值
    /* 非重载运算(不改变对象本身) */
    FLOAT length() const;//向量模
    Vertex3F normalize() const;//归一化
    FLOAT distance(const Vertex3F & rhs) const;//与目标点距离
    FLOAT dotProduct(const Vertex3F & rhs) const;//点乘
    Vertex3F crossProduct(const Vertex3F & rhs) const;//叉乘
    Vertex4F toPoint4F() const;//转成齐次坐标点
    Vertex4F toVector4F() const;//转成齐次向量
    /* 对象本身的修改 */
    Vertex3F & set(FLOAT x, FLOAT y, FLOAT z);

    /* 一些常用点/向量 */
    //TODO 现在还没找到需要写在这里的
private:
    Vertex3F & zeroFix();//把接近0.0f的值改成0.0f，本类的成员函数的返回值全可以直接和0.0f比较
    float x_, y_, z_;
};
/* 三维坐标 END*/

/* 四维齐次坐标 BEGIN*/
class Vertex4F
{
public:
    Vertex4F() {}
    Vertex4F(FLOAT x, FLOAT y, FLOAT z, FLOAT w) :x_(x), y_(y), z_(z), w_(w) { this->zeroFix(); }
    ~Vertex4F() {}
    /* 运算符重载 */
    Vertex4F & operator=(const Vertex4F & rhs);
    FLOAT & operator[](const int & index);//获得分量从1开始(任意值都是一个有意义的点或向量，暂时可以改)
    /* 非重载运算 */
    Vertex4F normalize() const;//乘投影矩阵后把w变成1来缩放(返回新对象)
    Vertex4F product(const Matrix4x4 & rhs) const;//乘矩阵，实现仿射变换
    Vertex3F toVertex3F() const;//3D向量
private:
    Vertex4F & zeroFix();//把接近0.0f的值改成0.0f，本类的成员函数的返回值全可以直接和0.0f比较
    FLOAT x_, y_, z_, w_;
};
/* 四维齐次坐标 END*/

/* 非重载运算的外部调用版本，增加可读性又不使用友元函数 */
/* Vertex3F */
inline FLOAT length(Vertex3F & lhs)
{
    return lhs.length();
}

inline Vertex3F normalize(Vertex3F & lhs)
{
    return lhs.normalize();
}

inline FLOAT distance(const Vertex3F & lhs, const Vertex3F & rhs)
{
    return lhs.distance(rhs);
}

inline FLOAT dotProduct(const Vertex3F & lhs, const Vertex3F & rhs)
{
    return lhs.dotProduct(rhs);
}

inline Vertex3F crossProduct(const Vertex3F & lhs, const Vertex3F & rhs)
{
    return lhs.crossProduct(rhs);
}

inline Vertex4F toPoint4F(const Vertex3F & lhs)
{
    return lhs.toPoint4F();
}

inline Vertex4F toVector4F(const Vertex3F & lhs)
{
    return lhs.toVector4F();
}

/* Vertex4F */
inline Vertex4F normalize(const Vertex4F & lhs)
{
    return lhs.normalize();
}

inline Vertex4F product(const Vertex4F & lhs, const Matrix4x4 & rhs)
{
    return lhs.product(rhs);
}

inline Vertex3F toVertex3F(const Vertex4F & lhs)
{
    return lhs.toVertex3F();
}