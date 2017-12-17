#include "PWRenderer.hpp"

#include <algorithm>
#include <cstdio>
#include <omp.h>

#define PI 3.1415926535898

/* Static Menber */
PWGL* PWGL::instance_ = nullptr;
LPCSTR PWGL::WINDOW_CLASS_NAME = "PWGL";
LPCSTR PWGL::WINDOW_NAME = "SOFT_RENDER";
const PWint PWGL::WINDOW_WIDTH = 1600;
const PWint PWGL::WINDOW_HEIGHT = 900;

/* Methods */
PWGL *PWGL::getInstance()
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
        PWGL::WINDOW_CLASS_NAME,
        PWGL::WINDOW_NAME,
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
    bmpInfo_.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo_.bmiHeader.biWidth = WINDOW_WIDTH;
    bmpInfo_.bmiHeader.biHeight = -WINDOW_HEIGHT;
    bmpInfo_.bmiHeader.biPlanes = 1;
    bmpInfo_.bmiHeader.biBitCount = 32;//ABGR
    bmpInfo_.bmiHeader.biCompression = BI_RGB;
    bmpInfo_.bmiHeader.biSizeImage = 0;
    bmpInfo_.bmiHeader.biXPelsPerMeter = 0;
    bmpInfo_.bmiHeader.biYPelsPerMeter = 0;
    bmpInfo_.bmiHeader.biClrUsed = 0;
    bmpInfo_.bmiHeader.biClrImportant = 0;
    hBITMAP_ = CreateDIBSection(hMemDC_, &bmpInfo_, DIB_RGB_COLORS, (void**)&bmpBuffer_, NULL, 0);
    //hTexture_ = (HBITMAP)::LoadImage(HINST_THISCOMPONENT, "Texture.bmp", IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);
    //GetObject(hTexture_, sizeof(BITMAP), &texture_);
#pragma endregion

    p_et_ = new EdgeTable[WINDOW_HEIGHT];
    /* 1 meter == 40 in model */
    p_model_ = new FileReader::ObjModel();
    p_model_->readObj("models/2.obj");
    /* Model-World parameters */
    rotAlpha_ = 0;
    rotAlphaV_ = 0;
    rotBeta_ = 0;
    rotBetaV_ = 0;
    rotGamma_ = 0;
    rotGammaV_ = 0;
    /* World-View matrix */
    camera_ = Camera::lookAt(0, 64, 80, 0, 64, 0, 0, 1, 0);
    /* Projection parameters */
    fovx_ = 60;
    aspect_ = 1.0 * WINDOW_WIDTH / WINDOW_HEIGHT;
    near_ = 1;
    far_ = 4500;
    /* Light */
    lightPos.set(0, 64, 80);// World position
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

HRESULT PWGL::onRender()
{
    LARGE_INTEGER startTick = {};
    LARGE_INTEGER endTick = {};
    QueryPerformanceCounter(&startTick);
    /* Reset frame buffer */
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
    {
        /* USUAO */
        bmpBuffer_[i] = 0x0093B491;
    }
    /* Reset scanline */
    pt_.clear();
    ipl_.clear();
    for (int i = 0; i < WINDOW_HEIGHT; i++)
    {
        p_et_[i].clear();
    }
    aet_.clear();

    Math::Matrix44d transform;// MV matrix
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
    /* pointList: Clipped vertex list */
    auto &vertexBuffer = p_model_->m_vertices;
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
            p[0] *= 25; p[0].setY(p[0].getY() + 64);
            p[1] *= 25; p[1].setY(p[1].getY() + 64);
            p[2] *= 25; p[2].setY(p[2].getY() + 64);
            /* View */
            p[0] = (transform * p[0].toVector4d1()).toVector3d();
            p[1] = (transform * p[1].toVector4d1()).toVector3d();
            p[2] = (transform * p[2].toVector4d1()).toVector3d();
            Math::Vector3d norm = Math::cross(p[1] - p[0], p[2] - p[1]).normal();

            /* Clip and Projection */
            Math::Matrix44d projection(WINDOW_WIDTH / std::tan(fovx_ / 180 * PI / 2) / 2.0, 0, -WINDOW_WIDTH / 2.0, 0,
                0, WINDOW_HEIGHT * aspect_ / tan(fovx_ / 180 * PI / 2) / 2.0, -WINDOW_HEIGHT / 2.0, 0,
                0, 0, far_ / (far_ - near_), near_ * far_ / (far_ - near_),
                0, 0, -1, 0);
            PWint index = 0;
            Math::Vector3d pointList[12];
            EdgeNode edgeList[12];
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
            /* If x and y are completely out of range */
            PWdouble left = min(screen[0].getX(), min(screen[1].getX(), screen[2].getX()));
            PWdouble right = max(screen[0].getX(), max(screen[1].getX(), screen[2].getX()));
            PWdouble top = max(screen[0].getY(), max(screen[1].getY(), screen[2].getY()));
            PWdouble bottom = min(screen[0].getY(), min(screen[1].getY(), screen[2].getY()));
            if (right < 0 || left > WINDOW_WIDTH || bottom > WINDOW_HEIGHT || top < 0)
            {
                continue;
            }
            /* Rasterize */
            /* 1. Add the Clipped polygon */
            pt_.emplace_back();
            auto &poly = pt_.back();
            //PWbyte* textureBuffer = (PWbyte *)(texture_.bmBits);
            poly.m_id = pt_.size() - 1;
            poly.m_a = normProj.getX();
            poly.m_b = normProj.getY();
            poly.m_c = normProj.getZ();
            poly.m_d = -(poly.m_a * screen[1].getX() + poly.m_b * screen[1].getY() + poly.m_c * screen[1].getZ());
            Math::Vector3d polygonInView = (p[0] + p[1] + p[2]) / 3;
            Math::Vector3d l = (lightInView - polygonInView).normal();
            Math::Vector3d r = norm * 2 * l.dot(norm) - l;
            Math::Vector3d v = -polygonInView.normal();
            PWdouble vDiff = max(norm.dot(l), 0.0f);
            PWdouble vSpec = max(v.dot(r) * v.dot(r) * v.dot(r) * v.dot(r), 0.0f);
            PWdouble vAmb = 1.0f;
            PWuint blue = (vDiff * lightDiffuse_.getX() + vSpec * lightSpecular_.getX() + lightAmbient.getX()) * 255;
            PWuint green = (vDiff * lightDiffuse_.getY() + vSpec * lightSpecular_.getY() + lightAmbient.getY()) * 255;
            PWuint red = (vDiff * lightDiffuse_.getZ() + vSpec * lightSpecular_.getZ() + lightAmbient.getZ()) * 255;
            poly.m_color = Math::Vector3d(blue, green, red);
            poly.m_isIn = false;
            /* 2. Add all of the edges of the polygon into the ET */
            PWint ETindex = 0;
            for (int k = 0; k < index; k++)
            {
                PWint x0 = (PWint)pointList[k].getX();
                PWint y0 = (PWint)pointList[k].getY();
                PWint x1 = (PWint)pointList[(k + 1) % index].getX();
                PWint y1 = (PWint)pointList[(k + 1) % index].getY();
                /* Ignore horizontal line */
                if (y0 == y1)
                {
                    continue;
                }
                auto &edge = edgeList[ETindex];
                edge.m_polygonId = poly.m_id;
                edge.m_ymax = max(y0, y1);
                /* ignore edge below 0 */
                if (edge.m_ymax < 0)
                {
                    continue;
                }
                PWint lowx = (y0 < y1) ? x0 : x1;
                PWint lowy = (y0 < y1) ? y0 : y1;
                edge.m_x = lowx;
                edge.m_y = lowy;
                /* ignore edge below 0 */
                if (edge.m_y >= WINDOW_HEIGHT)
                {
                    continue;
                }
                PWdouble v0 = (lowx - screen[2].getX()) * (screen[1].getY() - screen[2].getY()) + (lowy - screen[2].getY()) * (screen[2].getX() - screen[1].getX());
                v0 /= (screen[0].getX() - screen[2].getX()) * (screen[1].getY() - screen[2].getY()) + (screen[0].getY() - screen[2].getY()) * (screen[2].getX() - screen[1].getX());
                PWdouble v1 = (lowx - screen[0].getX()) * (screen[2].getY() - screen[0].getY()) + (lowy - screen[0].getY()) * (screen[0].getX() - screen[2].getX());
                v1 /= (screen[1].getX() - screen[0].getX()) * (screen[2].getY() - screen[0].getY()) + (screen[1].getY() - screen[0].getY()) * (screen[0].getX() - screen[2].getX());
                PWdouble v2 = 1 - v0 - v1;
                edge.m_z = 1 / (v0 / screen[0].getZ() + v1 / screen[1].getZ() + v2 / screen[2].getZ());
                edge.m_dx = 1.0 * (x1 - x0) / (y1 - y0);
                edge.m_dy = 1;
                edge.m_dz = (pointList[k + 1].getZ() - pointList[k].getZ()) / (y1 - y0);
                if (edge.m_y < 0)
                {
                    edge.m_x += edge.m_dx * (-edge.m_y);
                    edge.m_y = 0;
                    edge.m_z += edge.m_dz * (-edge.m_y);
                }
                ETindex++;
            }
            /* 3. Fix non-local-extremum */
            if (ETindex > 0)
            {
                /*   First Edge */
                {
                    auto &prevEdge = edgeList[ETindex - 1];
                    auto &edge = edgeList[0];
                    auto &nextEdge = edgeList[1];
                    if (Math::equal(nextEdge.m_ymax, edge.m_y) || Math::equal(prevEdge.m_ymax, edge.m_y))
                    {
                        edge.m_x += edge.m_dx;
                        edge.m_y += edge.m_dy;
                        edge.m_z += edge.m_dz;
                    }
                }
                /* Internal Edge */
                for (int idx = 1; idx < ETindex - 1; ++idx)
                {
                    auto &prevEdge = edgeList[idx - 1];
                    auto &edge = edgeList[idx];
                    auto &nextEdge = edgeList[idx + 1];
                    if (Math::equal(nextEdge.m_ymax, edge.m_y) || Math::equal(prevEdge.m_ymax, edge.m_y))
                    {
                        edge.m_x += edge.m_dx;
                        edge.m_y += edge.m_dy;
                        edge.m_z += edge.m_dz;
                    }
                }
                /* Last Edge */
                {
                    auto &prevEdge = edgeList[ETindex - 2];
                    auto &edge = edgeList[ETindex - 1];
                    auto &nextEdge = edgeList[0];
                    if (Math::equal(nextEdge.m_ymax, edge.m_y) || Math::equal(prevEdge.m_ymax, edge.m_y))
                    {
                        edge.m_x += edge.m_dx;
                        edge.m_y += edge.m_dy;
                        edge.m_z += edge.m_dz;
                    }
                }
            }
            /* 4. Move from edgelist to p_et_ */
            for (int idx = 0; idx < ETindex; ++idx)
            {
                p_et_[static_cast<int>(edgeList[idx].m_y)].push_back(edgeList[idx]);
            }
        }
    }
    /* Pixel Shader */
    /* 1. Scanline from bottom to top */
    for (PWint row = 0; row < WINDOW_HEIGHT; ++row)
    {
        ipl_.clear();
        /* Move edge from ET to AET */
        if (!p_et_[row].empty())
        {
            for (auto &edge : p_et_[row])
            {
                aet_.push_back(edge);
            }
        }
        if (aet_.empty())
            continue;
        /* Sort */
        aet_.sort(edgeLessComparator);
        /* First edge's polygon is in */
        ipl_.insert(aet_.front().m_polygonId);
        /* Each scanline period */
        auto edge1 = aet_.begin();
        auto edge2 = aet_.end();
        while (edge1 != aet_.end())
        {
            edge2 = std::next(edge1);
            /* Find nearest polygon BG if returns -1 */
            auto polyId = getNearestPoly(pt_, ipl_, aet_, edge1, edge2);
            if (polyId >= 0 && row >= 0)
            {
                for (PWint col = max(0, edge1->m_x); col < min(edge2->m_x, WINDOW_WIDTH); ++col)
                {
                    PWint red = pt_[polyId].m_color.getZ();
                    PWint green = pt_[polyId].m_color.getY();
                    PWint blue = pt_[polyId].m_color.getX();
                    bmpBuffer_[(WINDOW_HEIGHT - 1 - row) * WINDOW_WIDTH + col] = (red << 16) | (green << 8) | blue;
                }
            }
            /* Add or remove polygon of edge2 */
            if (edge2 != aet_.end())
            {
                PWbool edge2InIPL = false;
                if (ipl_.find(edge2->m_polygonId) != ipl_.end())
                {
                    ipl_.erase(edge2->m_polygonId);
                }
                else
                {
                    ipl_.insert(edge2->m_polygonId);
                }
            }
            edge1 = edge2;
        }
        /* Next scanline */
        for (auto edge = aet_.begin(); edge != aet_.end();)
        {
            edge->m_x += edge->m_dx;
            edge->m_y += edge->m_dy;
            edge->m_z += edge->m_dz;
            if (edge->m_y > edge->m_ymax)
            {
                edge = aet_.erase(edge);
            }
            else
            {
                ++edge;
            }
        }
    }

    /* Copy buffer */
    CHAR fpsBuffer[256];
    sprintf_s(fpsBuffer, 256, "FPS: %.1f", fps_);
    SetBkColor(hMemDC_, RGB(0x91, 0xB4, 0x93));
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
                PWdouble zDelta = 1.0f * GET_WHEEL_DELTA_WPARAM(wParam) / 1200;
                ppwgl->camera_.addTranslate(0, 0, zDelta);
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
