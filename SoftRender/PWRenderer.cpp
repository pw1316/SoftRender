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
    camera_ = Camera::lookAt(0, 0, 80, 0, 0, 0, 0, 1, 0);
    /* Projection parameters */
    fovx_ = 60;
    aspect_ = 1.0 * WINDOW_WIDTH / WINDOW_HEIGHT;
    near_ = 1;
    far_ = 4500;
    /* Light */
    lightPos.set(0, 0, 80);// World position
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
    /* p: Face's vertices in view */
    /* proj: Face's vertices in projection */
    /* screen: Face's vertices in projection with w normalized */
    auto &vertexBuffer = p_model_->m_vertices;
    for (auto &group : p_model_->m_groups)
    {
        for (auto &polyIdx : group.second.m_polygonIndices)
        {
            auto &polygon = p_model_->m_polygons[polyIdx];
            /* View */
            PWint vertexLen = polygon.m_vertexIndex.size();
            std::vector<Math::Vector3d> p(vertexLen);
            std::vector<Math::Vector4d> proj(vertexLen);
            std::vector<Math::Vector3d> screen(vertexLen);
            std::vector<EdgeNode> edgeList(vertexLen);
            for (int i = 0; i < vertexLen; i++)
            {
                p[i] = (transform * vertexBuffer[polygon.m_vertexIndex[i]].toVector4d1()).toVector3d();
            }

            /* Clip and Projection */
            Math::Matrix44d projection(WINDOW_WIDTH / std::tan(fovx_ / 180 * PI / 2) / 2.0, 0, -WINDOW_WIDTH / 2.0, 0,
                0, WINDOW_HEIGHT * aspect_ / tan(fovx_ / 180 * PI / 2) / 2.0, -WINDOW_HEIGHT / 2.0, 0,
                0, 0, far_ / (far_ - near_), near_ * far_ / (far_ - near_),
                0, 0, -1, 0);
            bool isBehindCamera = false;
            for (int i = 0; i < vertexLen; i++)
            {
                proj[i] = projection * p[i].toVector4d1();
                screen[i] = proj[i].normal().toVector3d();
                isBehindCamera = proj[i].getZ() > 0;
            }
            /* Behind Camera */
            if (isBehindCamera)
            {
                continue;
            }
            /* Backface */
            Math::Vector3d normProj;
            {
                PWint idx = vertexLen - 1;
                for (PWint i = 0; i < vertexLen; i++)
                {
                    PWdouble x = (screen[idx].getZ() + screen[i].getZ()) * (screen[idx].getY() - screen[i].getY());
                    PWdouble y = (screen[idx].getX() + screen[i].getX()) * (screen[idx].getZ() - screen[i].getZ());
                    PWdouble z = (screen[idx].getY() + screen[i].getY()) * (screen[idx].getX() - screen[i].getX());
                    normProj += Math::Vector3d(x, y, z);
                    idx = i;
                }
                if (normProj.getZ() < 0)
                {
                    continue;
                }
            }
            /* If x and y are completely out of range */
            PWdouble left = min(screen[0].getX(), min(screen[1].getX(), screen[2].getX()));
            PWdouble right = max(screen[0].getX(), max(screen[1].getX(), screen[2].getX()));
            PWdouble top = max(screen[0].getY(), max(screen[1].getY(), screen[2].getY()));
            PWdouble bottom = min(screen[0].getY(), min(screen[1].getY(), screen[2].getY()));
            PWdouble back = min(screen[0].getZ(), min(screen[1].getZ(), screen[2].getZ()));
            PWdouble front = max(screen[0].getZ(), max(screen[1].getZ(), screen[2].getZ()));
            if (right < 0 || left > WINDOW_WIDTH || bottom > WINDOW_HEIGHT || top < 0 || front < -1 || back > 0)
            {
                continue;
            }
            /* Rasterize */
            /* 1. Add the polygon */
            pt_.emplace_back();
            auto &poly = pt_.back();
            poly.m_id = pt_.size() - 1;
            /* ax+by+cz+d=0 */
            poly.m_a = normProj.getX();
            poly.m_b = normProj.getY();
            poly.m_c = normProj.getZ();
            poly.m_d = 0;
            for (int i = 0; i < vertexLen; i++)
            {
                poly.m_d -= normProj.dot(screen[i]);
            }
            poly.m_d /= vertexLen;
            Math::Vector3d norm;
            {
                PWint idx = vertexLen - 1;
                for (PWint i = 0; i < vertexLen; i++)
                {
                    PWdouble x = (p[idx].getZ() + p[i].getZ()) * (p[idx].getY() - p[i].getY());
                    PWdouble y = (p[idx].getX() + p[i].getX()) * (p[idx].getZ() - p[i].getZ());
                    PWdouble z = (p[idx].getY() + p[i].getY()) * (p[idx].getX() - p[i].getX());
                    norm += Math::Vector3d(x, y, z);
                    idx = i;
                }
            }
            norm.normalize();
            Math::Vector3d polygonInView;
            for (PWint i = 0; i < vertexLen; i++)
            {
                polygonInView += p[i];
            }
            polygonInView /= vertexLen;
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
            /* 2. Add all of the edges of the polygon into the ET */
            PWint index = vertexLen;
            PWint ETindex = 0;
            for (int k = 0; k < index; k++)
            {
                PWint x0 = (PWint)screen[k].getX();
                PWint y0 = (PWint)screen[k].getY();
                PWint x1 = (PWint)screen[(k + 1) % index].getX();
                PWint y1 = (PWint)screen[(k + 1) % index].getY();
                /* Ignore horizontal line */
                if ((y0 == y1) || (y0 < 0 && y1 < 0) || (y0 >= WINDOW_HEIGHT && y1 >= WINDOW_HEIGHT))
                {
                    continue;
                }
                auto &edge = edgeList[ETindex];
                edge.m_polygonId = poly.m_id;
                edge.m_ymax = max(y0, y1);
                PWint lowx = (y0 < y1) ? x0 : x1;
                PWint lowy = (y0 < y1) ? y0 : y1;
                edge.m_x = lowx;
                edge.m_y = lowy;
                edge.m_dx = 1.0 * (x1 - x0) / (y1 - y0);
                if (edge.m_y < 0)
                {
                    edge.m_x += edge.m_dx * (-edge.m_y);
                    edge.m_y = 0;
                }
                ETindex++;
            }
            /* 3. Fix non-local-extremum */
            for (int idx = 0; idx < ETindex; ++idx)
            {
                auto &prevEdge = edgeList[(idx - 1 + ETindex) % ETindex];
                auto &edge = edgeList[idx];
                auto &nextEdge = edgeList[(idx + 1) % ETindex];
                if (nextEdge.m_ymax == edge.m_y || prevEdge.m_ymax == edge.m_y)
                {
                    edge.m_x += edge.m_dx;
                    edge.m_y += 1;
                }
            }
            /* 4. Move from edgelist to p_et_ */
            for (int idx = 0; idx < ETindex; ++idx)
            {
                if (edgeList[idx].m_y < WINDOW_HEIGHT)
                {
                    p_et_[edgeList[idx].m_y].push_back(edgeList[idx]);
                }
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
        aet_.sort([](const EdgeNode &lhs, const EdgeNode &rhs) {return lhs.m_x < rhs.m_x; });
        /* First edge's polygon is in */
        ipl_.push_back(aet_.front().m_polygonId);
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
                for (auto &it = ipl_.begin(); it != ipl_.end(); ++it)
                {
                    if (*it == edge2->m_polygonId)
                    {
                        it = ipl_.erase(it);
                        edge2InIPL = true;
                        break;
                    }
                }
                if (!edge2InIPL)
                {
                    ipl_.push_back(edge2->m_polygonId);
                }
            }
            edge1 = edge2;
        }
        /* Next scanline */
        for (auto edge = aet_.begin(); edge != aet_.end();)
        {
            edge->m_x += edge->m_dx;
            edge->m_y += 1;
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
