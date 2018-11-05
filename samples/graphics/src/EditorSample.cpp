#include <gplay-engine.h>
#include "Example.h"
#include "SamplesGame.h"
#include "FirstPersonCamera.h"

using namespace gplay;

class EditorSample : public Example
{
    FirstPersonCamera _fpCamera;
    Scene* _scene;

public:

    EditorSample()
        : _scene(nullptr)
    {
    }

    void finalize()
    {
        SAFE_RELEASE(_scene);
        Game::getInstance()->showEditor(nullptr);
    }

    void initialize()
    {
        // Create a new scene.
        _scene = Scene::load("res/data/samples/browser/sample.scene");

        // create a node hierarchy
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

        // visit all the nodes in the scene to bind lighting to materials
        _scene->visit(this, &EditorSample::initializeMaterials);

        // create fps camera and set as active camera
        Vector3 cameraPosition(0, 1, 5);
        _fpCamera.initialize(1.0, 10000.0f);
        _fpCamera.setPosition(cameraPosition);
        _scene->addNode(_fpCamera.getRootNode());
        _scene->setActiveCamera(_fpCamera.getCamera());
        _scene->getActiveCamera()->setAspectRatio(getAspectRatio());

        // show this scene in editor
        Game::getInstance()->showEditor(_scene);
    }


    bool initializeMaterials(Node* node)
    {
        Model* model = dynamic_cast<Model*>(node->getDrawable());
        if (model)
        {
            Material* material = model->getMaterial();
            MaterialParameter* colorParam = material->getParameter("u_directionalLightColor[0]");
            colorParam->setValue(Vector3(0.75f, 0.75f, 0.75f));

            Node* lightNode = _scene->findNode("directionalLight");
            if(lightNode)
            {
                Light* light = dynamic_cast<Light*>(lightNode->getLight());
                if(light)
                {
                    MaterialParameter* directionParam = material->getParameter("u_directionalLightDirection[0]");
                    directionParam->bindValue(lightNode, &Node::getForwardVectorView);
                }
            }

        }
        return true;
    }

    void update(float elapsedTime)
    {
        // update camera
        _fpCamera.updateCamera(elapsedTime);
    }

    void render(float elapsedTime)
    {
        // Clear the color and depth buffers
        clear(CLEAR_COLOR_DEPTH, 0.6f, 0.6f, 0.8f, 1.0f, 1.0f, 0);

        // Visit all the nodes in the scene, drawing the models.
        _scene->visit(this, &EditorSample::drawScene);
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

