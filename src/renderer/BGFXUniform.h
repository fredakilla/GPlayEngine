#ifndef BGFXUNIFORM_H
#define BGFXUNIFORM_H

#include "../core/Base.h"
#include "../graphics/Texture.h"

namespace gplay {

class Vector2;
class Vector3;
class Vector4;
class Matrix;
class Effect;

/**
 * Represents a uniform variable within an effect.
 */
class Uniform
{
    friend class Effect;

public:

    enum UniformType
    {
        UT_SAMPLER,             /// Int, used for samplers only.
        UT_VECTOR4,             /// 4 floats vector.
        UT_MATRIX3,             /// 3x3 matrix.
        UT_MATRIX4,             /// 4x4 matrix.
        UT_UNDEFINED
    };

    struct UniformInfo
    {
        std::string name;       /// Uniform name.
        UniformType type;       /// Uniform type.
        unsigned short num;     /// Number of elements in array.
    };


    /**
     * Returns the name of this uniform.
     *
     * @return The name of the uniform.
     */
    const char* getName() const;

    /**
     * Returns the uniform type.
     *
     * @return The uniform type.
     */
    const UniformType getType() const;

    /**
     * Returns the effect for this uniform.
     *
     * @return The uniform's effect.
     */
    Effect* getEffect() const;

    /**
     * Returns the sampler index for this uniform (only available for sampler uniform)
     *
     * @return The sampler index.
     */
    const unsigned int getIndex() const;

    /**
     * Sets a float uniform value.
     *
     * @param uniform The uniform to set.
     * @param value The float value to set.
     */
    virtual void setValue(float value) = 0;

    /**
     * Sets a float array uniform value.
     *
     * @param uniform The uniform to set.
     * @param values The array to set.
     * @param count The number of elements in the array.
     */
    virtual void setValue(const float* values, unsigned int count = 1) = 0;

    /**
     * Sets an integer uniform value.
     *
     * @param uniform The uniform to set.
     * @param value The value to set.
     */
    virtual void setValue(int value) = 0;

    /**
     * Sets an integer array uniform value.
     *
     * @param uniform The uniform to set.
     * @param values The array to set.
     * @param count The number of elements in the array.
     */
    virtual void setValue(const int* values, unsigned int count = 1) = 0;

    /**
     * Sets a matrix uniform value.
     *
     * @param uniform The uniform to set.
     * @param value The value to set.
     */
    virtual void setValue(const Matrix& value) = 0;

    /**
     * Sets a matrix array uniform value.
     *
     * @param uniform The uniform to set.
     * @param values The array to set.
     * @param count The number of elements in the array.
     */
    virtual void setValue(const Matrix* values, unsigned int count = 1) = 0;

    /**
     * Sets a vector uniform value.
     *
     * @param uniform The uniform to set.
     * @param value The value to set.
     */
    virtual void setValue(const Vector2& value) = 0;

    /**
     * Sets a vector array uniform value.
     *
     * @param uniform The uniform to set.
     * @param values The array to set.
     * @param count The number of elements in the array.
     */
    virtual void setValue(const Vector2* values, unsigned int count = 1) = 0;

    /**
     * Sets a vector uniform value.
     *
     * @param uniform The uniform to set.
     * @param value The value to set.
     */
    virtual void setValue(const Vector3& value) = 0;

    /**
     * Sets a vector array uniform value.
     *
     * @param uniform The uniform to set.
     * @param values The array to set.
     * @param count The number of elements in the array.
     */
    virtual void setValue(const Vector3* values, unsigned int count = 1) = 0;

    /**
     * Sets a vector uniform value.
     *
     * @param uniform The uniform to set.
     * @param value The value to set.
     */
    virtual void setValue(const Vector4& value) = 0;

    /**
     * Sets a vector array uniform value.
     *
     * @param uniform The uniform to set.
     * @param values The array to set.
     * @param count The number of elements in the array.
     */
    virtual void setValue(const Vector4* values, unsigned int count = 1) = 0;

    /**
     * Sets a sampler uniform value.
     *
     * @param uniform The uniform to set.
     * @param sampler The sampler to set.
     */
    virtual void setValue(const Texture::Sampler* sampler) = 0;

    /**
     * Sets a sampler array uniform value.
     *
     * @param uniform The uniform to set.
     * @param values The sampler array to set.
     * @param count The number of elements in the array.
     *
     * @script{ignore}
     */
    virtual void setValue(const Texture::Sampler** values, unsigned int count) = 0;

    /**
     * Constructor.
     */
    Uniform();

    /**
     * Copy constructor.
     */
    Uniform(const Uniform& copy);

    /**
     * Destructor.
     */
    ~Uniform();

    /**
     * Hidden copy assignment operator.
     */
    Uniform& operator=(const Uniform&);

private:

    std::string _name;
    UniformType _type;
    unsigned int _index;
    Effect* _effect;
};



/**
 * Unniform class specialized for bgfx.
 */
class BGFXUniform : public Uniform
{
public:
    BGFXUniform(const char * name, UniformType type, uint16_t num);
    ~BGFXUniform();

    void setValue(float value) override;
    void setValue(const float* values, unsigned int count = 1) override;
    void setValue(int value) override;
    void setValue(const int* values, unsigned int count = 1) override;
    void setValue(const Matrix& value) override;
    void setValue(const Matrix* values, unsigned int count = 1) override;
    void setValue(const Vector2& value) override;
    void setValue(const Vector2* values, unsigned int count = 1) override;
    void setValue(const Vector3& value) override;
    void setValue(const Vector3* values, unsigned int count = 1) override;
    void setValue(const Vector4& value) override;
    void setValue(const Vector4* values, unsigned int count = 1) override;
    void setValue(const Texture::Sampler* sampler) override;
    void setValue(const Texture::Sampler** values, unsigned int count) override;

    bgfx::UniformHandle getHandle() { return _handle; }

private:
    bgfx::UniformHandle _handle;    // uniform handle
    unsigned int _num;              // number of elements in array
};

} // end namespace gplay

#endif // BGFXUNIFORM_H
