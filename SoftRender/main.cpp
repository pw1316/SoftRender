#include "header.h"

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
