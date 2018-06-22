#ifndef R_DYNAMIC_MESH_UPDATE_H_
#define R_DYNAMIC_MESH_UPDATE_H_

#include "gplay-engine.h"
#include "Example.h"

using namespace gplay;


class ImGuiTest : public Example
{
public:

    ImGuiTest();

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
};

#endif
