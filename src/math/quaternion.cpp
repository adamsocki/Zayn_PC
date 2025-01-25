#include "quaternion.h"


bool NearlyEquals(quaternion a, quaternion b, real32 epsilon) 
{
    return NearlyEquals(a.x, b.x, epsilon) &&
        NearlyEquals(a.y, b.y, epsilon) &&
        NearlyEquals(a.z, b.z, epsilon) &&
        NearlyEquals(a.w, b.w, epsilon);
}

bool HasNaN(quaternion q) {
    return isnan(q.x) || isnan(q.y) || isnan(q.z) || isnan(q.w);
}
