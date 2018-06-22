#pragma once

#include <spark/SPARK_Core.h>
#include <spark/Core/SPK_Renderer.h>
#include "../sparkparticles/SparkUtility.h"
#include "../graphics/Material.h"

namespace SPK {
namespace GP3D {

/**
 * The base renderer for all GPlay3D spark renderers
 */
class SparkBaseRenderer : public Renderer
{
public :

    virtual ~SparkBaseRenderer() {}

    virtual void setBlendMode(BlendMode blendMode) override;
   // void updateView(gplay::Camera* camera);
    gplay::Material* getMaterial();
    void setMaterial(gplay::Material* material);

    void setViewMatrix(gplay::Matrix viewMatrix) { _viewMatrix = viewMatrix; }

protected :

    SparkBaseRenderer(bool NEEDS_DATASET = false);  
    gplay::Material* _material;
    gplay::Matrix _viewMatrix;
};


}}
