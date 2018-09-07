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
    if(__views.count(id)>0)
        return __views.at(id);
    else
        return nullptr;
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

void View::clearAll()
{
    for(auto it : __views)
    {
        View* view = it.second;
        bgfx::resetView(view->_id);
        delete view;
    }

    __views.clear();
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
    View* view = nullptr;

    // search if the view id already exists in map
    size_t existingView = __views.count(id);
    if(existingView == 0)
    {
        // create a new view
        view = new View();
    }
    else
    {
        // reset existing view
        view = __views.at(id);
        bgfx::resetView(id);
    }

    // setup view
    view->_id = id;
    view->_rectangle = rectangle;
    view->_clearFlags = clearFlags;
    view->_clearColor = clearColor;
    view->_depth = depth;
    view->_stencil = stencil;


    uint16_t flags = static_cast<uint16_t>(clearFlags);
    bgfx::setViewClear(id, flags, clearColor, depth, stencil);
    bgfx::setViewRect(id, rectangle.x, rectangle.y, rectangle.width, rectangle.height);


    // if a new view add top map
    if(existingView == 0)
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
