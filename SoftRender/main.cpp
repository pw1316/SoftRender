#include "PWGL.hpp"
#include "Vertex.h"
#include "Matrix.h"
#include "Camera.h"

//#define TEST

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
        Vertex3F v1(0, 0, 0);
        Camera camera;
        Matrix4x4 m = camera.matrix();
        v1 = v1.toPoint4F().product(camera.matrix()).toVertex3F();
    }
}
#endif