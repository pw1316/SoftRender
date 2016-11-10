#pragma once
#include "common.h"
class Vertex3F;
class Matrix4x4;
class Camera;
#include "Vertex.h"

/* 摄像机 BEGIN */
class Camera
{
public:
    Camera();
    Camera(const Vertex3F & eye, const Vertex3F & at, const Vertex3F & up);
    ~Camera() {}
    /* 计算变换矩阵 */
    Matrix4x4 matrix();
    /* 摄像机移动 */
    void moveForward(FLOAT dist);
    /* 读变量 */
    Vertex3F getEye() { return eye_; }
private:
    Vertex3F eye_;
    Vertex3F forward_;
    Vertex3F up_;
};
/* 摄像机 END */