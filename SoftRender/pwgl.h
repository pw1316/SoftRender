#pragma once
#include "common.h"
#include "Vertex.h"
#include "Matrix.h"

/* Safe release for COM components */
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

    Vertex3F vertexBuffer_[8];
    TriangleIndex indexBuffer_[12];
    /* Transform */
    Vertex3F posInWorld = Vertex3F(0.0f, 0.0f, 0.0f);
    FLOAT rotX = 45.0f;
    FLOAT rotY = 0.0f;
    FLOAT rotZ = 0.0f;
};