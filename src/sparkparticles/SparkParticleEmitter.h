#ifndef GP_NO_SPARK

#pragma once

#include <spark/SPARK.h>
#include "../core/Base.h"
#include "../core/Ref.h"
#include "../graphics/Drawable.h"

namespace gplay {

class Mesh;

class SparkParticleEmitter : public Ref, public Drawable
{
    friend class Node;

public:

    static SparkParticleEmitter* create(SPK::Ref<SPK::System> sparkSystem, bool worldTransformed);
    static SparkParticleEmitter* createRef(SPK::Ref<SPK::System> sparkSystem, bool worldTransformed);


    unsigned int draw() override;
    void update(float dt);
    SPK::Ref<SPK::System> getSparkSystem();

private:

    /**
     * Constructor.
     */
    SparkParticleEmitter();

    /**
     * Destructor. Hidden use release() instead.
     */
    ~SparkParticleEmitter();

    /**
     * @see Drawable::clone
     */
    Drawable* clone(NodeCloneContext& context);


    void updateCameraPosition() const;


    SPK::Ref<SPK::System> _sparkSystem;
    bool _worldTransformed;
    bool _onlyWhenVisible;
    bool _alive;

};

}

#endif
