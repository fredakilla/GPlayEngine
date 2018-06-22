#include "../core/Base.h"
#include "../ui/Layout.h"
#include "../ui/Control.h"
#include "../ui/Container.h"
#include "../core/Game.h"

namespace gplay
{

bool Layout::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    return false;
}

}