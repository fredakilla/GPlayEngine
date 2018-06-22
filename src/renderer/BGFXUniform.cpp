#include "../renderer//BGFXUniform.h"
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

namespace gplay {


Uniform::Uniform() :
   _type(UT_UNDEFINED), _index(0), _effect(NULL)
{
}

Uniform::~Uniform()
{
    // hidden
}

Effect* Uniform::getEffect() const
{
    return _effect;
}

const char* Uniform::getName() const
{
    return _name.c_str();
}

const Uniform::UniformType Uniform::getType() const
{
    return _type;
}

const unsigned int Uniform::getIndex() const
{
    return _index;
}


BGFXUniform::BGFXUniform(const char * name, UniformType type, uint16_t num) :
    _handle(BGFX_INVALID_HANDLE)
{
    bgfx::UniformType::Enum bgfxUniformType;
    switch(type)
    {
        case UniformType::UT_SAMPLER: // Int, used for samplers only.
            bgfxUniformType =  bgfx::UniformType::Int1;
            break;
        case UniformType::UT_VECTOR4: // 4 floats vector.
            bgfxUniformType = bgfx::UniformType::Vec4;
            break;
        case UniformType::UT_MATRIX3: // 3x3 matrix.
            bgfxUniformType = bgfx::UniformType::Mat3;
            break;
        case UniformType::UT_MATRIX4: // 4x4 matrix.
            bgfxUniformType = bgfx::UniformType::Mat4;
            break;
        default:
            GP_ERROR("BGFXGpuProgram: Uniform type unknown.");
    };

    _num = num;
    _handle = bgfx::createUniform(name, bgfxUniformType, num);
    GP_ASSERT(bgfx::isValid(_handle));
}

BGFXUniform::~BGFXUniform()
{
    if(bgfx::isValid(_handle))
    {
         bgfx::destroy(_handle);
    }
}

void BGFXUniform::setValue(float value)
{
    GP_ASSERT(bgfx::isValid(_handle));
    bgfx::setUniform(_handle, &value);
}

void BGFXUniform::setValue(const float* values, unsigned int count)
{
    GP_ASSERT(bgfx::isValid(_handle));
    bgfx::setUniform(_handle, &values[0], count);
}

void BGFXUniform::setValue(int value)
{
    // in bgfx, Int used for samplers only.
    GP_ERROR("BGFXUniform::setValue : int reserved for samplers.");

    //GP_ASSERT(bgfx::isValid(_handle));
    //bgfx::setUniform(_handle, &value);    // doesn't works
    //setValue(static_cast<float>(value));  // works with float
}

void BGFXUniform::setValue(const int* values, unsigned int count)
{
    // in bgfx, Int used for samplers only.
    GP_ERROR("BGFXUniform::setValue : int reserved for samplers.");

    //GP_ASSERT(bgfx::isValid(_handle));
    //bgfx::setUniform(_handle, &values[0], count);     // doesn't works
    //GP_ERROR("Not implemented.");
}

void BGFXUniform::setValue(const Matrix& value)
{
    GP_ASSERT(bgfx::isValid(_handle));
    bgfx::setUniform(_handle, &value.m);
}

void BGFXUniform::setValue(const Matrix* values, unsigned int count)
{
    GP_ASSERT(bgfx::isValid(_handle));
    GP_ASSERT(_num >= count);
    bgfx::setUniform(_handle, &values[0].m, count);
}

void BGFXUniform::setValue(const Vector2& value)
{
    GP_ASSERT(bgfx::isValid(_handle));
    bgfx::setUniform(_handle, &value.x);
}

void BGFXUniform::setValue(const Vector2* values, unsigned int count)
{
    GP_ASSERT(bgfx::isValid(_handle));
    bgfx::setUniform(_handle, &values[0].x, count);
}

void BGFXUniform::setValue(const Vector3& value)
{
    GP_ASSERT(bgfx::isValid(_handle));
    bgfx::setUniform(_handle, &value.x);
}

void BGFXUniform::setValue(const Vector3* values, unsigned int count)
{
    GP_ASSERT(bgfx::isValid(_handle));
    bgfx::setUniform(_handle, &values[0].x, count);
}

void BGFXUniform::setValue(const Vector4& value)
{
    GP_ASSERT(bgfx::isValid(_handle));
    bgfx::setUniform(_handle, &value.x);
}

void BGFXUniform::setValue(const Vector4* values, unsigned int count)
{
    GP_ASSERT(bgfx::isValid(_handle));
    GP_ASSERT(_num >= count);
    bgfx::setUniform(_handle, &values[0].x, count);
}

void BGFXUniform::setValue(const Texture::Sampler* sampler)
{
    GP_ASSERT(getType() == UT_SAMPLER);
    GP_ASSERT(sampler);

    // Bind the sampler - this binds the texture and applies sampler state
    const_cast<Texture::Sampler*>(sampler)->bind(this);
}

void BGFXUniform::setValue(const Texture::Sampler** values, unsigned int count)
{
    GP_ASSERT(getType() == UT_SAMPLER);

    for (unsigned int i = 0; i < count; ++i)
    {
        GP_ASSERT(values[i]);

        // Bind the sampler - this binds the texture and applies sampler state

        //@@
        //const_cast<Texture::Sampler*>(values[i])->bind(this);
    }
}

} // end namespace gplay
