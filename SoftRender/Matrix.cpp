#include "Matrix.h"

int mid(int low, int value, int high)
{
    Assert(low < high);
    return min(max(low, value), high);
}

Matrix4x4::Matrix4x4()
{
    for (int i = 0; i < 16; i++)
    {
        m_[i] = 0.0f;
    }
}

Matrix4x4 Matrix4x4::operator+(const Matrix4x4 & rhs) const
{
    Matrix4x4 tmp;
    for (int i = 0; i < 16; i++)
    {
        tmp.m_[i] = this->m_[i] + rhs.m_[i];
    }
    return tmp.zeroFix();
}

Matrix4x4 & Matrix4x4::operator+=(const Matrix4x4 & rhs)
{
    for (int i = 0; i < 16; i++)
    {
        this->m_[i] += rhs.m_[i];
    }
    return this->zeroFix();
}

Matrix4x4 Matrix4x4::operator-() const
{
    Matrix4x4 tmp;
    for (int i = 0; i < 16; i++)
    {
        tmp.m_[i] = 0.0f - this->m_[i];
    }
    return tmp.zeroFix();
}

Matrix4x4 Matrix4x4::operator-(const Matrix4x4 & rhs) const
{
    Matrix4x4 tmp;
    for (int i = 0; i < 16; i++)
    {
        tmp.m_[i] = this->m_[i] - rhs.m_[i];
    }
    return tmp.zeroFix();
}

Matrix4x4 & Matrix4x4::operator-=(const Matrix4x4 & rhs)
{
    for (int i = 0; i < 16; i++)
    {
        this->m_[i] -= rhs.m_[i];
    }
    return this->zeroFix();
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 & rhs) const
{
    Matrix4x4 tmp;
    for (int i = 0; i < 16; i++)
    {
        int r = i / 4;
        int c = i % 4;
        tmp.m_[i] = 0.0f;
        for (int j = 0; j < 4; j++)
        {
            tmp.m_[i] += this->m_[r * 4 + j] * rhs.m_[j * 4 + c];
        }
    }
    return tmp.zeroFix();
}

Matrix4x4 & Matrix4x4::operator*=(const Matrix4x4 & rhs)
{
    Matrix4x4 tmp;
    for (int i = 0; i < 16; i++)
    {
        int r = i / 4;
        int c = i % 4;
        tmp.m_[i] = 0.0f;
        for (int j = 0; j < 4; j++)
        {
            tmp.m_[i] += this->m_[r * 4 + j] * rhs.m_[j * 4 + c];
        }
    }
    *this = tmp;
    return this->zeroFix();
}

Matrix4x4 & Matrix4x4::operator=(const Matrix4x4 & rhs)
{
    if (this != &rhs)
    {
        for (int i = 0; i < 16; i++)
        {
            this->m_[i] = rhs.m_[i];
        }
    }
    return this->zeroFix();
}

BOOL Matrix4x4::operator==(const Matrix4x4 & rhs)
{
    for (int i = 0; i < 16; i++)
    {
        FLOAT dx = this->m_[i] - rhs.m_[i];
        if (!isAndSetZero(dx))
        {
            return false;
        }
    }
    return true;
}

BOOL Matrix4x4::operator!=(const Matrix4x4 & rhs)
{
    return !(*this == rhs);
}

FLOAT & Matrix4x4::operator()(const int & row, const int & col)
{
    int r = row - 1;
    int c = col - 1;
    r = mid(0, r, 3);
    c = mid(0, c, 3);
    return m_[r * 4 + c];
}

void Matrix4x4::setTranslate(FLOAT x, FLOAT y, FLOAT z)
{
    this->m_[0] = 1.0f; this->m_[1] = 0.0f; this->m_[2] = 0.0f; this->m_[3] = 0.0f;
    this->m_[4] = 0.0f; this->m_[5] = 1.0f; this->m_[6] = 0.0f; this->m_[7] = 0.0f;
    this->m_[8] = 0.0f; this->m_[9] = 0.0f; this->m_[10] = 1.0f; this->m_[11] = 0.0f;
    this->m_[12] = x; this->m_[13] = y; this->m_[14] = z; this->m_[15] = 1.0f;
    this->zeroFix();
}

void Matrix4x4::setTranslate(Vertex3F v)
{
    this->m_[0] = 1.0f; this->m_[1] = 0.0f; this->m_[2] = 0.0f; this->m_[3] = 0.0f;
    this->m_[4] = 0.0f; this->m_[5] = 1.0f; this->m_[6] = 0.0f; this->m_[7] = 0.0f;
    this->m_[8] = 0.0f; this->m_[9] = 0.0f; this->m_[10] = 1.0f; this->m_[11] = 0.0f;
    this->m_[12] = v[1]; this->m_[13] = v[2]; this->m_[14] = v[3]; this->m_[15] = 1.0f;
    this->zeroFix();
}

void Matrix4x4::setRotate(FLOAT x, FLOAT y, FLOAT z, FLOAT angel)
{
    this->m_[0] = x * x * (1 - cos(angel)) + cos(angel); this->m_[1] = x * y * (1 - cos(angel)) + z * sin(angel); this->m_[2] = x * z * (1 - cos(angel)) - y * sin(angel); this->m_[3] = 0.0f;
    this->m_[4] = y * x * (1 - cos(angel)) - z * sin(angel); this->m_[5] = y * y * (1 - cos(angel)) + cos(angel); this->m_[6] = y * z * (1 - cos(angel)) + x * sin(angel); this->m_[7] = 0.0f;
    this->m_[8] = z * x * (1 - cos(angel)) + y * sin(angel); this->m_[9] = z * y * (1 - cos(angel)) - x * sin(angel); this->m_[10] = z * z * (1 - cos(angel)) + cos(angel); this->m_[11] = 0.0f;
    this->m_[12] = 0.0f; this->m_[13] = 0.0f; this->m_[14] = 0.0f; this->m_[15] = 1.0f;
    //TODO ¹éÒ»»¯
    this->zeroFix();
}

Matrix4x4 & Matrix4x4::zeroFix()
{
    for (int i = 0; i < 16; i++)
    {
        isAndSetZero(this->m_[i]);
    }
    return *this;
}

