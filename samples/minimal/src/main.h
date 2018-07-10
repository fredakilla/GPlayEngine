#ifndef MINIMAL_H
#define MINIMAL_H

#include <gplay-engine.h>

using namespace gplay;

class MinimalSample : public Game
{
public:

    //! Constructor.
    MinimalSample();

protected:

    //! Initialize callback that is called just before the first frame when the game starts.
    void initialize();

    //!  Finalize callback that is called when the game on exits.
    void finalize();

    //! Called just before render. Ideal for non-render code and game logic such as input and animation.
    void update(float elapsedTime);

    //! Called just after update. Ideal for all rendering code.
    void render(float elapsedTime);

private:

    //! Internal method for drawing node when visiting scene.
    bool drawScene(Node* node);

    Scene* _scene;

};

#endif // MINIMAL_H
