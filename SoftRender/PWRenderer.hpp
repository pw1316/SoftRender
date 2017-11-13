#pragma once
#include "common.h"
#include "Vertex.h"
#include "Matrix.h"
#include "Camera.h"

/* COM������ͷ� */
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

/* ��ǰhInstance */
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

    /* ������initDevice���ʼ�� */
    FLOAT *zBuffer_;
    /* ���㻺�� */
    Vertex3F vertexBuffer_[8];
    FLOAT vertexU[8];
    FLOAT vertexV[8];
    /* �������棬������ʱ�뷨���� */
    TriangleIndex indexBuffer_[12];
    /* ����任��yzyŷ���� p���� = p���� * R(gamma) * R(beta) * R(alpha) */
    FLOAT rotAlpha_;
    FLOAT rotAlphaV_;
    FLOAT rotBeta_;
    FLOAT rotBetaV_;
    FLOAT rotGamma_;
    FLOAT rotGammaV_;
    /* ��� */
    Camera camera_;
    /* ͶӰ */
    FLOAT fovx_;
    FLOAT aspect_;
    FLOAT near_;
    FLOAT far_;
    /* ��Դ */
    Vertex3F lightPos;//���Դλ��(��������)
    Vertex3F lightDiffuse_;//���������ɫ(R, G, B)
    Vertex3F lightSpecular_;//���淴�����ɫ(R, G, B)
    Vertex3F lightAmbient;//��������ɫ(R, G, B)
    FLOAT range_;//�����

    /* ����FPS */
    LARGE_INTEGER frequency_ = {};
    FLOAT fps_ = 60.0f;
};