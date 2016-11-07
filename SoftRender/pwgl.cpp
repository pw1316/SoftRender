#include "pwgl.h"

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
    
    /* 顶点缓存 */
    vertexBuffer_[0] = Vertex3F(-0.5f, -0.5f, -0.5f);
    vertexBuffer_[1] = Vertex3F(-0.5f, -0.5f, 0.5f);
    vertexBuffer_[2] = Vertex3F(-0.5f, 0.5f, -0.5f);
    vertexBuffer_[3] = Vertex3F(-0.5f, 0.5f, 0.5f);
    vertexBuffer_[4] = Vertex3F(0.5f, -0.5f, -0.5f);
    vertexBuffer_[5] = Vertex3F(0.5f, -0.5f, 0.5f);
    vertexBuffer_[6] = Vertex3F(0.5f, 0.5f, -0.5f);
    vertexBuffer_[7] = Vertex3F(0.5f, 0.5f, 0.5f);
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
    /* 世界坐标变换 */
    rotAlpha_ = 0.0f;
    rotBeta_ = 0.0f;
    rotGamma_ = 0.0f;
    /* 摄像机 */
    camera_.setPosition(5.0f, 5.0f, 5.0f);
    camera_.setLookAt(0.0f, 0.0f, 0.0f);
    camera_.setUp(0.0f, 1.0f, 0.0f);
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
    /* Reset*/
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
    {
        bmpBuffer_[i] = 0x00D7C4BB;
    }

    /*TODO Render*/
    Matrix4x4 transform;
    transform.setRotate(0.0f, 1.0f, 0.0f, rotGamma_ / 180.0f * 3.1415926536f);
    transform.addRotate(0.0f, 0.0f, 1.0f, rotBeta_ / 180.0f * 3.1415926536f);
    transform.addRotate(0.0f, 1.0f, 0.0f, rotAlpha_ / 180.0f * 3.1415926536f);
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
        if (norm[2] < 0) continue;
        /* 光照 */
        //TODO
        /* 裁剪 */
        //TODO
        /* 投影 */
        //TODO
        /* 光栅化 */
        //TODO
    }

    /* Copy buffer */
    SetBkColor(hMemDC_, RGB(0xD7, 0xC4, 0xBB));
    SetTextColor(hMemDC_, RGB(0xD0, 0x10, 0x4C));
    TextOut(hMemDC_, 0, 0, "FPS:", 4);
    SelectObject(hMemDC_, hBITMAP_);
    BitBlt(hDC_, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC_, 0, 0, SRCCOPY);
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
            }
        }
        if (!wasHandled)
        {
            result = DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    return result;
}

