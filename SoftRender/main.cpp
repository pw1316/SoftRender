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
        Matrix4x4 m1, m2, m3;
        m1(1, 1) = 1.0f; m1(1, 2) = 1.0f; m1(1, 3) = 1.0f; m1(1, 4) = 1.0f;
        m1(2, 1) = 1.0f; m1(2, 2) = 1.0f; m1(2, 3) = 1.0f; m1(2, 4) = 1.0f;
        m1(3, 1) = 1.0f; m1(3, 2) = 1.0f; m1(3, 3) = 1.0f; m1(3, 4) = 1.0f;
        m1(4, 1) = 1.0f; m1(4, 2) = 1.0f; m1(4, 3) = 1.0f; m1(4, 4) = 1.0f;
        m2 = m1 + m1;
        m3 = m1 * m2;
        m1.setTranslate(1, 0, 0);
        m1.setRotate(1, 0, 0, 1.57079632679489f);
        m2.setTranslate(v1);
    }
}
#endif