#pragma once
#include <windows.h>

/* Define assertion */
#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

inline BOOL isAndSetZero(FLOAT &value);

class Vertex3F
{
public:
    Vertex3F() :x_(0.0f), y_(0.0f), z_(0.0f) {}
    Vertex3F(FLOAT x, FLOAT y, FLOAT z) :x_(x), y_(y), z_(z) { zeroFix(); }
    ~Vertex3F() {}

private:
    void zeroFix();
    float x_, y_, z_;
};