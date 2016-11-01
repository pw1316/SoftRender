#include "Vertex.h"

#include <cmath>

inline BOOL isAndSetZero(FLOAT & value)
{
    if (abs(value) < 1e-10f)
    {
        value = 0.0f;
        return true;
    }
    return false;
}

void Vertex3F::zeroFix()
{
    isAndSetZero(x_);
    isAndSetZero(y_);
    isAndSetZero(z_);
}
