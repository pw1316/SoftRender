#include "Vertex.h"
#include "Matrix.h"
#include "Camera.h"

Camera::Camera()
{
    this->eye_ = Vertex3F(0.0f, 0.0f, 1.0f);
    this->forward_ = Vertex3F(0.0f, 0.0f, -1.0f);
    this->up_ = Vertex3F(0.0f, 1.0f, 0.0f);
}

void Camera::setPosition(const FLOAT & x, const FLOAT & y, const FLOAT & z)
{
    eye_ = Vertex3F(x, y, z);
}

void Camera::setPosition(const Vertex3F & v)
{
    eye_ = v;
}

BOOL Camera::setLookAt(const FLOAT & x, const FLOAT & y, const FLOAT & z)
{
    Vertex3F tgt(x, y, z);
    tgt = normalize(tgt - eye_);
    if (tgt.length() != 0.0f)
    {
        forward_ = tgt;
        return true;
    }
    return false;
}

BOOL Camera::setLookAt(const Vertex3F & v)
{
    Vertex3F tgt = v;
    tgt = normalize(tgt - eye_);
    if (tgt.length() != 0.0f)
    {
        forward_ = tgt;
        return true;
    }
    return false;
}

BOOL Camera::setUp(const FLOAT & x, const FLOAT & y, const FLOAT & z)
{
    Vertex3F tgt(x, y, z);
    tgt = forward_.crossProduct(tgt).crossProduct(forward_).normalize();
    if (tgt.length() != 0.0f)
    {
        up_ = tgt;
        return true;
    }
    return false;
}

BOOL Camera::setUp(const Vertex3F & v)
{
    Vertex3F tgt = v;
    tgt = forward_.crossProduct(tgt).crossProduct(forward_).normalize();
    if (tgt.length() != 0.0f)
    {
        up_ = tgt;
        return true;
    }
    return false;
}

Matrix4x4 Camera::matrix()
{
    forward_ = forward_.normalize();
    Vertex3F right = forward_.crossProduct(up_).normalize();
    up_ = right.crossProduct(forward_).normalize();
    Matrix4x4 translate, rotate;
    translate.setTranslate(-this->eye_[1], -this->eye_[2], -this->eye_[3]);
    Vertex4F col1(right[1], right[2], right[3], 0.0f);
    Vertex4F col2(up_[1], up_[2], up_[3], 0.0f);
    Vertex4F col3(-forward_[1], -forward_[2], -forward_[3], 0.0f);
    Vertex4F col4(0.0f, 0.0f, 0.0f, 1.0f);
    rotate = Matrix4x4(col1, col2, col3, col4);
    return translate * rotate;
}
