#include "../graphics/View.h"
#include "../renderer/BGFXRenderer.h"
#include <unordered_map>

namespace gplay {

static std::unordered_map<unsigned short, View*> __views;
unsigned short View::_curentViewId = 0;

unsigned short View::getCurrentViewId()
{
    return _curentViewId;
}

View* View::getView(unsigned short id)
{
    return __views[id];
}

void View::setViewOrder(std::vector<unsigned short>& views)
{
    if(views.size() == 0)
    {
        bgfx::setViewOrder();
    }
    else
    {
        bgfx::setViewOrder(views[0], views.size(), views.data());
    }
}

View::View() :
  _id(0)
  , _rectangle(Rectangle())
  , _clearFlags(ClearFlags::NONE)
  , _clearColor(0x00000000)
  , _depth(1.0f)
  , _stencil(0)
  , _name(0)
{
    uint16_t clearFlags = static_cast<uint16_t>(_clearFlags);
    bgfx::setViewClear(_id, clearFlags, _clearColor, _depth, _stencil);
    bgfx::setViewRect(_id, _rectangle.x, _rectangle.y, _rectangle.width, _rectangle.height);
}

View* View::create(unsigned short id, Rectangle rectangle, ClearFlags clearFlags, unsigned int clearColor, float depth, unsigned char stencil)
{
    View* view = new View();
    view->_id = id;
    view->_rectangle = rectangle;
    view->_clearFlags = clearFlags;
    view->_clearColor = clearColor;
    view->_depth = depth;
    view->_stencil = stencil;

    __views.insert(std::make_pair(id, view));

    return view;
}

void View::bind()
{
    _curentViewId = _id;

    uint16_t clearFlags = static_cast<uint16_t>(_clearFlags);
    bgfx::setViewClear(_id, clearFlags, _clearColor, _depth, _stencil);
    bgfx::setViewRect(_id, _rectangle.x, _rectangle.y, _rectangle.width, _rectangle.height);
}

void View::setViewRect(Rectangle rect)
{
    _rectangle = rect;
    bgfx::setViewRect(_id, rect.x, rect.y, rect.width, rect.height);
}

void View::setViewClear(unsigned int clearColor, float depth, unsigned char stencil)
{
    _clearColor = clearColor;
    _depth = depth;
    _stencil = stencil;
    bgfx::setViewClear(_id, clearColor, depth, stencil);
}

void View::setViewSortingMode(SortingMode viewMode)
{
    bgfx::ViewMode::Enum mode = static_cast<bgfx::ViewMode::Enum>(viewMode);
    bgfx::setViewMode(_id, mode);
}

}
