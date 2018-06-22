#ifndef R_RENDERTOTEXTURE_H_
#define R_RENDERTOTEXTURE_H_

#include "gplay-engine.h"
#include "Example.h"

using namespace gplay;


class RenderToTexture : public Example
{
public:

    RenderToTexture();

    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

protected:

    void initialize();
    void finalize();
    void update(float elapsedTime);
    void render(float elapsedTime);

private:

    Font* _font;
    Model* _triangleModel;
    float _spinDirection;
    Matrix _worldViewProjectionMatrix;
    Model* _cubeModel;
    FrameBuffer * _frameBuffer;
};

#endif
