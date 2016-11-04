#pragma once
#include "common.h"
class Vertex3F;
class Vertex4F;
class Matrix4x4;

/* ��ά���� BEGIN*/
class Vertex3F
{
public:
    Vertex3F() :x_(0.0f), y_(0.0f), z_(0.0f) {}
    Vertex3F(FLOAT x, FLOAT y, FLOAT z) :x_(x), y_(y), z_(z) { this->zeroFix(); }
    ~Vertex3F() {}
    /* ��������� */
    Vertex3F operator+(const Vertex3F & rhs) const;//�����ӷ�
    Vertex3F & operator+=(const Vertex3F & rhs);//�����﷨һ��
    Vertex3F operator-() const;//������
    Vertex3F operator-(const Vertex3F & rhs) const;//��������
    Vertex3F & operator-=(const Vertex3F & rhs);//�����﷨һ��
    Vertex3F operator*(const FLOAT & rhs) const;//������
    Vertex3F & operator*=(const FLOAT & rhs);//�����﷨һ��
    Vertex3F operator/(const FLOAT & rhs) const;//������
    Vertex3F & operator/=(const FLOAT & rhs);//�����﷨һ��
    Vertex3F & operator=(const Vertex3F & rhs);//��ֵ
    BOOL operator==(const Vertex3F & rhs);//���
    BOOL operator!=(const Vertex3F & rhs);//�����
    FLOAT operator[](const int & index) const;//��÷�����1��ʼ,��set��ֵ
    /* ����������(���ı������) */
    FLOAT length() const;//����ģ
    Vertex3F normalize() const;//��һ��
    FLOAT distance(const Vertex3F & rhs) const;//��Ŀ������
    FLOAT dotProduct(const Vertex3F & rhs) const;//���
    Vertex3F crossProduct(const Vertex3F & rhs) const;//���
    Vertex4F toPoint4F() const;//ת����������
    Vertex4F toVector4F() const;//ת���������
    /* ��������޸� */
    Vertex3F & set(FLOAT x, FLOAT y, FLOAT z);

    /* һЩ���õ�/���� */
    //TODO ���ڻ�û�ҵ���Ҫд�������
private:
    Vertex3F & zeroFix();//�ѽӽ�0.0f��ֵ�ĳ�0.0f������ĳ�Ա�����ķ���ֵȫ����ֱ�Ӻ�0.0f�Ƚ�
    float x_, y_, z_;
};
/* ��ά���� END*/

/* ��ά������� BEGIN*/
class Vertex4F
{
public:
    Vertex4F() {}
    Vertex4F(FLOAT x, FLOAT y, FLOAT z, FLOAT w) :x_(x), y_(y), z_(z), w_(w) { this->zeroFix(); }
    ~Vertex4F() {}
    /* ��������� */
    Vertex4F & operator=(const Vertex4F & rhs);
    FLOAT & operator[](const int & index);//��÷�����1��ʼ(����ֵ����һ��������ĵ����������ʱ���Ը�)
    /* ���������� */
    Vertex4F normalize() const;//��ͶӰ������w���1������(�����¶���)
    Vertex4F product(const Matrix4x4 & rhs) const;//�˾���ʵ�ַ���任
    Vertex3F toVertex3F() const;//3D����
private:
    Vertex4F & zeroFix();//�ѽӽ�0.0f��ֵ�ĳ�0.0f������ĳ�Ա�����ķ���ֵȫ����ֱ�Ӻ�0.0f�Ƚ�
    FLOAT x_, y_, z_, w_;
};
/* ��ά������� END*/

/* ������������ⲿ���ð汾�����ӿɶ����ֲ�ʹ����Ԫ���� */
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