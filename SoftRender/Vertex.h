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
    Vertex3F operator+(const Vertex3F & rhs) const;//�����ӷ�
    Vertex3F & operator+=(const Vertex3F & rhs);
    Vertex3F operator-() const;//������
    Vertex3F operator-(const Vertex3F & rhs) const;//��������
    Vertex3F & operator-=(const Vertex3F & rhs);
    Vertex3F operator*(const FLOAT & rhs) const;//������
    Vertex3F & operator*=(const FLOAT & rhs);
    Vertex3F operator/(const FLOAT & rhs) const;//������
    Vertex3F & operator/=(const FLOAT & rhs);
    Vertex3F & operator=(const Vertex3F & rhs);//��ֵ
    BOOL operator==(const Vertex3F & rhs);//���
    BOOL operator!=(const Vertex3F & rhs);//�����
    /* Other operation*/
    FLOAT length() const;//����ģ
    Vertex3F normalize() const;//��һ��
    FLOAT dotProduct(const Vertex3F & rhs) const;//���
    Vertex3F crossProduct(const Vertex3F & rhs) const;//���
    Vertex4F toPoint4F() const;//��������
    Vertex4F toVector4F() const;//�������
private:
    Vertex3F & zeroFix();//�ѽӽ�0.0f��ֵ�ĳ�0.0f������ĳ�Ա�����ķ���ֵȫ����ֱ�Ӻ�0.0f�Ƚ�
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
    Vertex4F normalize() const;//��ͶӰ������w���1������
    Vertex3F toVertex3F() const;//3D����
private:
    Vertex4F & zeroFix();//�ѽӽ�0.0f��ֵ�ĳ�0.0f������ĳ�Ա�����ķ���ֵȫ����ֱ�Ӻ�0.0f�Ƚ�
    FLOAT x_, y_, z_, w_;
};
/* 4D Point/Vector class END*/
