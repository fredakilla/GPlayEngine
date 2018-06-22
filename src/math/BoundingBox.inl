#include "BoundingBox.h"

namespace gplay
{

inline BoundingBox& BoundingBox::operator*=(const Matrix& matrix)
{
    transform(matrix);
    return *this;
}

inline const BoundingBox operator*(const Matrix& matrix, const BoundingBox& box)
{
    BoundingBox b(box);
    b.transform(matrix);
    return b;
}

inline bool operator== (const BoundingBox& a, const BoundingBox& b)
{
    return a.min == b.min && a.max == b.max;
}

inline bool BoundingBox::contains(const Vector3& point) const
{
    return min.x <= point.x && min.y <= point.y && min.z <= point.z &&
            point.x <= max.x && point.y <= max.y && point.z <= max.z;
}

}
