#include "header.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    //Notice heap failure
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
    
    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            DemoApp app;
            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);

        // Obtain the size of the drawing area.
        RECT rc;
        GetClientRect(
            hwnd,
            &rc
        );

        // Save the original object
        HGDIOBJ original = NULL;
        original = SelectObject(
            ps.hdc,
            GetStockObject(DC_PEN)
        );

        // Create a pen.            
        HPEN blackPen = CreatePen(PS_SOLID, 3, 0);

        // Select the pen.
        SelectObject(ps.hdc, blackPen);

        // Draw a rectangle.
        Rectangle(
            ps.hdc,
            rc.left + 100,
            rc.top + 100,
            rc.right - 100,
            rc.bottom - 100);

        DeleteObject(blackPen);

        // Restore the original object
        SelectObject(ps.hdc, original);

        EndPaint(hwnd, &ps);
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}