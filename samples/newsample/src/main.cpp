#include "main.h"

// Declare our game instance
MyNewGame game;


MyNewGame::MyNewGame() :
    _scene(nullptr)
{

}

void MyNewGame::initialize()
{
    // Create the scene.
    _scene = Scene::create();

    // Add a camera
    Camera* camera = Camera::createOrthographic(getWidth(), getHeight(), getAspectRatio(), 0.0, 1.0);
    Node* cameraNode = _scene->addNode("camera");
    cameraNode->setCamera(camera);
    _scene->setActiveCamera(camera);

    // Create a sprite
    _sprite = Sprite::create("res/core/icon.png");
    _sprite->setOffset(Sprite::OFFSET_VCENTER_HCENTER);

    // Add sprite to scene
    Node* node = _scene->addNode("logo");
    node->setTranslation(Vector3(0,0,0));
    node->setDrawable(_sprite);
}

void MyNewGame::finalize()
{
    SAFE_RELEASE(_sprite);
    SAFE_RELEASE(_scene);
}

void MyNewGame::update(float elapsedTime)
{

}

void MyNewGame::render(float elapsedTime)
{
    clear(ClearFlags::CLEAR_COLOR_DEPTH, Vector4::fromColor(0x4488aaff), 1.0f, 0);

    // Visit scene
    _scene->visit(this, &MyNewGame::drawScene);
}

bool MyNewGame::drawScene(Node* node)
{
    // if this node is a drawable, draw it.
    Drawable* drawable = dynamic_cast<Drawable*>(node->getDrawable());
    if (drawable)
        drawable->draw();
    return true;
}
