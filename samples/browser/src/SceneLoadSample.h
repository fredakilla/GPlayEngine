#ifndef SCENELOADSAMPLE_H_
#define SCENELOADSAMPLE_H_

#include "gplay-engine.h"
#include "Sample.h"

using namespace gplay;

/**
 * Sample for loading a scene from a .scene file.
 */
class SceneLoadSample : public Sample
{
public:

    SceneLoadSample();

    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

    void keyEvent(Keyboard::KeyEvent evt, int key);

protected:

    void initialize();

    void finalize();

    void update(float elapsedTime);

    void render(float elapsedTime);

private:

    bool initializeMaterials(Node* node);

    bool drawScene(Node* node);

    Font* _font;
    Scene* _scene;
};

#endif
