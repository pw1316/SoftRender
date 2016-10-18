#include "header.h"
#include <cstdio>
#include <string>

DemoApp::DemoApp():
    m_hwnd(NULL),
    m_pDirect2dFactory(NULL),
    m_pRenderTarget(NULL),
    m_pLightSlateGrayBrush(NULL),
    m_pCornflowerBlueBrush(NULL)
{
}

DemoApp::~DemoApp()
{
    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
}

HRESULT DemoApp::Initialize()
{
    HRESULT hr;
    hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr))
    {
        WNDCLASSEX wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = DemoApp::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = HINST_THISCOMPONENT;
        wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = "D2DDemoApp";
        wcex.hIconSm = LoadIcon(HINST_THISCOMPONENT, IDI_APPLICATION);
        RegisterClassEx(&wcex);
    }

    //Get DPI
    FLOAT dpiX, dpiY;
    m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

    // Create the window.
    m_hwnd = CreateWindowEx(
        0,
        "D2DDemoApp",//Class name
        "Direct2D Demo App",//Window name
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        static_cast<UINT>(ceil(800.f * dpiY / 96.f)),//Height
        static_cast<UINT>(ceil(800.f * dpiX / 96.f)),//Width
        NULL,
        NULL,
        HINST_THISCOMPONENT,
        this
    );
    hr = m_hwnd ? S_OK : E_FAIL;
    if (SUCCEEDED(hr))
    {
        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        UpdateWindow(m_hwnd);
    }
    return hr;
}

void DemoApp::RunMessageLoop()
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;
    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
    return hr;
}

HRESULT DemoApp::CreateDeviceResources()
{
    HRESULT hr = S_OK;
    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );
        // Create a Direct2D render target.
        hr = m_pDirect2dFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
        );

        if (SUCCEEDED(hr))
        {
            // Create a gray brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::LightSlateGray),
                &m_pLightSlateGrayBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a blue brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
                &m_pCornflowerBlueBrush
            );
        }
    }
    return hr;
}

void DemoApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
}

HRESULT DemoApp::OnRender()
{
    HRESULT hr = S_OK;
    hr = CreateDeviceResources();
    if (SUCCEEDED(hr))
    {
        m_pRenderTarget->BeginDraw();
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
        D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

        FILE *fp = NULL;
        fopen_s(&fp, "pointData2", "r");
        float Sx = -1, Sy = -1, SxO = 0, SyO = 0;
        float Cx = -1, Cy = -1, CxO = 0, CyO = 0;
        float CRx = -1, CRy = -1, CRxO = 0, CRyO = 0;
        int res = 0;
        int time = 0;
        res = fscanf_s(fp, "%f %f %f %f %f %f", &Sx, &Sy, &Cx, &Cy, &CRx, &CRy);
        while (res > 0)
        {
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(rtSize.width / 4 + SxO * 10, rtSize.height / 4 + SyO * 10),
                D2D1::Point2F(rtSize.width / 4 + Sx * 10, rtSize.height / 4 + Sy * 10),
                m_pCornflowerBlueBrush,
                1.0f
            );
            m_pRenderTarget->DrawLine(
                D2D1::Point2F((time - 1) * 40, rtSize.height / 4 + SxO * 50),
                D2D1::Point2F(time * 40, rtSize.height / 4 + Sx * 50),
                m_pCornflowerBlueBrush,
                1.0f
            );
            m_pRenderTarget->DrawLine(
                D2D1::Point2F((time - 1) * 40, rtSize.height / 4 * 3 + SyO * 50),
                D2D1::Point2F(time * 40, rtSize.height / 4 * 3 + Sy * 50),
                m_pCornflowerBlueBrush,
                1.0f
            );
            SxO = Sx;
            SyO = Sy;
            m_pRenderTarget->DrawLine(
                D2D1::Point2F((time - 1) * 40, rtSize.height / 4 + CRxO * 50),
                D2D1::Point2F(time * 40, rtSize.height / 4 + CRx * 50),
                m_pLightSlateGrayBrush,
                1.0f
            );
            m_pRenderTarget->DrawLine(
                D2D1::Point2F((time - 1) * 40, rtSize.height / 4 * 3 + CRyO * 50),
                D2D1::Point2F(time * 40, rtSize.height / 4 * 3 + CRy * 50),
                m_pLightSlateGrayBrush,
                1.0f
            );
            CRxO = CRx;
            CRyO = CRy;
            time++;
            res = fscanf_s(fp, "%f %f %f %f %f %f\n", &Sx, &Sy, &Cx, &Cy, &CRx, &CRy);
        }
        fclose(fp);
        fp = NULL;
        hr = m_pRenderTarget->EndDraw();
    }
    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardDeviceResources();
    }
    return hr;
}

void DemoApp::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}

LRESULT DemoApp::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    if (message == WM_CREATE)
    {
        //WM_CREATE message's LParam is its creating struct
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        //lpCreateParams is the LParam when CreateWindow is called
        DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;
        //Save the object into window attributes
        ::SetWindowLongPtrW(
            hWnd,
            GWLP_USERDATA,
            PtrToUlong(pDemoApp)
        );
        result = 1;
    }
    else
    {
        DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hWnd,
                GWLP_USERDATA
            )));
        bool wasHandled = false;
        if (pDemoApp)
        {
            switch (message)
            {
            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                pDemoApp->OnResize(width, height);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DISPLAYCHANGE:
            {
                InvalidateRect(hWnd, NULL, FALSE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_PAINT:
            {
                pDemoApp->OnRender();
                ValidateRect(hWnd, NULL);
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
