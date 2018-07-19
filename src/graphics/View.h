#ifndef VIEW_H
#define VIEW_H

#include "../math/Rectangle.h"
#include <vector>

namespace gplay {

/**
 * View is a sorting mechanism. View represent bucket of draw and compute calls.
 *
 * Compute and draw calls inside bucket are sorted in the way that all compute calls are executed before draw calls.
 * Compute calls are always in order of submission, while draw calls are sorted by internal state if view is not in
 * sequential mode. In the most of cases when z-buffer is used this change in order is not noticable to desired output.
 * In cases where order has to be preserved (for example in rendering GUIs), view can be set to be in sequential order.
 * Sequential order is less efficient, because it doesn’t allow state change optimization, and should be
 * avoided when possible.
 *
 * By default views ids are sorted in ascending order. For dynamic renderers where order might not be known until
 * the last moment, view ids can be remaped to arbitrary order by calling setViewOrder. View state is preserved
 * between multiple frames.
 */
class View
{
public:

    /**
     * @brief The ClearFlags enum
     */
    enum class ClearFlags : unsigned short
    {
        NONE    = 0x0000,
        COLOR   = 0x0001,
        DEPTH   = 0x0002,
        STENCIL = 0x0004,
        COLOR_DEPTH = COLOR | DEPTH,
        COLOR_STENCIL = COLOR | STENCIL,
        DEPTH_STENCIL = DEPTH | STENCIL,
        COLOR_DEPTH_STENCIL = COLOR | DEPTH | STENCIL
    };

    /**
     * @brief The SortingMode enum
     */
    enum class SortingMode
    {
        DEFAULT,            //!< Default sort order.
        SEQUENTIAL,         //!< Sort in the same order in which submit calls were called.
        DEPTH_ASCENDING,    //!< Sort draw call depth in ascending order.
        DEPTH_DESCENDING,   //!< Sort draw call depth in descending order.
    };

    /**
     * @brief Create a view. The view is added to an internal manager to get it later - see getView()
     * @param id The id of the view
     * @param rectangle The origin and size of the view.
     * @param clearFlags The clear flags - see ClearFlags enum.
     * @param color The clear color.
     * @param depth The depth value.
     * @param stencil The stencil value.
     * @return
     */
    static View* create(unsigned short id, Rectangle rectangle, ClearFlags clearFlags,
                        unsigned int color = 0x00000000, float depth = 1.0f, unsigned char stencil = 0);

    /**
     * @brief Bind the view for next draw calls.
     */
    void bind();

    /**
     * @brief Set the view rectangle.
     * @param rect The rectangle that represent the rendering area of the view.
     */
    void setViewRect(Rectangle rect);

    /**
     * @brief Set clear settings of the view.
     * @param clearColor The clear color.
     * @param depth The depth value.
     * @param stencil The stencil value.
     */
    void setViewClear(unsigned int clearColor, float depth = 1.0f, unsigned char stencil = 0);

    /**
     * @brief Set the view sorting mode.
     * @param viewMode The sorting mode.
     */
    void setViewSortingMode(SortingMode viewMode);


    /**
     * @brief Get a view from id.
     * @param id The id of the view.
     * @return The view.
     */
    static View* getView(unsigned short id);

    /**
     * @brief Get the current view id (the last binded view).
     * @return The id of the current binded view.
     */
    static unsigned short getCurrentViewId();

    /**
     * @brief View reordering
     * @param views View remap id table. Passing empty vector will reset view ids to default state.
     */
    static void setViewOrder(std::vector<unsigned short>& views);

    /**
     * @brief reset to default all views and clear cache
     */
    static void clearAll();

private:

    View();

    unsigned short _id;
    Rectangle _rectangle;
    ClearFlags _clearFlags;
    unsigned int _clearColor;
    float _depth;
    unsigned char _stencil;
    const char* _name;
    static unsigned short _curentViewId;
};

}

#endif // VIEW_H
