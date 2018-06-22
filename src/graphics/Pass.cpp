#include "../core/Base.h"
#include "../graphics/Pass.h"
#include "../graphics/Technique.h"
#include "../graphics/Material.h"
#include "../graphics/Node.h"

namespace gplay
{

Pass::Pass(const char* id, Technique* technique) :
    _id(id ? id : ""), _technique(technique), _effect(NULL)
{
    RenderState::_parent = _technique;
}

Pass::~Pass()
{
    SAFE_RELEASE(_effect);
}

Pass* Pass::create(Effect* effect, const char* id)
{
    GP_ASSERT(effect);

    Pass* pass = new Pass(id, nullptr);
    pass->_effect = effect;
    effect->addRef();

    return pass;
}

bool Pass::initialize(const char* vshPath, const char* fshPath, const char* defines)
{
    GP_ASSERT(vshPath);
    GP_ASSERT(fshPath);

    SAFE_RELEASE(_effect);

    // Attempt to create/load the effect.
    _effect = Effect::createFromFile(vshPath, fshPath, defines);
    if (_effect == NULL)
    {
        GP_WARN("Failed to create effect for pass. vertexShader = %s, fragmentShader = %s, defines = %s", vshPath, fshPath, defines ? defines : "");
        return false;
    }

    return true;
}

const char* Pass::getId() const
{
    return _id.c_str();
}

Effect* Pass::getEffect() const
{
    return _effect;
}

void Pass::bind(Mesh::PrimitiveType primitiveType)
{
    GP_ASSERT(_effect);

    // Bind our effect.
    _effect->bind();

    // Bind our render state
    RenderState::bind(this, primitiveType);
}

void Pass::unbind()
{
    BGFXRenderer::getInstance().submit(_effect->getGpuProgram());
}

Pass* Pass::clone(Technique* technique, NodeCloneContext &context) const
{
    GP_ASSERT(_effect);
    _effect->addRef();

    Pass* pass = new Pass(getId(), technique);
    pass->_effect = _effect;

    RenderState::cloneInto(pass, context);
    pass->_parent = technique;
    return pass;
}

void Pass::setEffect(Effect* effect)
{
    GP_ASSERT(effect);

    if(_effect)
        SAFE_RELEASE(_effect);

    _effect = effect;
}

bool Pass::reload()
{
    return _effect->getGpuProgram()->reload();
}

}
