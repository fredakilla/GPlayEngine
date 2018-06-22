#include "../core/Base.h"
#include "../ui/Control.h"
#include "../ui/AbsoluteLayout.h"
#include "../ui/Container.h"

namespace gplay
{

static AbsoluteLayout* __instance;

AbsoluteLayout::AbsoluteLayout()
{
}

AbsoluteLayout::~AbsoluteLayout()
{
    __instance = NULL;
}

AbsoluteLayout* AbsoluteLayout::create()
{
    if (!__instance)
    {
        __instance = new AbsoluteLayout();
    }
    else
    {
        __instance->addRef();
    }

    return __instance;
}

Layout::Type AbsoluteLayout::getType()
{
    return Layout::LAYOUT_ABSOLUTE;
}

void AbsoluteLayout::update(const Container* container)
{
    // Nothing to do for absolute layout
}

}
