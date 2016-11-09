#include "pwgl.h"

#include <vector>

#define PI 3.1415926535898f

/* Static Menber */
PWGL *PWGL::instance_ = nullptr;
LPCSTR PWGL::WINDOW_CLASS_NAME = "PWGL";
const INT PWGL::WINDOW_WIDTH = 800;
const INT PWGL::WINDOW_HEIGHT = 600;

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
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
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
#pragma region regionBMPBuffer
    hDC_ = GetDC(hWND_);
    hMemDC_ = CreateCompatibleDC(hDC_);
    hBITMAP_ = CreateCompatibleBitmap(hDC_, WINDOW_WIDTH, WINDOW_HEIGHT);
    SelectObject(hMemDC_, hBITMAP_);
    bmpInfo_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);//结构体的字节数
    bmpInfo_.bmiHeader.biWidth = WINDOW_WIDTH;//以像素为单位的位图宽
    bmpInfo_.bmiHeader.biHeight = -WINDOW_HEIGHT;//以像素为单位的位图高,若为负，表示以左上角为原点，否则以左下角为原点
    bmpInfo_.bmiHeader.biPlanes = 1;//目标设备的平面数，必须设置为1
    bmpInfo_.bmiHeader.biBitCount = 32; //位图中每个像素的位数
    bmpInfo_.bmiHeader.biCompression = BI_RGB;
    bmpInfo_.bmiHeader.biSizeImage = 0;
    bmpInfo_.bmiHeader.biXPelsPerMeter = 0;
    bmpInfo_.bmiHeader.biYPelsPerMeter = 0;
    bmpInfo_.bmiHeader.biClrUsed = 0;
    bmpInfo_.bmiHeader.biClrImportant = 0;
    hBITMAP_ = CreateDIBSection(hMemDC_, &bmpInfo_, DIB_RGB_COLORS, (void**)&bmpBuffer_, NULL, 0);
#pragma endregion
    zBuffer_ = new FLOAT[WINDOW_WIDTH * WINDOW_HEIGHT];
#pragma region regionVertexBuffer
    /* 顶点缓存 */
    vertexBuffer_[0] = Vertex3F(-0.5f, -0.5f, -0.5f);//左下后
    vertexBuffer_[1] = Vertex3F(-0.5f, -0.5f, 0.5f);//左下前
    vertexBuffer_[2] = Vertex3F(-0.5f, 0.5f, -0.5f);//左上后
    vertexBuffer_[3] = Vertex3F(-0.5f, 0.5f, 0.5f);//左上前
    vertexBuffer_[4] = Vertex3F(0.5f, -0.5f, -0.5f);//右下后
    vertexBuffer_[5] = Vertex3F(0.5f, -0.5f, 0.5f);//右下前
    vertexBuffer_[6] = Vertex3F(0.5f, 0.5f, -0.5f);//右上后
    vertexBuffer_[7] = Vertex3F(0.5f, 0.5f, 0.5f);//右上前
#pragma endregion
#pragma region regionIndexBuffer
    /* 索引缓存 */
    indexBuffer_[0] = { 1, 5, 7 };
    indexBuffer_[1] = { 1, 7, 3 };
    indexBuffer_[2] = { 5, 4, 6 };
    indexBuffer_[3] = { 5, 6, 7 };
    indexBuffer_[4] = { 4, 0, 2 };
    indexBuffer_[5] = { 4, 2, 6 };
    indexBuffer_[6] = { 0, 1, 3 };
    indexBuffer_[7] = { 0, 3, 2 };
    indexBuffer_[8] = { 3, 7, 6 };
    indexBuffer_[9] = { 3, 6, 2 };
    indexBuffer_[10] = { 0, 4, 5 };
    indexBuffer_[11] = { 0, 5, 1 };
#pragma endregion
    /* 世界坐标变换 */
    rotAlpha_ = 0.0f;
    rotAlphaV_ = 0.0f;
    rotBeta_ = 0.0f;
    rotBetaV_ = 0.0f;
    rotGamma_ = 0.0f;
    rotGammaV_ = 0.0f;
    /* 摄像机 */
    camEye_ = Vertex3F(5.0f, 0.0f, 5.0f);
    camAt_ = Vertex3F(0.0f, 0.0f, 0.0f);
    camUp_ = Vertex3F(0.0f, 1.0f, 0.0f);
    /* 投影 */
    fovx_ = 60.0f;
    aspect_ = 1.0f * WINDOW_WIDTH / WINDOW_HEIGHT;
    near_ = 1.0f;
    far_ = 10.0f;

    /* 计算FPS */
    QueryPerformanceFrequency(&frequency_);
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
    LARGE_INTEGER startTick = {};
    LARGE_INTEGER endTick = {};
    QueryPerformanceCounter(&startTick);
    /* Reset*/
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
    {
        bmpBuffer_[i] = 0x00D7C4BB;
        zBuffer_[i] = 0.0f;
    }

    /*TODO Render*/
    Matrix4x4 transform;
    transform.setRotate(0.0f, 1.0f, 0.0f, rotGamma_ / 180.0f * PI);
    transform.addRotate(0.0f, 0.0f, 1.0f, rotBeta_ / 180.0f * PI);
    transform.addRotate(0.0f, 1.0f, 0.0f, rotAlpha_ / 180.0f * PI);
    rotAlpha_ += rotAlphaV_;
    rotBeta_ += rotBetaV_;
    rotGamma_ += rotGammaV_;
    camera_.setPosition(camEye_);
    camera_.setLookAt(camAt_);
    camera_.setUp(camUp_);
    transform *= camera_.matrix();
    for (int i = 0; i < 12; i++)
    {
        /* 局部坐标系 */
        Vertex3F p0 = vertexBuffer_[this->indexBuffer_[i].p0];
        Vertex3F p1 = vertexBuffer_[this->indexBuffer_[i].p1];
        Vertex3F p2 = vertexBuffer_[this->indexBuffer_[i].p2];
        /* 观察坐标系 */
        p0 = p0.toPoint4F().product(transform).toVertex3F();
        p1 = p1.toPoint4F().product(transform).toVertex3F();
        p2 = p2.toPoint4F().product(transform).toVertex3F();
        /* 表面法向量 */
        Vertex3F norm = crossProduct((p1 - p0), (p2 - p1));
        /* 删除背面 */
        if (norm[3] <= 0) continue;
        /* 光照 */
        //TODO
        /* 裁剪，投影 */
        Matrix4x4 projection(
            Vertex4F(1.0f * WINDOW_WIDTH / tan(fovx_ / 180.0f * PI / 2.0f) / 2.0f, 0.0f, -1.0f * WINDOW_WIDTH / 2.0f, 0.0f),
            Vertex4F(0.0f, 1.0f * WINDOW_HEIGHT * aspect_ / tan(fovx_ / 180.0f * PI / 2.0f) / 2.0f, -1.0f * WINDOW_HEIGHT / 2.0f, 0.0f),
            Vertex4F(0.0f, 0.0f, far_ / (far_ - near_), near_ * far_ / (far_ - near_)),
            Vertex4F(0.0f, 0.0f, -1.0f, 0.0f)
        );
        std::vector<Vertex3F> pointList;
        std::vector<Vertex3F>::iterator itPointList;
        Vertex3F proj0 = p0.toPoint4F().product(projection).normalize().toVertex3F();
        Vertex3F proj1 = p1.toPoint4F().product(projection).normalize().toVertex3F();
        Vertex3F proj2 = p2.toPoint4F().product(projection).normalize().toVertex3F();
        pointList.push_back(proj0);
        pointList.push_back(proj1);
        pointList.push_back(proj2);
        pointList.push_back(proj0);
        itPointList = pointList.begin();
        while (itPointList != pointList.end())
        {
            if (itPointList + 1 == pointList.end())
            {
                itPointList = pointList.erase(itPointList);
                break;
            }
            Vertex3F startP = *itPointList;
            Vertex3F endP = *(itPointList + 1);
            if (startP[3] > 0.0f)
            {
                itPointList = pointList.erase(itPointList);
                if (endP[3] > 0.0f)
                {
                }
                else if(endP[3] < -1.0f)
                {
                    FLOAT t = (startP[3] + 1.0f) / (startP[3] - endP[3]);
                    itPointList = pointList.insert(itPointList, startP + (endP - startP) * t);
                    t = (startP[3] - 0.0f) / (startP[3] - endP[3]);
                    itPointList = pointList.insert(itPointList, startP + (endP - startP) * t);
                    itPointList += 2;
                }
                else
                {
                    FLOAT t = (startP[3] - 0.0f) / (startP[3] - endP[3]);
                    itPointList = pointList.insert(itPointList, startP + (endP - startP) * t);
                    itPointList++;
                }
            }
            else if (startP[3] < -1.0f)
            {
                itPointList = pointList.erase(itPointList);
                if (endP[3] > 0.0f)
                {
                    FLOAT t = (startP[3] - 0.0f) / (startP[3] - endP[3]);
                    itPointList = pointList.insert(itPointList, startP + (endP - startP) * t);
                    t = (startP[3] + 1.0f) / (startP[3] - endP[3]);
                    itPointList = pointList.insert(itPointList, startP + (endP - startP) * t);
                    itPointList += 2;
                }
                else if (endP[3] < -1.0f)
                {
                }
                else
                {
                    FLOAT t = (startP[3] + 1.0f) / (startP[3] - endP[3]);
                    itPointList = pointList.insert(itPointList, startP + (endP - startP) * t);
                    itPointList++;
                }
            }
            else {
                itPointList++;
                if (endP[3] > 0.0f)
                {
                    FLOAT t = (startP[3] - 0.0f) / (startP[3] - endP[3]);
                    itPointList = pointList.insert(itPointList, startP + (endP - startP) * t);
                    itPointList++;
                }
                else if (endP[3] < -1.0f)
                {
                    FLOAT t = (startP[3] + 1.0f) / (startP[3] - endP[3]);
                    itPointList = pointList.insert(itPointList, startP + (endP - startP) * t);
                    itPointList++;
                }
            }
        }
        if (pointList.size() < 3) continue;
        //TODO
        /* 光栅化 */
        /* 裁剪后的每个三角形 */
        for (int i = 1; i < pointList.size() - 1; i++)
        {
            Vertex3F tmp0 = pointList[0];
            Vertex3F tmp1 = pointList[i];
            Vertex3F tmp2 = pointList[i + 1];
            FLOAT yMax = max(max(tmp0[2], tmp1[2]), tmp2[2]);
            FLOAT yMin = min(min(tmp0[2], tmp1[2]), tmp2[2]);
            INT iyMax = static_cast<INT>(yMax);
            INT iyMin = static_cast<INT>(yMin);
            FLOAT delta = yMin - static_cast<FLOAT>(iyMin);
            if (!isAndSetZero(delta))
            {
                iyMin++;
            }
            iyMin = max(0, iyMin);
            iyMax = min(WINDOW_HEIGHT - 1, iyMax);
            /* 扫描每一行 */
            for (INT i = iyMax; i >= iyMin; i--)
            {
                FLOAT xMin = static_cast<FLOAT>(WINDOW_WIDTH - 1);
                FLOAT xMax = 0.0f;
                /* 忽略水平线 */
                if (tmp0[2] != tmp1[2])
                {
                    if ((tmp0[2] - i) * (tmp1[2] - i) < 0.0f)
                    {
                        FLOAT tmpx = (tmp0[1] - tmp1[1]) / (tmp0[2] - tmp1[2]) * (i - tmp0[2]) + tmp0[1];
                        if (tmpx > xMax) xMax = tmpx;
                        if (tmpx < xMin) xMin = tmpx;
                    }
                }
                if (tmp0[2] != tmp2[2])
                {
                    if ((tmp0[2] - i) * (tmp2[2] - i) < 0.0f)
                    {
                        FLOAT tmpx = (tmp0[1] - tmp2[1]) / (tmp0[2] - tmp2[2]) * (i - tmp0[2]) + tmp0[1];
                        if (tmpx > xMax) xMax = tmpx;
                        if (tmpx < xMin) xMin = tmpx;
                    }
                }
                if (tmp2[2] != tmp1[2])
                {
                    if ((tmp1[2] - i) * (tmp2[2] - i) < 0.0f)
                    {
                        FLOAT tmpx = (tmp2[1] - tmp1[1]) / (tmp2[2] - tmp1[2]) * (i - tmp2[2]) + tmp2[1];
                        if (tmpx > xMax) xMax = tmpx;
                        if (tmpx < xMin) xMin = tmpx;
                    }
                }
                INT ixMin = static_cast<INT>(xMin);
                INT ixMax = static_cast<INT>(xMax);
                ixMin = max(0, ixMin);
                ixMax = min(WINDOW_WIDTH - 1, ixMax);
                for (INT j = ixMin; j <= ixMax; j++)
                {
                    Vertex3F lerp0(tmp0[1], tmp0[2], tmp0[3]);
                    Vertex3F lerp1(tmp1[1], tmp1[2], tmp1[3]);
                    Vertex3F lerp2(tmp2[1], tmp2[2], tmp2[3]);
                    Vertex3F lerpNorm = (lerp1 - lerp0).crossProduct(lerp2 - lerp1);
                    FLOAT depth = lerp0[3] - ((j - lerp0[1]) * lerpNorm[1] + (i - lerp0[2]) * lerpNorm[2]) / lerpNorm[3];
                    if (zBuffer_[i * WINDOW_WIDTH + j] > -depth) continue;
                    bmpBuffer_[i * WINDOW_WIDTH + j] = 0x00FF0000;
                }
            }
        }
        //TODO
    }

    /* Copy buffer */
    CHAR fpsBuffer[256];
    sprintf_s(fpsBuffer, 256, "FPS: %.1f", fps_);
    SetBkColor(hMemDC_, RGB(0xD7, 0xC4, 0xBB));
    SetTextColor(hMemDC_, RGB(0xD0, 0x10, 0x4C));
    TextOut(hMemDC_, 0, 0, fpsBuffer, lstrlen(fpsBuffer));
    SelectObject(hMemDC_, hBITMAP_);
    BitBlt(hDC_, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC_, 0, 0, SRCCOPY);
    
    /* 计算FPS */
    QueryPerformanceCounter(&endTick);
    LARGE_INTEGER delta;
    delta.QuadPart = endTick.QuadPart - startTick.QuadPart;
    fps_ = 1.0f * frequency_.QuadPart / delta.QuadPart;
    return S_OK;
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

            case WM_MOUSEWHEEL:
            {
                LONG zDelta = GET_WHEEL_DELTA_WPARAM(wParam) / 120;
                ppwgl->camEye_ = Vertex3F(ppwgl->camEye_[1], ppwgl->camEye_[2], ppwgl->camEye_[3] + zDelta);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_KEYDOWN:
            {
                switch (wParam)
                {
                case VK_LEFT:
                    ppwgl->rotAlphaV_ = -1.0f;
                    break;
                case VK_RIGHT:
                    ppwgl->rotAlphaV_ = 1.0f;
                    break;
                case VK_UP:
                    ppwgl->rotBetaV_ = -1.0f;
                    break;
                case VK_DOWN:
                    ppwgl->rotBetaV_ = 1.0f;
                    break;
                }
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_KEYUP:
            {
                switch (wParam)
                {
                case VK_LEFT:
                case VK_RIGHT:
                    ppwgl->rotAlphaV_ = 0.0f;
                    break;
                case VK_UP:
                case VK_DOWN:
                    ppwgl->rotBetaV_ = 0.0f;
                    break;
                }
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

