#pragma once
#include "common.h"
class Vertex3F;
class Matrix4x4;
class Camera;
#include "Vertex.h"

/* ����� BEGIN */
class Camera
{
public:
    Camera();
    Camera(const Vertex3F & eye, const Vertex3F & at, const Vertex3F & up);
    ~Camera() {}
    /* ����任���� */
    Matrix4x4 matrix();
    /* ������ƶ� */
    void moveForward(FLOAT dist);
    /* ������ */
    Vertex3F getEye() { return eye_; }
private:
    Vertex3F eye_;
    Vertex3F forward_;
    Vertex3F up_;
};
/* ����� END */