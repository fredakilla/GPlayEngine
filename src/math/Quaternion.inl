#include "Quaternion.h"

namespace gplay
{

inline const Quaternion Quaternion::operator*(const Quaternion& q) const
{
    Quaternion result(*this);
    result.multiply(q);
    return result;
}

inline Quaternion& Quaternion::operator*=(const Quaternion& q)
{
    multiply(q);
    return *this;
}

inline bool Quaternion::operator==(const Quaternion& v) const
{
    return x == v.x && y == v.y && z == v.z && w == v.w;
}

inline bool Quaternion::operator!=(const Quaternion& v) const
{
    return x != v.x || y != v.y || z != v.z || w != v.w;
}

}
