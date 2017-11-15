#pragma once
#include "stdafx.h"
#include "Camera.hpp"

/* Release for COM Components */
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

/* Current hInstance */
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
    HBITMAP hTexture_ = nullptr;
    BITMAP texture_ = {};

    /* Z-Buffer */
    PWdouble *zBuffer_;
    /* VertexBuffer, UV */
    Math::Vector3d vertexBuffer_[8];
    PWdouble vertexU[8];
    PWdouble vertexV[8];
    /* IndexBuffer,Right hand CCW as normal */
    TriangleIndex indexBuffer_[12];
    /* yzy Euler angle: p_world = R(alpha) * R(beta) * R(gamma) * p_model */
    PWdouble rotAlpha_;
    PWdouble rotAlphaV_;
    PWdouble rotBeta_;
    PWdouble rotBetaV_;
    PWdouble rotGamma_;
    PWdouble rotGammaV_;
    /* World-View Matrix */
    Math::Matrix44d camera_;
    /* Projection parameters */
    PWdouble fovx_;
    PWdouble aspect_;
    PWdouble near_;
    PWdouble far_;
    /* Light parameters */
    Math::Vector3d lightPos;//Position in world
    Math::Vector3d lightDiffuse_;//Diffuse color (R, G, B)
    Math::Vector3d lightSpecular_;//Specular color (R, G, B)
    Math::Vector3d lightAmbient;//Ambient color (R, G, B)
    PWdouble range_;// Light range

    /* FPS counter */
    LARGE_INTEGER frequency_ = {};
    FLOAT fps_ = 60.0f;
};