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
    bmpInfo_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);//�ṹ����ֽ���
    bmpInfo_.bmiHeader.biWidth = WINDOW_WIDTH;//������Ϊ��λ��λͼ��
    bmpInfo_.bmiHeader.biHeight = -WINDOW_HEIGHT;//������Ϊ��λ��λͼ��,��Ϊ������ʾ�����Ͻ�Ϊԭ�㣬���������½�Ϊԭ��
    bmpInfo_.bmiHeader.biPlanes = 1;//Ŀ���豸��ƽ��������������Ϊ1
    bmpInfo_.bmiHeader.biBitCount = 32; //λͼ��ÿ�����ص�λ��
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
    p_model_ = new FileReader::ObjModel();
    p_model_->readObj("obj.model");
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
    }
    pt_.clear();
    ipl_.clear();
    et_.clear();
    aet_.clear();

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
    auto &vertexBuffer = p_model_->m_vertices;
    auto &texBuffer = p_model_->m_texcoords;
    for (auto &group : p_model_->m_groups)
    {
        for (auto &triIdx : group.second.m_triangleIndices)
        {
            auto &triangle = p_model_->m_triangles[triIdx];
            /* Model */
            Math::Vector3d p[3];
            p[0] = vertexBuffer[triangle.m_vertexIndex[0]];
            p[1] = vertexBuffer[triangle.m_vertexIndex[1]];
            p[2] = vertexBuffer[triangle.m_vertexIndex[2]];
            Math::Vector2d uv[3];
            uv[0] = texBuffer[triangle.m_texcoordIndex[0]];
            uv[1] = texBuffer[triangle.m_texcoordIndex[1]];
            uv[2] = texBuffer[triangle.m_texcoordIndex[2]];
            /* View */
            p[0] = (transform * p[0].toVector4d1()).toVector3d();
            p[1] = (transform * p[1].toVector4d1()).toVector3d();
            p[2] = (transform * p[2].toVector4d1()).toVector3d();
            Math::Vector3d norm = Math::cross(p[1] - p[0], p[2] - p[1]);
            /* Clip and Projection */
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
            pt_.emplace_back();
            auto &poly = pt_.back();
            PWbyte* textureBuffer = (PWbyte *)(texture_.bmBits);
            poly.m_id = pt_.size() - 1;
            poly.m_a = norm.getX();
            poly.m_b = norm.getY();
            poly.m_c = norm.getZ();
            poly.m_d = -(poly.m_a * p[1].getX() + poly.m_b * p[1].getY() + poly.m_c * p[1].getZ());
            PWuint blue = textureBuffer[texture_.bmWidthBytes * static_cast<PWint>(uv[0].getY() * (texture_.bmHeight - 1)) + \
                texture_.bmBitsPixel * static_cast<PWint>(uv[0].getX() * (texture_.bmWidth - 1)) / 8];
            blue += textureBuffer[texture_.bmWidthBytes * static_cast<PWint>(uv[1].getY() * (texture_.bmHeight - 1)) + \
                texture_.bmBitsPixel * static_cast<PWint>(uv[1].getX() * (texture_.bmWidth - 1)) / 8];
            blue += textureBuffer[texture_.bmWidthBytes * static_cast<PWint>(uv[2].getY() * (texture_.bmHeight - 1)) + \
                texture_.bmBitsPixel * static_cast<PWint>(uv[2].getX() * (texture_.bmWidth - 1)) / 8];
            blue /= 3;
            PWuint green = textureBuffer[texture_.bmWidthBytes * static_cast<PWint>(uv[0].getY() * (texture_.bmHeight - 1)) + \
                texture_.bmBitsPixel * static_cast<PWint>(uv[0].getX() * (texture_.bmWidth - 1)) / 8 + 1];
            blue += textureBuffer[texture_.bmWidthBytes * static_cast<PWint>(uv[1].getY() * (texture_.bmHeight - 1)) + \
                texture_.bmBitsPixel * static_cast<PWint>(uv[1].getX() * (texture_.bmWidth - 1)) / 8 + 1];
            blue += textureBuffer[texture_.bmWidthBytes * static_cast<PWint>(uv[2].getY() * (texture_.bmHeight - 1)) + \
                texture_.bmBitsPixel * static_cast<PWint>(uv[2].getX() * (texture_.bmWidth - 1)) / 8 + 1];
            blue /= 3;
            PWuint red = textureBuffer[texture_.bmWidthBytes * static_cast<PWint>(uv[0].getY() * (texture_.bmHeight - 1)) + \
                texture_.bmBitsPixel * static_cast<PWint>(uv[0].getX() * (texture_.bmWidth - 1)) / 8 + 2];
            blue += textureBuffer[texture_.bmWidthBytes * static_cast<PWint>(uv[1].getY() * (texture_.bmHeight - 1)) + \
                texture_.bmBitsPixel * static_cast<PWint>(uv[1].getX() * (texture_.bmWidth - 1)) / 8 + 2];
            blue += textureBuffer[texture_.bmWidthBytes * static_cast<PWint>(uv[2].getY() * (texture_.bmHeight - 1)) + \
                texture_.bmBitsPixel * static_cast<PWint>(uv[2].getX() * (texture_.bmWidth - 1)) / 8 + 2];
            blue /= 3;
            poly.m_color = Math::Vector3d(blue, green, red);
            poly.m_isIn = false;
            for (int k = 0; k < index - 1; k++)
            {
                PWint x0 = (PWint)pointList[k].getX();
                PWint x1 = (PWint)pointList[k].getY();
                PWint y0 = (PWint)pointList[k + 1].getX();
                PWint y1 = (PWint)pointList[k + 1].getY();
                /* Ignore horizontal line */
                if (y0 == y1)
                {
                    continue;
                }
                et_.emplace_back();
                auto &edge = et_.back();
                edge.m_polygonId = poly.m_id;
                edge.m_ymax = max(y0, y1);
                edge.m_x = (y0 < y1) ? x0 : x1;
                edge.m_dx = 1.0 * (x1 - x0) / (y1 - y0);
            }
            /* Fix non-local-extremum */
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
