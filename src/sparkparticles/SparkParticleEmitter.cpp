#ifndef GP_NO_SPARK

#include "../sparkparticles/SparkParticleEmitter.h"
#include "../graphics/Scene.h"
#include "../sparkparticles/SparkBaseRenderer.h"

namespace gplay {


SparkParticleEmitter::SparkParticleEmitter() : Drawable()
{
    _worldTransformed = false;
    _onlyWhenVisible = false;
    _alive = true;
}

SparkParticleEmitter::~SparkParticleEmitter()
{

}

SparkParticleEmitter* SparkParticleEmitter::create(SPK::Ref<SPK::System> sparkSystem, bool worldTransformed)
{
    SparkParticleEmitter* sparkParticleEmitter = new SparkParticleEmitter();
    sparkParticleEmitter->_sparkSystem = SPK::SPKObject::copy(sparkSystem);
    sparkParticleEmitter->_worldTransformed = worldTransformed;
    return sparkParticleEmitter;
}

SparkParticleEmitter* SparkParticleEmitter::createRef(SPK::Ref<SPK::System> sparkSystem, bool worldTransformed)
{
    SparkParticleEmitter* sparkParticleEmitter = new SparkParticleEmitter();
    sparkParticleEmitter->_sparkSystem = sparkSystem;
    sparkParticleEmitter->_worldTransformed = worldTransformed;
    return sparkParticleEmitter;
}

Drawable* SparkParticleEmitter::clone(NodeCloneContext& context)
{
	return nullptr;
}

void SparkParticleEmitter::updateCameraPosition() const
{
    if(_node && _node->getScene())
    {
        Camera* camera = _node->getScene()->getActiveCamera();

        Matrix invTransform;
        invTransform = camera->getInverseViewMatrix();

        SPK::GP3D::SparkBaseRenderer* renderer = nullptr;

        for (size_t i=0; i<_sparkSystem->getNbGroups(); ++i)
        {
            // set view matrix for renderer
            renderer = reinterpret_cast<SPK::GP3D::SparkBaseRenderer*>(_sparkSystem->getGroup(i)->getRenderer().get());
            GP_ASSERT(renderer);

            // bind view projection matrix to shader
            renderer->getMaterial()->getParameter("u_worldViewProjectionMatrix")->setValue(camera->getViewProjectionMatrix());

            // store inverse view matrix to compute face orientation when rendering particles
            renderer->setViewMatrix(invTransform);

            // set spark camera position when group do sorting or distance computation
            if (_sparkSystem->getGroup(i)->isDistanceComputationEnabled())
            {
                Vector3 pos = camera->getNode()->getTranslationWorld();
                _sparkSystem->setCameraPosition(SPK::GP3D::gp3d2spk(pos));
            }
        }
    }
}


void SparkParticleEmitter::update(float dt)
{
    if(_node && _node->isEnabled() && !_onlyWhenVisible)
    {
        if (_worldTransformed)
        {
            // move spark system using node transformation
            _sparkSystem->getTransform().set(_node->getWorldMatrix().m);
        }

        updateCameraPosition();

        _alive = _sparkSystem->updateParticles(dt * 0.001f);
    }
}

unsigned int SparkParticleEmitter::draw()
{
    if(_alive)
    {
        _sparkSystem->renderParticles();
        return 1;
    }

    return 0;
}

SPK::Ref<SPK::System> SparkParticleEmitter::getSparkSystem()
{
    return _sparkSystem;
}

}

#endif
