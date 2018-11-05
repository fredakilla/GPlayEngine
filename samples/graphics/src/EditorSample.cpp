#include <gplay-engine.h>
#include "Example.h"
#include "SamplesGame.h"
#include "FirstPersonCamera.h"

using namespace gplay;

class EditorSample : public Example
{
    FirstPersonCamera _fpCamera;
    Font* _font;
    Scene* _scene;

public:

    EditorSample()
        : _font(nullptr), _scene(nullptr)
    {
    }

    void finalize()
    {
        SAFE_RELEASE(_font);
        SAFE_RELEASE(_scene);
        Game::getInstance()->showEditor(nullptr);
    }

    void initialize()
    {
        // Create the font for drawing the framerate.
        _font = Font::create("res/core/ui/arial.gpb");

        // Create a new empty scene.
        _scene = Scene::create();

        // create a node hierarchy for testing
        Node* n1 = Node::create("n1");
        Node* n2 = Node::create("n2");
        Node* n3 = Node::create("n3");
        Node* n4 = Node::create("n4");
        Node* n5 = Node::create("n5");
        Node* n6 = Node::create("n6");

        n2->addChild(n3);
        n2->addChild(n4);
        n4->addChild(n5);

        _scene->addNode(n1);
        _scene->addNode(n2);
        _scene->addNode(n6);
        _scene->addNode();


        // set fps camera
        Vector3 cameraPosition(0, 1, 5);
        _fpCamera.initialize(1.0, 10000.0f);
        _fpCamera.setPosition(cameraPosition);
        _scene->addNode(_fpCamera.getRootNode());
        _scene->setActiveCamera(_fpCamera.getCamera());
        _scene->getActiveCamera()->setAspectRatio(getAspectRatio());



        // load a box model

        Bundle* bundle = Bundle::create("res/data/scenes/box.gpb");
        Model* modelBox = Model::create(bundle->loadMesh("box_Mesh"));
        modelBox->setMaterial("res/data/materials/box.material");
        Node* nodeBox = Node::create("nodeBox");
        nodeBox->setDrawable(modelBox);
        SAFE_RELEASE(modelBox);
        _scene->addNode(nodeBox);

        nodeBox->setScale(2,1,3);
        nodeBox->setTranslation(1,2,-3);
        Quaternion q;
        Quaternion::createFromEulerAngles(Vector3(25,-42,126), &q);
        nodeBox->setRotation(q);

        SAFE_RELEASE(nodeBox);




        // show editor
        Game::getInstance()->showEditor(_scene);


        View::create(0, Rectangle(getWidth(), getHeight()), View::ClearFlags::COLOR_DEPTH, 0xaaaaffff);
    }

    void update(float elapsedTime)
    {
        // update camera
        _fpCamera.updateCamera(elapsedTime);
    }

    void render(float elapsedTime)
    {

        // Visit all the nodes in the scene, drawing the models.
        _scene->visit(this, &EditorSample::drawScene);

        drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
    }

    bool drawScene(Node* node)
    {
        Drawable* drawable = node->getDrawable();
        if (drawable)
            drawable->draw();
        return true;
    }

    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
    {
        _fpCamera.touchEvent(evt, x, y, contactIndex);

        switch (evt)
        {
        case Touch::TOUCH_PRESS:
            if (x < 75 && y < 50)
            {
                // Toggle Vsync if the user touches the top left corner
                setVsync(!isVsync());
            }
            break;
        case Touch::TOUCH_RELEASE:
            break;
        case Touch::TOUCH_MOVE:
            break;
        };
    }

    void keyEvent(Keyboard::KeyEvent evt, int key)
    {
        _fpCamera.keyEvent(evt, key);
    }

};

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Graphics", "InGame Editor", EditorSample, 255);
#endif

