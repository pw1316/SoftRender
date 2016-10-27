#include "header.h"
#include <cstdio>
#include <string>

DemoApp::DemoApp()
{
    FILE *fp = NULL;
    FLOAT Sx = 0.f, Sy = 0.f;
    FLOAT Cx = 0.f, Cy = 0.f;
    FLOAT CRx = 0.f, CRy = 0.f;

    fopen_s(&fp, "pointData2", "r");
    while (fscanf_s(fp, "%f %f %f %f %f %f", &Sx, &Sy, &Cx, &Cy, &CRx, &CRy) > 0)
    {
        syncChaseList.push_back(D2D1::Point2F(Cx, Cy));
        syncClientList.push_back(D2D1::Point2F(CRx, CRy));
    }
    fclose(fp);
    fp = NULL;
    scaleWorld = serverList.size() - 1;
    if (scaleWorld < 1)
    {
        scaleWorld = 1;
    }
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
    HRESULT hr = S_OK;
    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

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
        wcex.lpszClassName = WINDOW_CLASS_NAME;
        wcex.hIconSm = LoadIcon(HINST_THISCOMPONENT, IDI_APPLICATION);
        RegisterClassEx(&wcex);
    }

    //Get DPI
    FLOAT dpiX, dpiY;
    m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

    // Create the window.
    m_hwnd = CreateWindowEx(
        0,
        WINDOW_CLASS_NAME,//Class name
        "Sync Result",//Window name
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        static_cast<UINT>(ceil(WINDOW_WIDTH * dpiX / 96.f)),//Width
        static_cast<UINT>(ceil(WINDOW_HEIGHT * dpiY / 96.f)),//Height
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
        timerOn = SetTimer(m_hwnd, 0, 100, NULL);
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
        if (SUCCEEDED(hr))
        {
            // Create a red brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::IndianRed),
                &m_pRedBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            // Create a black brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Black),
                &m_pBlackBrush
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
        const FLOAT LEFT = rtSize.width / 4;
        const FLOAT RIGHT = rtSize.width / 4 * 3;
        const FLOAT UP = rtSize.height / 4;
        const FLOAT DOWN = rtSize.width / 4 * 3;
        const FLOAT AxisX = rtSize.width / 4 / scaleWorld;
        const FLOAT AxisY = rtSize.height / 4 / scaleWorld;
        const FLOAT AxisT = rtSize.width / 2 / scaleWorld;

        D2D1_MATRIX_3X2_F m1 = D2D1::Matrix3x2F::Translation(transform.x, transform.y);
        D2D1_MATRIX_3X2_F m2 = D2D1::Matrix3x2F::Scale(scaleView, scaleView);
        m_pRenderTarget->SetTransform(m1 * m2);
        m_pRenderTarget->DrawLine(
            D2D1::Point2F(rtSize.width / 2, 0),
            D2D1::Point2F(rtSize.width / 2, rtSize.height),
            m_pBlackBrush,
            1.0f
        );
        m_pRenderTarget->DrawLine(
            D2D1::Point2F(0, rtSize.height / 2),
            D2D1::Point2F(rtSize.width, rtSize.height / 2),
            m_pBlackBrush,
            1.0f
        );

        for (int i = 1; i < frame + 1; i++)
        {
            float xx, x, yy, y;
            // Server
            xx = serverList[i - 1].x;
            yy = serverList[i - 1].y;
            x = serverList[i].x;
            y = serverList[i].y;
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(LEFT + xx * AxisX, UP + yy * AxisY),
                D2D1::Point2F(LEFT + x * AxisX, UP + y * AxisY),
                m_pCornflowerBlueBrush,
                LINE_WIDTH
            );

            // Normal
            // Render Position
            xx = normalClientList[i - 1].x;
            yy = normalClientList[i - 1].y;
            x = normalClientList[i].x;
            y = normalClientList[i].y;
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(LEFT + xx * AxisX, UP + yy * AxisY + 4),
                D2D1::Point2F(LEFT + x * AxisX, UP + y * AxisY + 4),
                m_pLightSlateGrayBrush,
                LINE_WIDTH
            );
            // Server Position
            xx = normalChaseList[i - 1].x;
            yy = normalChaseList[i - 1].y;
            x = normalChaseList[i].x;
            y = normalChaseList[i].y;
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(LEFT + xx * AxisX, UP + yy * AxisY + 2),
                D2D1::Point2F(LEFT + x * AxisX, UP + y * AxisY + 2),
                m_pBlackBrush,
                LINE_WIDTH
            );
        }
        for (int i = 1; i < serverList.size(); i++)
        {
            float xx, x, yy, y;
            //Delta
            float ddeltaSC = sqrt(pow(normalChaseList[i - 1].x - serverList[i - 1].x, 2) + pow(normalChaseList[i - 1].y - serverList[i - 1].y, 2));
            float deltaSC = sqrt(pow(normalChaseList[i].x - serverList[i].x, 2) + pow(normalChaseList[i].y - serverList[i].y, 2));
            float ddeltaCR = sqrt(pow(normalChaseList[i - 1].x - normalClientList[i - 1].x, 2) + pow(normalChaseList[i - 1].y - normalClientList[i - 1].y, 2));
            float deltaCR = sqrt(pow(normalChaseList[i].x - normalClientList[i].x, 2) + pow(normalChaseList[i].y - normalClientList[i].y, 2));
            float ddeltaSR = sqrt(pow(normalClientList[i - 1].x - serverList[i - 1].x, 2) + pow(normalClientList[i - 1].y - serverList[i - 1].y, 2));
            float deltaSR = sqrt(pow(normalClientList[i].x - serverList[i].x, 2) + pow(normalClientList[i].y - serverList[i].y, 2));
            xx = i - 1.0f;
            yy = ddeltaSC;
            x = i;
            y = deltaSC;
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(2 * LEFT + xx * AxisT, 2 * UP + yy * AxisY * 10),
                D2D1::Point2F(2 * LEFT + x * AxisT, 2 * UP + y * AxisY * 10),
                m_pCornflowerBlueBrush,
                LINE_WIDTH
            );
            xx = i - 1.0f;
            yy = ddeltaSC + ddeltaCR;
            x = i;
            y = deltaSC + deltaCR;
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(2 * LEFT + xx * AxisT, 2 * UP + yy * AxisY * 10),
                D2D1::Point2F(2 * LEFT + x * AxisT, 2 * UP + y * AxisY * 10),
                m_pBlackBrush,
                LINE_WIDTH
            );
            xx = i - 1.0f;
            yy = ddeltaSR;
            x = i;
            y = deltaSR;
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(2 * LEFT + xx * AxisT, 2 * UP + yy * AxisY * 10),
                D2D1::Point2F(2 * LEFT + x * AxisT, 2 * UP + y * AxisY * 10),
                m_pRedBrush,
                LINE_WIDTH
            );
        }
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

            case WM_TIMER:
            {
                pDemoApp->frame++;
                if (pDemoApp->frame >= pDemoApp->serverList.size())
                {
                    pDemoApp->frame = 0;
                }
                InvalidateRect(hWnd, NULL, FALSE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_LBUTTONDOWN:
            {
                pDemoApp->posX = LOWORD(lParam);
                pDemoApp->posY = HIWORD(lParam);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_RBUTTONDOWN:
            {
                if (pDemoApp->timerOn)
                {
                    KillTimer(pDemoApp->m_hwnd, 0);
                    pDemoApp->timerOn = false;
                }
                else
                {
                    pDemoApp->timerOn = SetTimer(pDemoApp->m_hwnd, 0, 100, NULL);
                }
                InvalidateRect(hWnd, NULL, FALSE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_MOUSEMOVE:
            {
                if (wParam & MK_LBUTTON)
                {
                    INT x = LOWORD(lParam);
                    INT y = HIWORD(lParam);
                    pDemoApp->transform.x += (x - pDemoApp->posX) / pDemoApp->scaleView;
                    pDemoApp->transform.y += (y - pDemoApp->posY) / pDemoApp->scaleView;
                    pDemoApp->posX = x;
                    pDemoApp->posY = y;
                    InvalidateRect(hWnd, NULL, FALSE);
                }
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_MOUSEWHEEL:
            {
                INT data = GET_WHEEL_DELTA_WPARAM(wParam);
                if (data > 0)
                {
                    pDemoApp->scaleView += 1;
                }
                else
                {
                    pDemoApp->scaleView -= 1;
                }
                if (pDemoApp->scaleView < 1)
                {
                    pDemoApp->scaleView = 1;
                }
                InvalidateRect(hWnd, NULL, FALSE);
            }
            result = 0;
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
