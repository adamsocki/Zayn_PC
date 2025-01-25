

#include "matrix.h"


mat4 SetViewDirection(vec3 position, vec3 direction, vec3 up)
{
    mat4 viewMatrix;
    vec3 w = Normalize(direction);
    vec3 u = Normalize(Cross(w, up));
    vec3 v = Cross(w, u);

    viewMatrix = Identity4();
    viewMatrix.m00 = u.x;
    viewMatrix.m10 = u.y;
    viewMatrix.m20 = u.z;
    viewMatrix.m01 = v.x;
    viewMatrix.m11 = v.y;
    viewMatrix.m21 = v.z;
    viewMatrix.m02 = w.x;
    viewMatrix.m12 = w.y;
    viewMatrix.m22 = w.z;
    viewMatrix.m30 = -Dot(u, position);
    viewMatrix.m31 = -Dot(v, position);
    viewMatrix.m32 = -Dot(w, position);

    return viewMatrix;
}

mat4 SetViewTarget(vec3 position, vec3 target, vec3 up)
{
    return SetViewDirection(position, target - position, up);
}