#ifndef EFFECT_H_
#define EFFECT_H_

#include "../core/Ref.h"
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix.h"
#include "../graphics/Texture.h"
#include "../renderer/Renderer.h"

namespace gplay {

class Uniform;

/**
 * Defines an effect which can be applied during rendering.
 *
 * An effect essentially wraps an OpenGL program object, which includes the
 * vertex and fragment shader.
 *
 * In the future, this class may be extended to support additional logic that
 * typical effect systems support, such as GPU render state management,
 * techniques and passes.
 */
class Effect: public Ref
{
public:

    /**
     * Creates an effect using the specified vertex and fragment shader.
     *
     * @param vshPath The path to the vertex shader file.
     * @param fshPath The path to the fragment shader file.
     * @param defines A new-line delimited list of preprocessor defines. May be NULL.
     * 
     * @return The created effect.
     */
    static Effect* createFromFile(const char* vshPath, const char* fshPath, const char* defines = NULL, bool useCache = true);

    /**
     * Returns the unique string identifier for the effect, which is a concatenation of
     * the shader paths it was loaded from.
     */
    const char* getId() const;

    /**
     * Returns the uniform handle for the uniform with the specified name.
     *
     * @param name The name of the uniform to return.
     * 
     * @return The uniform, or NULL if no such uniform exists.
     */
    Uniform* getUniform(const char* name) const;

    /**
     * Returns the specified active uniform.
     * 
     * @param index The index of the uniform to return.
     * 
     * @return The uniform, or NULL if index is invalid.
     */
    Uniform* getUniform(unsigned int index) const;

    /**
     * Returns the number of active uniforms in this effect.
     * 
     * @return The number of active uniforms.
     */
    unsigned int getUniformCount() const;

    /**
     * Binds this effect to make it the currently active effect for the rendering system.
     */
    void bind();

    /**
     * Returns the currently bound effect for the rendering system.
     *
     * @return The currently bound effect, or NULL if no effect is currently bound.
     */
    static Effect* getCurrentEffect();

private:

    /**
     * Hidden constructor (use createEffect instead).
     */
    Effect();

    /**
     * Hidden destructor (use destroyEffect instead).
     */
    ~Effect();

    /**
     * Hidden copy assignment operator.
     */
    Effect& operator=(const Effect&);


    std::string _id;
    mutable std::map<std::string, Uniform*> _uniforms;


//@@
public:
    const BGFXGpuProgram * getGpuProgram() const;
    BGFXGpuProgram* getGpuProgram();
    static void initialize();
    static Effect* GetInvalidEffect();

private:
    BGFXGpuProgram * _gpuProgram;
    static Effect* _invalidEffect;
};

}

#endif
