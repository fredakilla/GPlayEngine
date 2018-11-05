#include <gplay-engine.h>
#include "Example.h"
#include "SamplesGame.h"
#include "FirstPersonCamera.h"
#include <dear-imgui/imgui.h>

using namespace gplay;


//---------------------------------------------------------------------------------
// DebugDrawing sample
//---------------------------------------------------------------------------------
class DebugDrawing : public Example
{
    FirstPersonCamera _fpCamera;
    Font* _font;
    Scene* _scene;


public:

    DebugDrawing()
        : _font(nullptr), _scene(nullptr)
    {
    }

    void finalize()
    {
        SAFE_RELEASE(_font);
        SAFE_RELEASE(_scene);
    }

    void initialize()
    {
        // Create the font for drawing the framerate.
        _font = Font::create("res/core/ui/arial.gpb");

        // Create a new empty scene.
        _scene = Scene::create();

        // set fps camera
        Vector3 cameraPosition(0, 1, 5);
        _fpCamera.initialize(1.0, 10000.0f);
        _fpCamera.setPosition(cameraPosition);
        _scene->addNode(_fpCamera.getRootNode());
        _scene->setActiveCamera(_fpCamera.getCamera());
        _scene->getActiveCamera()->setAspectRatio(getAspectRatio());

        // Load box shape
        Bundle* bundle = Bundle::create("res/data/scenes/box.gpb");
        Node* nodeBox = bundle->loadNode("box");
        dynamic_cast<Model*>(nodeBox->getDrawable())->setMaterial("res/data/materials/box.material", 0);
        SAFE_RELEASE(bundle);

        _scene->addNode(nodeBox);
    }

    void update(float elapsedTime)
    {
        // show toolbox
        showToolbox();

        // update camera
        _fpCamera.updateCamera(elapsedTime);
    }

    void render(float elapsedTime)
    {
        // Clear the color and depth buffers
        clear(CLEAR_COLOR_DEPTH, 0.1f, 0.1f, 0.2f, 1.0f, 1.0f, 0);

        // Visit all the nodes in the scene, drawing the models.
        _scene->visit(this, &DebugDrawing::drawScene);


        Matrix mat;
        //mat.scale(0.25f);
        //mat.translate(Vector3(1,1,0));
        //mat.rotateZ(MATH_DEG_TO_RAD(45.0f));

        DebugDraw::getInstance()->begin(_scene->getActiveCamera()->getViewProjectionMatrix());

        //debugDrawer.drawLine(Vector3(2,1,-5), Vector3(4,2,-10), Vector3(1,1,1), Vector3(1,0,0));
        //debugDrawer.drawBox(Vector3(-0.5,-0.5,-0.5), Vector3(0.5,0.5,0.5), Vector3(1,1,1));
        //debugDrawer.drawBox(Vector3(-1,-1,-1), Vector3(1,1,1), mat, Vector3(1,0,0));
        DebugDraw::getInstance()->drawSphere(Vector3(0,0,0), 1.0f, Vector3(1,1,1));
        //debugDrawer.drawCapsule(1.0f, 1.0f, 1, mat, Vector3(0,1,0));
        //debugDrawer.drawPlane(Vector3(0,1,0), 0.0f, Matrix::identity(), Vector3(1,1,1));
        //debugDrawer.drawCone(1.0f, 2.0f, 1, mat, Vector3(1,0.5,0.25));
        //debugDrawer.drawTransform(mat.tr, 10.0f);
        //debugDrawer.drawAabb(Vector3(0,0,0), Vector3(1,1,1), Vector3(1,1,1));
        //debugDrawer.drawBox(Vector3(-1,-1,-1), Vector3(1,1,1), mat, Vector3(1,1,0));
        //debugDrawer.drawSphere(1.0f, Vector3(0,0,0), mat, Vector3(1,1,1));

        DebugDraw::getInstance()->end();


        drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
    }

    bool drawScene(Node* node)
    {
        Drawable* drawable = node->getDrawable();
        if (drawable)
            drawable->draw();
        return true;
    }

    void showToolbox()
    {
        ImGui::Begin("Toolbox");
        ImGui::End();
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
    ADD_SAMPLE("Graphics", "DebugDrawing", DebugDrawing, 255);
#endif


