#include "header.h"
#include "Utils.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    //Notice heap failure
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
    
    PW::GenerateSyncPosition gsp;
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
