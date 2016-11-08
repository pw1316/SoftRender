#pragma once
#include "common.h"
#include "Vertex.h"
#include "Matrix.h"
#include "Camera.h"

/* COM组件的释放 */
template<class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = NULL;
    }
}

typedef struct __TriangleIndex {
    int p0, p1, p2;
} TriangleIndex;

/* 当前hInstance */
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class PWGL
{
public:
    static PWGL * getInstance();
    static void releaseInstance();
    HRESULT initWindow();
    HRESULT initDevice();
    void mainLoop();

private:
    PWGL();
    ~PWGL();
    HRESULT onRender();
    void onResize(UINT width, UINT height);
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    static PWGL *instance_;
    static LPCSTR WINDOW_CLASS_NAME;
    const static INT WINDOW_WIDTH;
    const static INT WINDOW_HEIGHT;

    HWND hWND_ = nullptr;
    HDC hDC_ = nullptr;
    HDC hMemDC_ = nullptr;
    HBITMAP hBITMAP_ = nullptr;
    BITMAPINFO bmpInfo_ = {};
    UINT *bmpBuffer_ = nullptr;

    /* 立方体initDevice里初始化 */
    FLOAT *zBuffer_;
    /* 顶点缓存 */
    Vertex3F vertexBuffer_[8];
    /* 索引缓存，右手逆时针法向量 */
    TriangleIndex indexBuffer_[12];
    /* 世界变换，yzy欧拉角 p世界 = p刚体 * R(gamma) * R(beta) * R(alpha) */
    FLOAT rotAlpha_;
    FLOAT rotBeta_;
    FLOAT rotGamma_;
    /* 相机 */
    Camera camera_;
    /* 投影 */
    FLOAT fovx_;
    FLOAT aspect_;
    FLOAT near_;
    FLOAT far_;
    //暂时不投影，直接投到摄像机坐标系z=0上

    /* 计算FPS */
    LARGE_INTEGER frequency_ = {};
    FLOAT fps_ = 0.0f;
};