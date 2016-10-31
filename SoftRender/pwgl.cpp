#include "pwgl.h"

/* Static Menber */
PWGL *PWGL::instance_ = nullptr;
LPCSTR PWGL::WINDOW_CLASS_NAME = "PWGL";
const UINT PWGL::WINDOW_WIDTH = 800;
const UINT PWGL::WINDOW_HEIGHT = 600;

/* Methods */
PWGL * PWGL::getInstance()
{
    if (PWGL::instance_ == nullptr)
    {
        instance_ = new PWGL();
    }
    return PWGL::instance_;
}

void PWGL::releaseInstance()
{
    if (PWGL::instance_ != nullptr)
    {
        delete PWGL::instance_;
        PWGL::instance_ = nullptr;
    }
}

HRESULT PWGL::initWindow()
{
    HRESULT hr = S_OK;
    if (SUCCEEDED(hr))
    {
        WNDCLASSEX wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = PWGL::wndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = HINST_THISCOMPONENT;
        wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = PWGL::WINDOW_CLASS_NAME;
        wcex.hIconSm = LoadIcon(HINST_THISCOMPONENT, IDI_APPLICATION);
        RegisterClassEx(&wcex);
    }
    hWND_ = CreateWindowEx(
        0,
        PWGL::WINDOW_CLASS_NAME,//Class name
        "SoftRender",//Window name
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        PWGL::WINDOW_WIDTH,
        PWGL::WINDOW_HEIGHT,
        NULL,
        NULL,
        HINST_THISCOMPONENT,
        this
    );
    hr = hWND_ ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        hr = initDevice();
    }
    if (SUCCEEDED(hr))
    {
        ShowWindow(hWND_, SW_SHOWNORMAL);
        UpdateWindow(hWND_);
    }
    return hr;
}

HRESULT PWGL::initDevice()
{
    HRESULT hr = S_OK;
    hDC_ = GetDC(hWND_);
    hMemDC_ = CreateCompatibleDC(hDC_);
    hBITMAP_ = CreateCompatibleBitmap(hDC_, WINDOW_WIDTH, WINDOW_HEIGHT);
    SelectObject(hMemDC_, hBITMAP_);

    return hr;
}

void PWGL::mainLoop()
{
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        onRender();
    }
}

PWGL::PWGL()
{
}

PWGL::~PWGL()
{
}

HRESULT PWGL::onRender()
{
    return E_NOTIMPL;
}

void PWGL::onResize(UINT width, UINT height)
{
}

LRESULT PWGL::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    if (message == WM_CREATE)
    {
        result = 1;
    }
    else
    {
        PWGL *ppwgl = PWGL::getInstance();
        bool wasHandled = false;
        if (ppwgl)
        {
            switch (message)
            {
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                ppwgl->onResize(width, height);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DESTROY:
            {
                PostQuitMessage(0);
            }
            result = 1;
            wasHandled = true;
            break;
            }
        }
        if (!wasHandled)
        {
            result = DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    return result;
}

