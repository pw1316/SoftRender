#include "Camera.h"

Camera::Camera()
{
    this->eye_ = Vertex3F(0.0f, 0.0f, 1.0f);
    this->forward_ = Vertex3F(0.0f, 0.0f, -1.0f);
    this->up_ = Vertex3F(0.0f, 1.0f, 0.0f);
}

Camera::Camera(const Vertex3F & eye, const Vertex3F & at, const Vertex3F & up)
{
    eye_ = eye;
    forward_ = (at - eye).normalize();
    if (forward_.length() == 0.0f) forward_.set(0.0f, 0.0f, -1.0f);
    up_ = forward_.crossProduct(up).crossProduct(forward_).normalize();
    if (up_.length() == 0.0f) up_.set(0.0f, 1.0f, 0.0f);
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

void Camera::moveForward(FLOAT dist)
{
    eye_ += forward_ * dist;
}

