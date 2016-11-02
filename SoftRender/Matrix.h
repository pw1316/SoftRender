#pragma once
#include "common.h"
#include "Vertex.h"
#include "Matrix.h"

class Matrix4x4
{
public:
    Matrix4x4();
    ~Matrix4x4() {}
    /* Operator */
    Matrix4x4 operator+(const Matrix4x4 & rhs) const;//矩阵加
    Matrix4x4 & operator+=(const Matrix4x4 & rhs);
    Matrix4x4 operator-() const;//矩阵取反
    Matrix4x4 operator-(const Matrix4x4 & rhs) const;//矩阵减
    Matrix4x4 & operator-=(const Matrix4x4 & rhs);
    Matrix4x4 operator*(const Matrix4x4 & rhs) const;//矩阵乘
    Matrix4x4 & operator*=(const Matrix4x4 & rhs);
    Matrix4x4 & operator=(const Matrix4x4 & rhs);//赋值
    BOOL operator==(const Matrix4x4 & rhs);//相等
    BOOL operator!=(const Matrix4x4 & rhs);//不相等
    FLOAT & operator()(const int & row, const int & col);//取分量1开始
    /* Other Operation */

    /* Some Transform Matrix*/
    void setTranslate(FLOAT x, FLOAT y, FLOAT z);
    void setTranslate(Vertex3F v);
    void setRotate(FLOAT x, FLOAT y, FLOAT z, FLOAT angel);
private:
    Matrix4x4 & zeroFix();
    FLOAT m_[16];
};