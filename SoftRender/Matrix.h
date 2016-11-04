#pragma once
#include "common.h"
class Vertex3F;
class Vertex4F;
class Matrix4x4;

class Matrix4x4
{
public:
    Matrix4x4();
    Matrix4x4(Vertex4F c1, Vertex4F c2, Vertex4F c3, Vertex4F c4);//������������
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
    /* set��ͷ�ķ����ǽ������޸ĳ�ָ������add��ͷ�ķ�������ԭ�л���������ָ���任 */
    void setIdentity();//��λ����
    void setTranslate(const FLOAT & x, const FLOAT & y, const FLOAT & z);//���������꣬������ƽ��
    void addTranslate(const FLOAT & x, const FLOAT & y, const FLOAT & z);//���������꣬������ƽ��
    void setTranslate(const Vertex3F & v);//��������������ƽ��
    void addTranslate(const Vertex3F & v);//��������������ƽ��
    void setRotate(FLOAT x, FLOAT y, FLOAT z, FLOAT angle);//����������ͽǶȣ���������ʱ��ת
    void addRotate(FLOAT x, FLOAT y, FLOAT z, FLOAT angle);//����������ͽǶȣ���������ʱ��ת
    void setRotate(Vertex3F v, FLOAT angle);//�������ͽǶȣ���������ʱ��ת
    void addRotate(Vertex3F v, FLOAT angle);//�������ͽǶȣ���������ʱ��ת
private:
    Matrix4x4 & zeroFix();
    FLOAT m_[16];
};