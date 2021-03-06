#pragma once
#include "common.h"
class Vertex3F;
class Vertex4F;
class Matrix4x4;

class Matrix4x4
{
public:
    Matrix4x4();
    Matrix4x4(Vertex4F c1, Vertex4F c2, Vertex4F c3, Vertex4F c4);//用列向量构造
    ~Matrix4x4() {}
    /* 运算符重载 */
    Matrix4x4 operator+(const Matrix4x4 & rhs) const;//矩阵加
    Matrix4x4 & operator+=(const Matrix4x4 & rhs);//保持语法一致
    Matrix4x4 operator-() const;//矩阵取反
    Matrix4x4 operator-(const Matrix4x4 & rhs) const;//矩阵减
    Matrix4x4 & operator-=(const Matrix4x4 & rhs);//保持语法一致
    Matrix4x4 operator*(const Matrix4x4 & rhs) const;//矩阵乘
    Matrix4x4 & operator*=(const Matrix4x4 & rhs);//保持语法一致
    Matrix4x4 & operator=(const Matrix4x4 & rhs);//赋值
    BOOL operator==(const Matrix4x4 & rhs) const;//相等
    BOOL operator!=(const Matrix4x4 & rhs) const;//不相等
    FLOAT operator()(const int & row, const int & col) const;//取分量1开始(矩阵只有特定变换矩阵有意义，只能特殊构造，不能直接改某一个值)
    /* 非重载运算 */
    //TODO 行列式等坑爹运算

    /* 一些变换矩阵 */
    /* set开头的方法是将矩阵修改成指定矩阵，add开头的方法是在原有基础上增加指定变换 */
    void setIdentity();//单位矩阵
    void setTranslate(const FLOAT & x, const FLOAT & y, const FLOAT & z);//传向量坐标，朝向量平移
    void addTranslate(const FLOAT & x, const FLOAT & y, const FLOAT & z);//传向量坐标，朝向量平移
    void setTranslate(const Vertex3F & v);//传向量，朝向量平移
    void addTranslate(const Vertex3F & v);//传向量，朝向量平移
    void setRotate(FLOAT x, FLOAT y, FLOAT z, FLOAT angle);//传向量坐标和角度，绕向量逆时针转
    void addRotate(FLOAT x, FLOAT y, FLOAT z, FLOAT angle);//传向量坐标和角度，绕向量逆时针转
    void setRotate(Vertex3F v, FLOAT angle);//传向量和角度，绕向量逆时针转
    void addRotate(Vertex3F v, FLOAT angle);//传向量和角度，绕向量逆时针转
private:
    Matrix4x4 & zeroFix();
    FLOAT m_[16];
};