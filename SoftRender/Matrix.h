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
    Matrix4x4 operator+(const Matrix4x4 & rhs) const;//�����
    Matrix4x4 & operator+=(const Matrix4x4 & rhs);
    Matrix4x4 operator-() const;//����ȡ��
    Matrix4x4 operator-(const Matrix4x4 & rhs) const;//�����
    Matrix4x4 & operator-=(const Matrix4x4 & rhs);
    Matrix4x4 operator*(const Matrix4x4 & rhs) const;//�����
    Matrix4x4 & operator*=(const Matrix4x4 & rhs);
    Matrix4x4 & operator=(const Matrix4x4 & rhs);//��ֵ
    BOOL operator==(const Matrix4x4 & rhs);//���
    BOOL operator!=(const Matrix4x4 & rhs);//�����
    FLOAT & operator()(const int & row, const int & col);//ȡ����1��ʼ
    /* Other Operation */

    /* Some Transform Matrix*/
    void setTranslate(FLOAT x, FLOAT y, FLOAT z);
    void setTranslate(Vertex3F v);
    void setRotate(FLOAT x, FLOAT y, FLOAT z, FLOAT angel);
private:
    Matrix4x4 & zeroFix();
    FLOAT m_[16];
};