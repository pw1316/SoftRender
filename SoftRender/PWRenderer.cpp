#include "PWRenderer.hpp"

#include <cstdio>
#include <omp.h>

#define PI 3.1415926535898

/* Static Menber */
PWGL* PWGL::instance_ = nullptr;
LPCSTR PWGL::WINDOW_CLASS_NAME = "PWGL";
const PWint PWGL::WINDOW_WIDTH = 800;
const PWint PWGL::WINDOW_HEIGHT = 600;

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
    zBuffer_ = new PWdouble[WINDOW_WIDTH * WINDOW_HEIGHT];
#pragma region regionVertexBuffer
    /* Vertex Buffer */
    vertexBuffer_[0] = Math::Vector3d(-0.5f, -0.5f, -0.5f);//LeftBottomBehind
    vertexBuffer_[1] = Math::Vector3d(-0.5f, -0.5f, 0.5f);//LBF
    vertexBuffer_[2] = Math::Vector3d(-0.5f, 0.5f, -0.5f);//LTB
    vertexBuffer_[3] = Math::Vector3d(-0.5f, 0.5f, 0.5f);//LTF
    vertexBuffer_[4] = Math::Vector3d(0.5f, -0.5f, -0.5f);//RBB
    vertexBuffer_[5] = Math::Vector3d(0.5f, -0.5f, 0.5f);//RBF
    vertexBuffer_[6] = Math::Vector3d(0.5f, 0.5f, -0.5f);//RTB
    vertexBuffer_[7] = Math::Vector3d(0.5f, 0.5f, 0.5f);//RTF
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
    /* Index Buffer */
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
    /* Model-World parameters */
    rotAlpha_ = 0.0f;
    rotAlphaV_ = 0.0f;
    rotBeta_ = 0.0f;
    rotBetaV_ = 0.0f;
    rotGamma_ = 0.0f;
    rotGammaV_ = 0.0f;
    /* World-View matrix */
    camera_ = Camera::lookAt(0, 0, 2, 0, 0, 0, 0, 1, 0);
    /* Projection parameters */
    fovx_ = 60.0f;
    aspect_ = 1.0f * WINDOW_WIDTH / WINDOW_HEIGHT;
    near_ = 1.0f;
    far_ = 100.0f;
    /* Light */
    lightPos.set(0.0f, 0.0f, 5.0f);
    lightDiffuse_.set(0.4f, 0.4f, 0.4f);
    lightSpecular_.set(0.4f, 0.4f, 0.4f);
    lightAmbient.set(0.2f, 0.2f, 0.2f);
    range_ = 100.0f;

    /* FPS counter */
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
        zBuffer_[i] = -1;
    }

    Math::Matrix44d transform;
    transform.setRotate(0.0f, 1.0f, 0.0f, rotGamma_ / 180.0f * PI);
    transform.addRotate(0.0f, 0.0f, 1.0f, rotBeta_ / 180.0f * PI);
    transform.addRotate(0.0f, 1.0f, 0.0f, rotAlpha_ / 180.0f * PI);
    rotAlpha_ += rotAlphaV_ * 60.f / fps_;
    rotBeta_ += rotBetaV_ * 60.f / fps_;
    rotGamma_ += rotGammaV_ * 60.f / fps_;
    transform = camera_ * transform;
    camera_ * lightPos.toVector4d1();
    Math::Vector3d lightInView = (camera_ * lightPos.toVector4d1()).toVector3d();
    /* p0, p1, p2: Face's vertices in view */
    /* proj0, proj1, proj2: Face's vertices in projection */
    /* screen0, screen1, screen2: Face's vertices in projection with w normalized */
    /* pointList: Clipped triangle list */
    /* (j, i) Point on screen */
    for (int face = 0; face < 4; face++)
    {
        /* Model */
        Math::Vector3d p[3];
        p[0] = vertexBuffer_[this->indexBuffer_[face].p0];
        p[1] = vertexBuffer_[this->indexBuffer_[face].p1];
        p[2] = vertexBuffer_[this->indexBuffer_[face].p2];
        PWdouble pu[3];
        pu[0] = vertexU[this->indexBuffer_[face].p0];
        pu[1] = vertexU[this->indexBuffer_[face].p1];
        pu[2] = vertexU[this->indexBuffer_[face].p2];
        PWdouble pv[3];
        pv[0] = vertexV[this->indexBuffer_[face].p0];
        pv[1] = vertexV[this->indexBuffer_[face].p1];
        pv[2] = vertexV[this->indexBuffer_[face].p2];
        /* View */
        p[0] = (transform * p[0].toVector4d1()).toVector3d();
        p[1] = (transform * p[1].toVector4d1()).toVector3d();
        p[2] = (transform * p[2].toVector4d1()).toVector3d();
        Math::Vector3d norm = Math::cross(p[1] - p[0], p[2] - p[1]);
        /* Clip，Projection */
        Math::Matrix44d projection(WINDOW_WIDTH / std::tan(fovx_ / 180 * PI / 2) / 2.0, 0, -WINDOW_WIDTH / 2.0, 0,
            0, WINDOW_HEIGHT * aspect_ / tan(fovx_ / 180 * PI / 2) / 2.0, -WINDOW_HEIGHT / 2.0, 0,
            0, 0, far_ / (far_ - near_), near_ * far_ / (far_ - near_),
            0, 0, -1, 0);
        PWint index = 0;
        Math::Vector3d pointList[10];
        Math::Vector4d proj[3];
        Math::Vector3d screen[3];
        proj[0] = projection * p[0].toVector4d1();
        proj[1] = projection * p[1].toVector4d1();
        proj[2] = projection * p[2].toVector4d1();
        screen[0] = proj[0].normal().toVector3d();
        screen[1] = proj[1].normal().toVector3d();
        screen[2] = proj[2].normal().toVector3d();
        /* Behind Camera */
        if (proj[0].getZ() > 0 || proj[1].getZ() > 0 || proj[2].getZ() > 0) continue;
        /* Backface */
        Math::Vector3d normProj = Math::cross(screen[1] - screen[0], screen[2] - screen[1]);
        if (normProj.getZ() < 0) continue;
        /* Face to be clipped */
        for (int edgeIndex = 0; edgeIndex < 3; edgeIndex++)
        {
            int start = edgeIndex;
            int end = (edgeIndex + 1) % 3;
            if (screen[start].getZ() > 0.0f)
            {
                if (screen[end].getZ() > 0.0f)
                {
                }
                else if (screen[end].getZ() < -1.0f)
                {
                    PWdouble t = (screen[start].getZ() - 0.0f) / (screen[start].getZ() - screen[end].getZ());
                    pointList[index++] = screen[start] + (screen[end] - screen[start]) * t;
                    t = (screen[start].getZ() + 1.0f) / (screen[start].getZ() - screen[end].getZ());
                    pointList[index++] = screen[start] + (screen[end] - screen[start]) * t;
                }
                else
                {
                    PWdouble t = (screen[start].getZ() - 0.0f) / (screen[start].getZ() - screen[end].getZ());
                    pointList[index++] = screen[start] + (screen[end] - screen[start]) * t;
                }
            }
            else if (screen[start].getZ() < -1.0f)
            {
                if (screen[end].getZ() > 0.0f)
                {
                    PWdouble t = (screen[start].getZ() + 1.0f) / (screen[start].getZ() - screen[end].getZ());
                    pointList[index++] = screen[start] + (screen[end] - screen[start]) * t;
                    t = (screen[start].getZ() - 0.0f) / (screen[start].getZ() - screen[end].getZ());
                    pointList[index++] = screen[start] + (screen[end] - screen[start]) * t;
                }
                else if (screen[end].getZ() < -1.0f)
                {
                }
                else
                {
                    PWdouble t = (screen[start].getZ() + 1.0f) / (screen[start].getZ() - screen[end].getZ());
                    pointList[index++] = screen[start] + (screen[end] - screen[start]) * t;
                }
            }
            else {
                pointList[index++] = screen[start];
                if (screen[end].getZ() > 0.0f)
                {
                    PWdouble t = (screen[start].getZ() - 0.0f) / (screen[start].getZ() - screen[end].getZ());
                    pointList[index++] = screen[start] + (screen[end] - screen[start]) * t;
                }
                else if (screen[end].getZ() < -1.0f)
                {
                    PWdouble t = (screen[start].getZ() + 1.0f) / (screen[start].getZ() - screen[end].getZ());
                    pointList[index++] = screen[start] + (screen[end] - screen[start]) * t;
                }
            }
        }
        if (index < 3) continue;
        /* Rasterize */
        for (int k = 1; k < index - 1; k++)
        {
            PWdouble yMax = max(max(pointList[0].getY(), pointList[k].getY()), pointList[k + 1].getY());
            PWdouble yMin = min(min(pointList[0].getY(), pointList[k].getY()), pointList[k + 1].getY());
            PWint iyMax = static_cast<PWint>(yMax);
            PWint iyMin = static_cast<PWint>(yMin);
            PWdouble delta = yMin - static_cast<PWdouble>(iyMin);
            if (!Math::equal(delta, 0))
            {
                iyMin++;
            }
            iyMin = max(0, iyMin);
            iyMax = min(WINDOW_HEIGHT - 1, iyMax);
            /* Each row */
            for (PWint row = iyMax; row >= iyMin; row--)
            {
                PWdouble xMin = static_cast<PWdouble>(WINDOW_WIDTH - 1);
                PWdouble xMax = 0.0f;
                /* Ignore horizontal line */
                if (pointList[0].getY() != pointList[k].getY())
                {
                    if ((pointList[0].getY() - row) * (pointList[k].getY() - row) < 0.0f)
                    {
                        PWdouble tmpx = (pointList[0].getX() - pointList[k].getX()) / (pointList[0].getY() - pointList[k].getY()) * (row - pointList[0].getY()) + pointList[0].getX();
                        if (tmpx > xMax) xMax = tmpx;
                        if (tmpx < xMin) xMin = tmpx;
                    }
                }
                if (pointList[0].getY() != pointList[k + 1].getY())
                {
                    if ((pointList[0].getY() - row) * (pointList[k + 1].getY() - row) < 0.0f)
                    {
                        PWdouble tmpx = (pointList[0].getX() - pointList[k + 1].getX()) / (pointList[0].getY() - pointList[k + 1].getY()) * (row - pointList[0].getY()) + pointList[0].getX();
                        if (tmpx > xMax) xMax = tmpx;
                        if (tmpx < xMin) xMin = tmpx;
                    }
                }
                if (pointList[k + 1].getY() != pointList[k].getY())
                {
                    if ((pointList[k].getY() - row) * (pointList[k + 1].getY() - row) < 0.0f)
                    {
                        PWdouble tmpx = (pointList[k + 1].getX() - pointList[k].getX()) / (pointList[k + 1].getY() - pointList[k].getY()) * (row - pointList[k + 1].getY()) + pointList[k + 1].getX();
                        if (tmpx > xMax) xMax = tmpx;
                        if (tmpx < xMin) xMin = tmpx;
                    }
                }
                PWint ixMin = static_cast<PWint>(xMin);
                PWint ixMax = static_cast<PWint>(xMax);
                ixMin = max(0, ixMin);
                ixMax = min(WINDOW_WIDTH - 1, ixMax);
                /* parallel optimize */
#pragma omp parallel for
                for (PWint col = ixMin; col <= ixMax; col++)
                {
                    PWdouble v0 = (col - screen[2].getX()) * (screen[1].getY() - screen[2].getY()) + (row - screen[2].getY()) * (screen[2].getX() - screen[1].getX());
                    v0 /= (screen[0].getX() - screen[2].getX()) * (screen[1].getY() - screen[2].getY()) + (screen[0].getY() - screen[2].getY()) * (screen[2].getX() - screen[1].getX());
                    PWdouble v1 = (col - screen[0].getX()) * (screen[2].getY() - screen[0].getY()) + (row - screen[0].getY()) * (screen[0].getX() - screen[2].getX());
                    v1 /= (screen[1].getX() - screen[0].getX()) * (screen[2].getY() - screen[0].getY()) + (screen[1].getY() - screen[0].getY()) * (screen[0].getX() - screen[2].getX());
                    PWdouble v2 = 1 - v0 - v1;
                    PWdouble depthInView = 1 / (v0 / p[0].getZ() + v1 / p[1].getZ() + v2 / p[2].getZ());
                    PWdouble depthInProj = 1 / (v0 / screen[0].getZ() + v1 / screen[1].getZ() + v2 / screen[2].getZ());
                    if (zBuffer_[row * WINDOW_WIDTH + col] > depthInProj) continue;
                    zBuffer_[row * WINDOW_WIDTH + col] = depthInProj;
                    /* Z-fix */
                    {
                        v0 = v0 * depthInView / p[0].getZ();
                        v1 = v1 * depthInView / p[1].getZ();
                        v2 = 1 - v0 - v1;
                    }
                    Math::Vector3d jiInView = p[0] * v0 + p[1] * v1 + p[2] * v2;
                    PWdouble uInView = pu[0] * v0 + pu[1] * v1 + pu[2] * v2;
                    PWdouble vInView = pv[0] * v0 + pv[1] * v1 + pv[2] * v2;
                    uInView = min(max(uInView, 0), 1);
                    vInView = min(max(vInView, 0), 1);
                    /* Pixel level lighting */
                    Math::Vector3d l = (lightInView - jiInView).normal();
                    Math::Vector3d r = norm * 2 * l.dot(norm) - l;
                    Math::Vector3d v = -jiInView.normal();
                    PWdouble vDiff = max(norm.dot(l), 0.0f);
                    PWdouble vSpec = max(v.dot(r) * v.dot(r) * v.dot(r) * v.dot(r), 0.0f);
                    PWdouble vAmb = 1.0f;
                    PWbyte* position = (PWbyte *)(texture_.bmBits);
                    position = position + texture_.bmWidthBytes * static_cast<PWint>(vInView * (texture_.bmHeight - 1));
                    position = position + texture_.bmBitsPixel * static_cast<PWint>(uInView * (texture_.bmWidth - 1)) / 8;
                    PWuint blue = static_cast<PWuint>(*position * (vDiff * lightDiffuse_.getX() + vSpec * lightSpecular_.getX() + vAmb * lightAmbient.getX()));
                    PWuint green = static_cast<PWuint>(*(position + 1) * (vDiff * lightDiffuse_.getY() + vSpec * lightSpecular_.getY() + vAmb * lightAmbient.getY()));
                    PWuint red = static_cast<PWuint>(*(position + 2) * (vDiff * lightDiffuse_.getZ() + vSpec * lightSpecular_.getZ() + vAmb * lightAmbient.getZ()));
                    bmpBuffer_[row * WINDOW_WIDTH + col] = (red << 16) | (green << 8) | blue;
                }
            }
        }
    }

    /* Copy buffer */
    CHAR fpsBuffer[256];
    sprintf_s(fpsBuffer, 256, "FPS: %.1f", fps_);
    SetBkColor(hMemDC_, RGB(0xD7, 0xC4, 0xBB));
    SetTextColor(hMemDC_, RGB(0xD0, 0x10, 0x4C));
    TextOut(hMemDC_, 0, 0, fpsBuffer, lstrlen(fpsBuffer));
    SelectObject(hMemDC_, hBITMAP_);
    BitBlt(hDC_, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC_, 0, 0, SRCCOPY);

    /* Count FPS */
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
                FLOAT zDelta = 1.0f * GET_WHEEL_DELTA_WPARAM(wParam) / 1200;
                //ppwgl->camera_.moveForward(zDelta);
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
