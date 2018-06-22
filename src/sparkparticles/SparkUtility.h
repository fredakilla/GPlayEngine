#pragma once

#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include <spark/SPARK_Core.h>

namespace SPK {

class Vector3D;

namespace GP3D {


    ////////////////////////////
    /// Conversion functions ///
    ////////////////////////////

    /**
    * @brief Converts a SPARK Vector3D to a GPlay3D Vector3
    * @param v : the Vector3D to convert
    * @return the GPlay3D Vector3
    */
    //inline gplay::Vector3 spk2gp3d(const Vector3D& v);

    /**
    * @brief Converts a GPlay3D Vector3 to a SPARK Vector3D
    * @param v : the Vector3 to convert
    * @return the SPARK Vector3D
    */
    //inline SPK::Vector3D gp3d2spk(const gplay::Vector3& v);

    inline gplay::Vector3 spk2gp3d(const Vector3D& v)
    {
        return gplay::Vector3(v.x, v.y, v.z);
    }

    inline SPK::Vector3D gp3d2spk(const gplay::Vector3& v)
    {
        return Vector3D(v.x, v.y, v.z);
    }

}}
