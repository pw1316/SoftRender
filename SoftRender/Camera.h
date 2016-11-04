#pragma once
#include "common.h"
class Vertex3F;
class Matrix4x4;
class Camera;
#include "Vertex.h"

/* ÉãÏñ»ú BEGIN */
class Camera
{
public:
    Camera();
    ~Camera() {}
    void setPosition(const FLOAT & x, const FLOAT & y, const FLOAT & z);
    void setPosition(const Vertex3F & v);
    BOOL setLookAt(const FLOAT & x, const FLOAT & y, const FLOAT & z);
    BOOL setLookAt(const Vertex3F & v);
    BOOL setUp(const FLOAT & x, const FLOAT & y, const FLOAT & z);
    BOOL setUp(const Vertex3F & v);
    Matrix4x4 matrix();
private:
    Vertex3F eye_;
    Vertex3F forward_;
    Vertex3F up_;
};
/* ÉãÏñ»ú END */