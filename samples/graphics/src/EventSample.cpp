#include "gplay-engine.h"
#include "Example.h"
#include "SamplesGame.h"
#include "FirstPersonCamera.h"
#include <dear-imgui/imgui.h>

#include "events/EventManager.h"
#include "events/BaseEventData.h"

using namespace gplay;



/**
 * Declare an event.
 *
 * This event store a Vector2 for mouse position.
 * And is intended to be triggered when mouse is clicked.
 */
using MyMouseEventRef = std::shared_ptr<class MyMouseEvent>;
class MyMouseEvent : public EventData
{
public:

    GP_DECLARE_EVENT(MyMouseEvent)

    Vector2 _mousePos;

    static MyMouseEventRef create(Vector2 mousePos)
    {
        print("MyMouseEvent::create\n");
        return MyMouseEventRef(new MyMouseEvent(mousePos));
    }

    ~MyMouseEvent()
    {
        print("MyMouseEvent::Destructor.\n");
    }

private:

    explicit MyMouseEvent(Vector2 mousePos) :
        EventData()
      , _mousePos(mousePos)
    {
        print("MyMouseEvent::Constructor\n");
    }
};




/**
 * An actor that wants to be notified by event.
 */
class DummyActor
{
public:

    DummyActor()
    {
        _pos = Vector2(0,0);
    }

    void onEventMouseClicked(EventDataRef eventData)
    {
        // this method was declared in initialisation as a listener for the MyMouseEvent event.

        // MyMouseEvent have been queued or triggered.
        // dynamic_cast the shared_ptr to get access to MyMouseEvent data.
        auto mouseEvent = std::dynamic_pointer_cast<MyMouseEvent>(eventData);

        print("DummyActor: oh the mouse was clicked at (%.0f,%.0f) !!!\n", mouseEvent->_mousePos.x, mouseEvent->_mousePos.y);

        // Do something usefull with that.

        _pos = mouseEvent->_mousePos;
    }

    void update()
    {
        ImGui::SetNextWindowPos(ImVec2(_pos.x, _pos.y));
        ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoNav);
        ImGui::Text("Hello cube, I'm DummyActor");
        ImGui::End();
    }

private:

    Vector2 _pos;

};






/**
 * Sample to demonstrate events communication system.
 */
class EventSample : public Example
{
    FirstPersonCamera _fpCamera;
    Font* _font;
    Scene* _scene;
    Node* _originalBox;
    DummyActor _dummyActor;

public:

    EventSample()
        : _font(nullptr), _scene(nullptr)
    {
    }    

    void finalize()
    {
        // Remove the delegates from the event manager
        EventManager::getInstance()->removeListener(GP_EVENT_LISTENER(this, EventSample::onEventCreateNewActor), MyMouseEvent::ID());
        EventManager::getInstance()->removeListener(GP_EVENT_LISTENER(&_dummyActor, DummyActor::onEventMouseClicked), MyMouseEvent::ID());

        SAFE_RELEASE(_font);
        SAFE_RELEASE(_scene);
        SAFE_RELEASE(_originalBox);
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


        // Load a box shape and create the original box node but don't add it to scene.
        // It will be used as base node for creating clones.
        Bundle * bundle = Bundle::create("res/data/scenes/box.gpb");
        _originalBox = bundle->loadNode("box");
        dynamic_cast<Model*>(_originalBox->getDrawable())->setMaterial("res/data/materials/box.material", 0);
        SAFE_RELEASE(bundle);

        // Add some listeners for the event MyMouseEvent.
        // This event is triggered in touchEvent() when a button is clicked.
        // Any object that wants to be notified of this event can add a listener.
        EventManager::getInstance()->addListener(GP_EVENT_LISTENER(this, EventSample::onEventCreateNewActor), MyMouseEvent::ID());
        EventManager::getInstance()->addListener(GP_EVENT_LISTENER(&_dummyActor, DummyActor::onEventMouseClicked), MyMouseEvent::ID());
    }

    void onEventCreateNewActor(EventDataRef eventData)
    {
        // this method was declared in initialisation as a listener for the MyMouseEvent event.

        // MyMouseEvent have been queued or triggered.
        // dynamic_cast the shared_ptr to get access to MyMouseEvent data.
        auto mouseEvent = std::dynamic_pointer_cast<MyMouseEvent>(eventData);

        if(!mouseEvent)
            return;

        // Find the position where the pick ray intersects with the floor (invisible plane on XZ axis).

        Vector3 position;
        Ray ray;
        Camera* camera = _scene->getActiveCamera();
        camera->pickRay(getViewport(), mouseEvent->_mousePos.x, mouseEvent->_mousePos.y, &ray);

        float distance = ray.intersects(Plane(0, 1, 0, -0.5f));
        if (distance != Ray::INTERSECTS_NONE)
        {
            position = Vector3((ray.getDirection() * distance) + ray.getOrigin());
        }

        // clone the box and add it to scene.
        Node* newBox = _originalBox->clone();
        newBox->setTranslation(position);
        _scene->addNode(newBox);

        print("onEventCreateNewActor => Create a new actor at mousePos(%.0fx%.0f) : 3DPos(%f,%f,%f)\n",
              mouseEvent->_mousePos.x,
              mouseEvent->_mousePos.y,
              position.x,
              position.y,
              position.z);
    }

    void update(float elapsedTime)
    {
        // show toolbox
        showToolbox();

        _dummyActor.update();

        // update camera
        _fpCamera.updateCamera(elapsedTime);
    }

    void render(float elapsedTime)
    {
        // Clear the color and depth buffers
        clear(CLEAR_COLOR_DEPTH, 0.1f, 0.1f, 0.2f, 1.0f, 1.0f, 0);

        // Visit all the nodes in the scene, drawing the models.
        _scene->visit(this, &EventSample::drawScene);

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

            // Send event that mouse was clicked, don't care about if someone is waiting this event or not.
            EventManager::getInstance()->queueEvent(MyMouseEvent::create(Vector2(x,y)));

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
    ADD_SAMPLE("Events", "Event demo", EventSample, 255);
#endif

