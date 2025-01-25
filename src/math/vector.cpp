//vector.cpp

#include "vector.h"
#include <math.h>




vec2 Cardinal4Clamp(vec2 dir) {
    vec2 result = {};

    int32 smallestComp;
    int32 largestComponent = LargestComponent(dir, &smallestComp);

    if (largestComponent == -1) { return result; }

    result.data[largestComponent] = dir.data[largestComponent];
    if (result.data[largestComponent] < 0) {
        result.data[largestComponent] = -1;
    }
    else if (result.data[largestComponent] > 0) {
        result.data[largestComponent] = 1;
    }

    return result;
}

bool HasNaN(vec3 v) {
    return isnan(v.x) || isnan(v.y) || isnan(v.z);
}

bool NearlyEquals(vec2 a, vec2 b, real32 epsilon) {
    return NearlyEquals(a.x, b.x, epsilon) &&
        NearlyEquals(a.y, b.y, epsilon);
}

bool NearlyEquals(vec3 a, vec3 b, real32 epsilon) {
    return NearlyEquals(a.x, b.x, epsilon) &&
        NearlyEquals(a.y, b.y, epsilon) &&
        NearlyEquals(a.z, b.z, epsilon);
}

bool NearlyEquals(vec4 a, vec4 b, real32 epsilon) {
    return NearlyEquals(a.x, b.x, epsilon) &&
        NearlyEquals(a.y, b.y, epsilon) &&
        NearlyEquals(a.z, b.z, epsilon) &&
        NearlyEquals(a.w, b.w, epsilon);
}

vec3 Round(vec3 a) {
    vec3 result = a;
    result.x = Roundf(a.x);
    result.y = Roundf(a.y);
    result.z = Roundf(a.z);
    return result;
}

vec3 Ceilv(vec3 a) {
    vec3 result = a;
    result.x = Ceilf(a.x);
    result.y = Ceilf(a.y);
    result.z = Ceilf(a.z);
    return result;
}

