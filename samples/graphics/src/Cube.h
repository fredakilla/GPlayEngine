#ifndef R_GEOMETRY_H_
#define R_GEOMETRY_H_

#include "gplay-engine.h"
#include "Example.h"

using namespace gplay;


class CubeExample : public Example, Control::Listener
{
public:

    CubeExample();
    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

protected:

    void initialize();
    void finalize();
    void update(float elapsedTime);
    void render(float elapsedTime);

    void controlEvent(Control* control, EventType evt);

private:

    void setGeometry(bool dynamic);
    void setIndexedGeometry(bool dynamic);

    Form* _form;
    Font* _font;
    Model* _model;
    Material* _material;
    float _spinDirection;
    Matrix _worldViewProjectionMatrix;
};

#endif
