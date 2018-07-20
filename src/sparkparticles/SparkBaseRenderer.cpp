#ifndef GP_NO_SPARK

#include "../sparkparticles/SparkBaseRenderer.h"

namespace SPK {
namespace GP3D {

SparkBaseRenderer::SparkBaseRenderer(bool NEEDS_DATASET) :
    Renderer(NEEDS_DATASET)
{
    /*_camera = nullptr;
    _material = nullptr;*/
}

void SparkBaseRenderer::setBlendMode(BlendMode blendMode)
{
    switch(blendMode)
    {
    case BLEND_MODE_NONE :
        break;

    case BLEND_MODE_ADD :
        break;

    case BLEND_MODE_ALPHA :
        break;

    default :
        SPK_LOG_WARNING("Unsupported blending mode. Nothing happens");
        break;
    }
}

/*void SparkBaseRenderer::updateView(gplay::Camera* camera)
{
    _camera = camera;
}*/

gplay::Material* SparkBaseRenderer::getMaterial()
{
    return _material;
}

void SparkBaseRenderer::setMaterial(gplay::Material* material)
{
    _material =  material;
}


}}

#endif
