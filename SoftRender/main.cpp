#include "PWRenderer.hpp"

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
    Math::Vector4d v1(1, 2, 3, 2);
    Math::Vector4d v2 = v1.normal();
    Math::Vector4d v3(1, 2, 3, 0);
    Math::Vector4d v4 = v3.normal();
    v4.setW(111);
}
#endif