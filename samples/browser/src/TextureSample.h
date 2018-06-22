#ifndef TEXTURESAMPLE_H_
#define TEXTURESAMPLE_H_

#include "gplay-engine.h"
#include "Sample.h"

using namespace gplay;

/**
 * Sample the creating and drawing with textures with different modes.
 */
class TextureSample : public Sample
{
public:

    TextureSample();

    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex);

protected:

    void initialize();

    void finalize();

    void update(float elapsedTime);

    void render(float elapsedTime);

private:

    bool drawScene(Node* node);

    Font* _font;
    Scene* _scene;
    float _zOffset;
};

#endif
