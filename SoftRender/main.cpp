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
#include <iostream>
#include <string>
#include <sstream>
#include "ObjReader.hpp"
int main() {
    FileReader::ObjModel model;
    model.readObj("test.obj");
}
#endif