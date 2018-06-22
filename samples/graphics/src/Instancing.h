#ifndef R_INSTANCING_H_
#define R_INSTANCING_H_

#include "gplay-engine.h"
#include "Example.h"

using namespace gplay;


class Instancing : public Example, Control::Listener
{
public:

    Instancing();
    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

protected:

    void initialize();
    void initializeUI();
    void finalize();
    void update(float elapsedTime);
    void render(float elapsedTime);

    void controlEvent(Control* control, EventType evt);

private:

    void setColoredGeometry();
    void setTexturedGeometry();

    Form* _form;
    Font* _font;
    Model* _model;
    Material* _materialColored;
    Material* _materialTextured;
    Matrix _worldViewProjectionMatrix;
    float _rotValue;
    unsigned int _maxDimensions;
};

#endif
