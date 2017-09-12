#include "pwgl.h"

#include <cstdio>
#include <omp.h>

#define PI 3.1415926535898f

/* Static Menber */
PWGL* PWGL::instance_ = nullptr;
LPCSTR PWGL::WINDOW_CLASS_NAME = "PWGL";
const INT PWGL::WINDOW_WIDTH = 800;
const INT PWGL::WINDOW_HEIGHT = 600;

/* Methods */
PWGL* PWGL::getInstance()
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
    hTexture_ = (HBITMAP)::LoadImage(HINST_THISCOMPONENT, "Texture.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);
    GetObject(hTexture_, sizeof(BITMAP), &texture_);
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
    vertexU[0] = 1; vertexV[0] = 1;
    vertexU[1] = 0; vertexV[1] = 1;
    vertexU[2] = 1; vertexV[2] = 0;
    vertexU[3] = 0; vertexV[3] = 0;
    vertexU[4] = 0; vertexV[4] = 1;
    vertexU[5] = 1; vertexV[5] = 1;
    vertexU[6] = 0; vertexV[6] = 0;
    vertexU[7] = 1; vertexV[7] = 0;
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
    Vertex3F camEye(0.0f, 0.0f, 2.0f);
    Vertex3F camAt(0.0f, 0.0f, 0.0f);
    Vertex3F camUp(0.0f, 1.0f, 0.0f);
    camera_ = Camera(camEye, camAt, camUp);
    /* 投影 */
    fovx_ = 60.0f;
    aspect_ = 1.0f * WINDOW_WIDTH / WINDOW_HEIGHT;
    near_ = 1.0f;
    far_ = 100.0f;
    /* 光源 */
    lightPos.set(0.0f, 0.0f, 5.0f);//点光源位置(世界坐标)
    lightDiffuse_.set(0.4f, 0.4f, 0.4f);//漫反射光颜色(R, G, B)
    lightSpecular_.set(0.4f, 0.4f, 0.4f);//镜面反射光颜色(R, G, B)
    lightAmbient.set(0.2f, 0.2f, 0.2f);//环境光颜色(R, G, B)
    range_ = 100.0f;//最大光程

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
    rotAlpha_ += rotAlphaV_ * 60.f / fps_;
    rotBeta_ += rotBetaV_ * 60.f / fps_;
    rotGamma_ += rotGammaV_ * 60.f / fps_;
    transform *= camera_.matrix();
    Vertex3F lightInView = lightPos.toPoint4F().product(camera_.matrix()).toVertex3F();
    /* p0, p1, p2 面顶点的观察坐标 */
    /* proj0, proj1, proj2 面顶点的屏幕坐标 */
    /* pointList 裁剪面顶点的屏幕坐标 */
    /* (j, i) 插值点的屏幕坐标 */
#pragma omp parallel for
    for (int face = 0; face < 12; face++)
    {
        /* 局部坐标系 */
        Vertex3F p[3];
        p[0] = vertexBuffer_[this->indexBuffer_[face].p0];
        p[1] = vertexBuffer_[this->indexBuffer_[face].p1];
        p[2] = vertexBuffer_[this->indexBuffer_[face].p2];
        FLOAT pu[3];
        pu[0] = vertexU[this->indexBuffer_[face].p0];
        pu[1] = vertexU[this->indexBuffer_[face].p1];
        pu[2] = vertexU[this->indexBuffer_[face].p2];
        FLOAT pv[3];
        pv[0] = vertexV[this->indexBuffer_[face].p0];
        pv[1] = vertexV[this->indexBuffer_[face].p1];
        pv[2] = vertexV[this->indexBuffer_[face].p2];
        /* 观察坐标系 */
        p[0] = p[0].toPoint4F().product(transform).toVertex3F();
        p[1] = p[1].toPoint4F().product(transform).toVertex3F();
        p[2] = p[2].toPoint4F().product(transform).toVertex3F();
        Vertex3F norm = crossProduct((p[1] - p[0]), (p[2] - p[1]));
        /* 裁剪，投影 */
        Matrix4x4 projection(
            Vertex4F(1.0f * WINDOW_WIDTH / tan(fovx_ / 180.0f * PI / 2.0f) / 2.0f, 0.0f, -1.0f * WINDOW_WIDTH / 2.0f, 0.0f),
            Vertex4F(0.0f, 1.0f * WINDOW_HEIGHT * aspect_ / tan(fovx_ / 180.0f * PI / 2.0f) / 2.0f, -1.0f * WINDOW_HEIGHT / 2.0f, 0.0f),
            Vertex4F(0.0f, 0.0f, far_ / (far_ - near_), near_ * far_ / (far_ - near_)),
            Vertex4F(0.0f, 0.0f, -1.0f, 0.0f)
        );
        INT index = 0;
        Vertex3F pointList[10];
        Vertex3F proj[3];
        proj[0] = p[0].toPoint4F().product(projection).normalize().toVertex3F();
        proj[1] = p[1].toPoint4F().product(projection).normalize().toVertex3F();
        proj[2] = p[2].toPoint4F().product(projection).normalize().toVertex3F();
        Vertex3F projNorm = crossProduct((proj[1] - proj[0]), (proj[2] - proj[1]));
        if (projNorm[3] < 0.0f) continue;
        /* 3条边 */
        for (int edgeIndex = 0; edgeIndex < 3; edgeIndex++)
        {
            int start = edgeIndex;
            int end = (edgeIndex + 1) % 3;
            if (proj[start][3] > 0.0f)
            {
                if (proj[end][3] > 0.0f)
                {
                }
                else if (proj[end][3] < -1.0f)
                {
                    FLOAT t = (proj[start][3] - 0.0f) / (proj[start][3] - proj[end][3]);
                    pointList[index++] = proj[start] + (proj[end] - proj[start]) * t;
                    t = (proj[start][3] + 1.0f) / (proj[start][3] - proj[end][3]);
                    pointList[index++] = proj[start] + (proj[end] - proj[start]) * t;
                }
                else
                {
                    FLOAT t = (proj[start][3] - 0.0f) / (proj[start][3] - proj[end][3]);
                    pointList[index++] = proj[start] + (proj[end] - proj[start]) * t;
                }
            }
            else if (proj[start][3] < -1.0f)
            {
                if (proj[end][3] > 0.0f)
                {
                    FLOAT t = (proj[start][3] + 1.0f) / (proj[start][3] - proj[end][3]);
                    pointList[index++] = proj[start] + (proj[end] - proj[start]) * t;
                    t = (proj[start][3] - 0.0f) / (proj[start][3] - proj[end][3]);
                    pointList[index++] = proj[start] + (proj[end] - proj[start]) * t;
                }
                else if (proj[end][3] < -1.0f)
                {
                }
                else
                {
                    FLOAT t = (proj[start][3] + 1.0f) / (proj[start][3] - proj[end][3]);
                    pointList[index++] = proj[start] + (proj[end] - proj[start]) * t;
                }
            }
            else {
                pointList[index++] = proj[start];
                if (proj[end][3] > 0.0f)
                {
                    FLOAT t = (proj[start][3] - 0.0f) / (proj[start][3] - proj[end][3]);
                    pointList[index++] = proj[start] + (proj[end] - proj[start]) * t;
                }
                else if (proj[end][3] < -1.0f)
                {
                    FLOAT t = (proj[start][3] + 1.0f) / (proj[start][3] - proj[end][3]);
                    pointList[index++] = proj[start] + (proj[end] - proj[start]) * t;
                }
            }
        }
        if (index < 3) continue;
        /* 光栅化 */
        /* 裁剪后的每个三角形 */
        for (int k = 1; k < index - 1; k++)
        {
            FLOAT yMax = max(max(pointList[0][2], pointList[k][2]), pointList[k + 1][2]);
            FLOAT yMin = min(min(pointList[0][2], pointList[k][2]), pointList[k + 1][2]);
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
            for (INT row = iyMax; row >= iyMin; row--)
            {
                FLOAT xMin = static_cast<FLOAT>(WINDOW_WIDTH - 1);
                FLOAT xMax = 0.0f;
                /* 忽略水平线 */
                if (pointList[0][2] != pointList[k][2])
                {
                    if ((pointList[0][2] - row) * (pointList[k][2] - row) < 0.0f)
                    {
                        FLOAT tmpx = (pointList[0][1] - pointList[k][1]) / (pointList[0][2] - pointList[k][2]) * (row - pointList[0][2]) + pointList[0][1];
                        if (tmpx > xMax) xMax = tmpx;
                        if (tmpx < xMin) xMin = tmpx;
                    }
                }
                if (pointList[0][2] != pointList[k + 1][2])
                {
                    if ((pointList[0][2] - row) * (pointList[k + 1][2] - row) < 0.0f)
                    {
                        FLOAT tmpx = (pointList[0][1] - pointList[k + 1][1]) / (pointList[0][2] - pointList[k + 1][2]) * (row - pointList[0][2]) + pointList[0][1];
                        if (tmpx > xMax) xMax = tmpx;
                        if (tmpx < xMin) xMin = tmpx;
                    }
                }
                if (pointList[k + 1][2] != pointList[k][2])
                {
                    if ((pointList[k][2] - row) * (pointList[k + 1][2] - row) < 0.0f)
                    {
                        FLOAT tmpx = (pointList[k + 1][1] - pointList[k][1]) / (pointList[k + 1][2] - pointList[k][2]) * (row - pointList[k + 1][2]) + pointList[k + 1][1];
                        if (tmpx > xMax) xMax = tmpx;
                        if (tmpx < xMin) xMin = tmpx;
                    }
                }
                INT ixMin = static_cast<INT>(xMin);
                INT ixMax = static_cast<INT>(xMax);
                ixMin = max(0, ixMin);
                ixMax = min(WINDOW_WIDTH - 1, ixMax);
                for (INT col = ixMin; col <= ixMax; col++)
                {
                    FLOAT v0 = (col - proj[2][1]) * (proj[1][2] - proj[2][2]) + (row - proj[2][2]) * (proj[2][1] - proj[1][1]);
                    v0 /= (proj[0][1] - proj[2][1]) * (proj[1][2] - proj[2][2]) + (proj[0][2] - proj[2][2]) * (proj[2][1] - proj[1][1]);
                    FLOAT v1 = (col - proj[0][1]) * (proj[2][2] - proj[0][2]) + (row - proj[0][2]) * (proj[0][1] - proj[2][1]);
                    v1 /= (proj[1][1] - proj[0][1]) * (proj[2][2] - proj[0][2]) + (proj[1][2] - proj[0][2]) * (proj[0][1] - proj[2][1]);
                    FLOAT v2 = 1 - v0 - v1;
                    FLOAT depth = 1 / (v0 / p[0][3] + v1 / p[1][3] + v2 / p[2][3]);
                    if (zBuffer_[row * WINDOW_WIDTH + col] > -depth) continue;
                    zBuffer_[row * WINDOW_WIDTH + col] = -depth;
                    /* Z-fix */
                    {
                        v0 = v0 * depth / p[0][3];
                        v1 = v1 * depth / p[1][3];
                        v2 = 1 - v0 - v1;
                    }
                    Vertex3F jiInView = p[0] * v0 + p[1] * v1 + p[2] * v2;
                    FLOAT uInView = pu[0] * v0 + pu[1] * v1 + pu[2] * v2;
                    FLOAT vInView = pv[0] * v0 + pv[1] * v1 + pv[2] * v2;
                    uInView = min(max(uInView, 0), 1);
                    vInView = min(max(vInView, 0), 1);
                    /* 光照 */
                    Vertex3F l = (lightInView - jiInView).normalize();
                    Vertex3F r = norm * 2.0f * l.dotProduct(norm) - l;
                    Vertex3F v = -jiInView.normalize();
                    FLOAT vDiff = max(norm.dotProduct(l), 0.0f);
                    FLOAT vSpec = max(v.dotProduct(r) * v.dotProduct(r) * v.dotProduct(r) * v.dotProduct(r), 0.0f);
                    FLOAT vAmb = 1.0f;
                    BYTE* position = (BYTE*)(texture_.bmBits);
                    position = position + texture_.bmWidthBytes * static_cast<INT>(vInView * (texture_.bmHeight - 1));
                    position = position + texture_.bmBitsPixel * static_cast<INT>(uInView * (texture_.bmWidth - 1)) / 8;
                    UINT blue = *position * (vDiff * lightDiffuse_[1] + vSpec * lightSpecular_[1] + vAmb * lightAmbient[1]);
                    UINT green = *(position + 1) * (vDiff * lightDiffuse_[2] + vSpec * lightSpecular_[2] + vAmb * lightAmbient[2]);
                    UINT red = *(position + 2) * (vDiff * lightDiffuse_[3] + vSpec * lightSpecular_[3] + vAmb * lightAmbient[3]);
                    bmpBuffer_[row * WINDOW_WIDTH + col] = (red << 16) | (green << 8) | blue;
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
                ppwgl->camera_.moveForward(zDelta);
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

