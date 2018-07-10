#ifndef NEWGAME_H
#define NEWGAME_H

#include <gplay-engine.h>

using namespace gplay;

class MyNewGame : public Game
{
public:

    //! Constructor.
    MyNewGame();

protected:

    //! Initialize callback that is called just before the first frame when the game starts.
    void initialize() override;

    //!  Finalize callback that is called when the game on exits.
    void finalize() override;

    //! Called just before render. Ideal for non-render code and game logic such as input and animation.
    void update(float elapsedTime) override;

    //! Called just after update. Ideal for all rendering code.
    void render(float elapsedTime) override;

private:

    //! Internal method for drawing node when visiting scene.
    bool drawScene(Node* node);

    Scene* _scene;
    Sprite* _sprite;
};

#endif // NEWGAME_H
