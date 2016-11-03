#pragma once
#include "common.h"

class Matrix4x4
{
public:
    Matrix4x4();
    ~Matrix4x4() {}
    /* ��������� */
    Matrix4x4 operator+(const Matrix4x4 & rhs) const;//�����
    Matrix4x4 & operator+=(const Matrix4x4 & rhs);//�����﷨һ��
    Matrix4x4 operator-() const;//����ȡ��
    Matrix4x4 operator-(const Matrix4x4 & rhs) const;//�����
    Matrix4x4 & operator-=(const Matrix4x4 & rhs);//�����﷨һ��
    Matrix4x4 operator*(const Matrix4x4 & rhs) const;//�����
    Matrix4x4 & operator*=(const Matrix4x4 & rhs);//�����﷨һ��
    Matrix4x4 & operator=(const Matrix4x4 & rhs);//��ֵ
    BOOL operator==(const Matrix4x4 & rhs) const;//���
    BOOL operator!=(const Matrix4x4 & rhs) const;//�����
    FLOAT operator()(const int & row, const int & col) const;//ȡ����1��ʼ(����ֻ���ض��任���������壬ֻ�����⹹�죬����ֱ�Ӹ�ĳһ��ֵ)
    /* ���������� */
    //TODO ����ʽ�ȿӵ�����

    /* һЩ�任���� */
    /* set��ͷ�ķ����ǽ������޸ĳ�ָ�����󣬷�set��ͷ�ķ�������ԭ�л���������ָ���任 */
    void setIdentity();//��λ����
    void setTranslate(FLOAT x, FLOAT y, FLOAT z);//���������꣬������ƽ��
    void setTranslate(Vertex3F v);//��������������ƽ��
    void setRotate(FLOAT x, FLOAT y, FLOAT z, FLOAT angel);//����������ͽǶȣ���������ʱ��ת
    void setRotate(Vertex3F v, FLOAT angel);//�������ͽǶȣ���������ʱ��ת
private:
    Matrix4x4 & zeroFix();
    FLOAT m_[16];
};