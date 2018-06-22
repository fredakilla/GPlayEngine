#include "../core/Base.h"
#include "../graphics/Drawable.h"
#include "../graphics/Node.h"


namespace gplay
{

Drawable::Drawable()
    : _node(NULL)
    , _mask(0)
{
}

Drawable::~Drawable()
{
}

Node* Drawable::getNode() const
{
    return _node;
}

void Drawable::setNode(Node* node)
{
    _node = node;
}

void Drawable::useMask(unsigned char mask)
{
    _mask |= mask;
}

bool Drawable::hasMask(unsigned char mask)
{
    return ((_mask & mask)) ? true : false;
}

}
