#include "pwgl.h"
#include "Vertex.h"
#include "Matrix.h"

#define TEST

#ifndef TEST
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    //Notice heap failure
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            PWGL *ppwgl = PWGL::getInstance();
            if (SUCCEEDED(ppwgl->initWindow()))
            {
                ppwgl->mainLoop();
            }
            PWGL::releaseInstance();
            ppwgl = nullptr;
        }
        CoUninitialize();
    }
    return 0;
}
#else
int main() {
    {
        Vertex3F v1(3, 4, 5);
        Vertex4F v1p = v1.toPoint4F();
        Matrix4x4 m1, m2, m3;
        m1(1, 1) = 1.0f; m1(1, 2) = 1.0f; m1(1, 3) = 1.0f; m1(1, 4) = 1.0f;
        m1(2, 1) = 1.0f; m1(2, 2) = 1.0f; m1(2, 3) = 1.0f; m1(2, 4) = 1.0f;
        m1(3, 1) = 1.0f; m1(3, 2) = 1.0f; m1(3, 3) = 1.0f; m1(3, 4) = 1.0f;
        m1(4, 1) = 1.0f; m1(4, 2) = 1.0f; m1(4, 3) = 1.0f; m1(4, 4) = 1.0f;
        m2 = m1 + m1;
        m3 = m1 * m2;
        m1.setTranslate(1, 0, 0);
        m2.setTranslate(v1);
        v1p = v1p.product(m1);
        v1 = v1p.toVertex3F();
    }
}
#endif