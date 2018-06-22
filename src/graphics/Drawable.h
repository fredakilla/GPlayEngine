#ifndef DRAWABLE_H_
#define DRAWABLE_H_

namespace gplay
{

class Node;
class NodeCloneContext;

/**
 * Defines a drawable object that can be attached to a Node.
 */
class Drawable
{
    friend class Node;

public:

    /**
     * Constructor.
     */
    Drawable();

    /**
     * Destructor.
     */
    virtual ~Drawable();

    /**
     * Draws the object.
     *
     * @return The number of graphics draw calls required to draw the object.
     */

    virtual unsigned int draw() = 0;

    /**
     * Gets the node this drawable is attached to.
     *
     * @return The node this drawable is attached to.
     */
    Node* getNode() const;


    /**
     * Assign a bit mask. Can be usefull for conditionnal rendering.
     *
     * @param bitmask to set on.
     */
    void useMask(unsigned char mask);

    /**
     * Check if bitmask is used.
     *
     * @param bitmask to test.
     * @return true if bit is set and false if not set.
     */
    bool hasMask(unsigned char mask);

protected:

    /**
     * Clones the drawable and returns a new drawable.
     *
     * @param context The clone context.
     * @return The newly created drawable.
     */
    virtual Drawable* clone(NodeCloneContext& context) = 0;

    /**
     * Sets the node this drawable is attached to.
     *
     * @param node The node this drawable is attached to.
     */
    virtual void setNode(Node* node);

    /**
     * Node this drawable is attached to.
     */
    Node* _node;


    /**
     * Bit mask.
     */
    unsigned char _mask;
};

}

#endif
