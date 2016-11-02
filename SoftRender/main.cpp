#include "pwgl.h"
#include "Vertex.h"

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
        Vertex3F a(1.0f, 0.0f, 0.0f);
        Vertex3F b(0.0f, 1.0f, 0.0f);
        Vertex3F c = crossProduct(a, b);
        Vertex4F d = c.toPoint4F();
        FLOAT f = dotProduct(a, b);
        b = a.normalize();
    }
}
#endif