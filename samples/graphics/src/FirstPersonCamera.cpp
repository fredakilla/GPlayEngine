#include "FirstPersonCamera.h"

static const unsigned int MOVE_FORWARD = 1;
static const unsigned int MOVE_BACKWARD = 2;
static const unsigned int MOVE_LEFT = 4;
static const unsigned int MOVE_RIGHT = 8;
static const unsigned int MOVE_UP = 16;
static const unsigned int MOVE_DOWN = 32;
static const float MOVE_SPEED = 15.0f;
static const float UP_DOWN_SPEED = 10.0f;


FirstPersonCamera::FirstPersonCamera()
    : _pitchNode(NULL), _rootNode(NULL)
    , _moveFlags(0), _prevX(0), _prevY(0), _buttonPressed(false)
{
    
}

FirstPersonCamera::~FirstPersonCamera()
{
    SAFE_RELEASE(_pitchNode);
    SAFE_RELEASE(_rootNode);
}

void FirstPersonCamera::initialize(float nearPlane, float farPlane, float fov)
{
    SAFE_RELEASE(_pitchNode);
    SAFE_RELEASE(_rootNode);
    _rootNode = Node::create("FirstPersonCamera_root");
    _pitchNode = Node::create("FirstPersonCamera_pitch");
    _rootNode->addChild(_pitchNode);

    float aspectRatio = Game::getInstance()->getAspectRatio();
    assert(aspectRatio > 0.0f);
    Camera* camera = Camera::createPerspective(fov, aspectRatio, nearPlane, farPlane);
    _pitchNode->setCamera(camera);
    SAFE_RELEASE(camera);
}

Node* FirstPersonCamera::getRootNode()
{
    return _rootNode;
}

Camera* FirstPersonCamera::getCamera()
{
    if (_pitchNode)
        return _pitchNode->getCamera();
    return NULL;
}

void FirstPersonCamera::setPosition(const Vector3& position)
{
    _rootNode->setTranslation(position);
}

void FirstPersonCamera::moveForward(float amount)
{
    Vector3 v = _pitchNode->getForwardVectorWorld();
    v.normalize().scale(amount);
    _rootNode->translate(v);
}

void FirstPersonCamera::moveBackward(float amount)
{
    moveForward(-amount);
}

void FirstPersonCamera::moveLeft(float amount)
{
    _rootNode->translateLeft(amount);
}

void FirstPersonCamera::moveRight(float amount)
{
    _rootNode->translateLeft(-amount);
}

void FirstPersonCamera::moveUp(float amount)
{
    _rootNode->translateUp(amount);
}

void FirstPersonCamera::moveDown(float amount)
{
    _rootNode->translateUp(-amount);
}

void FirstPersonCamera::rotate(float yaw, float pitch)
{
    _rootNode->rotateY(-yaw);
    _pitchNode->rotateX(pitch);
}

void FirstPersonCamera::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
    {
        _prevX = x;
        _prevY = y;
        break;
    }
    case Touch::TOUCH_RELEASE:
    {
        _prevX = 0;
        _prevY = 0;
        break;
     }
    case Touch::TOUCH_MOVE:
    {
        int deltaX = x - _prevX;
        int deltaY = y - _prevY;
        _prevX = x;
        _prevY = y;
        float pitch = -MATH_DEG_TO_RAD(deltaY * 0.5f);
        float yaw = MATH_DEG_TO_RAD(deltaX * 0.5f);
        rotate(yaw, pitch);
        break;
    }
    };
}

void FirstPersonCamera::updateCamera(float elapsedTime)
{
    float time = (float)elapsedTime / 1000.0f;

    Vector2 move;

    if (_moveFlags != 0)
    {
        // Forward motion
        if (_moveFlags & MOVE_FORWARD)
        {
            move.y = 1;
        }
        else if (_moveFlags & MOVE_BACKWARD)
        {
            move.y = -1;
        }
        // Strafing
        if (_moveFlags & MOVE_LEFT)
        {
            move.x = 1;
        }
        else if (_moveFlags & MOVE_RIGHT)
        {
            move.x = -1;
        }
        move.normalize();

        // Up and down
        if (_moveFlags & MOVE_UP)
        {
            moveUp(time * UP_DOWN_SPEED);
        }
        else if (_moveFlags & MOVE_DOWN)
        {
            moveDown(time * UP_DOWN_SPEED);
        }
    }
    /*else if (_gamepad->getJoystickCount() > 0)
    {
        _gamepad->getJoystickValues(0, &move);
        move.x = -move.x;
    }
    if (_gamepad->getJoystickCount() > 1)
    {
        Vector2 joy2;
        _gamepad->getJoystickValues(1, &joy2);
        _fpCamera.rotate(MATH_DEG_TO_RAD(joy2.x * 2.0f), MATH_DEG_TO_RAD(joy2.y * 2.0f));
    }*/

    if (!move.isZero())
    {
        move.scale(time * MOVE_SPEED);
        moveForward(move.y);
        moveLeft(move.x);
    }

    /*if (!_buttonPressed && _gamepad->isButtonDown(Gamepad::BUTTON_A))
    {
        addSound("footsteps.wav");
    }
    _buttonPressed = _gamepad->isButtonDown(Gamepad::BUTTON_A);*/
}

void FirstPersonCamera::keyEvent(Keyboard::KeyEvent evt, int key)
{
    if (evt == Keyboard::KEY_PRESS)
    {
        switch (key)
        {
        case Keyboard::KEY_UP_ARROW:
        case Keyboard::KEY_W:
            _moveFlags |= MOVE_FORWARD;
            break;
        case Keyboard::KEY_DOWN_ARROW:
        case Keyboard::KEY_S:
            _moveFlags |= MOVE_BACKWARD;
            break;
        case Keyboard::KEY_LEFT_ARROW:
        case Keyboard::KEY_A:
            _moveFlags |= MOVE_LEFT;
            break;
        case Keyboard::KEY_RIGHT_ARROW:
        case Keyboard::KEY_D:
            _moveFlags |= MOVE_RIGHT;
            break;

        case Keyboard::KEY_Q:
            _moveFlags |= MOVE_DOWN;
            break;
        case Keyboard::KEY_E:
            _moveFlags |= MOVE_UP;
            break;
        case Keyboard::KEY_PG_UP:
            rotate(0, MATH_PIOVER4);
            break;
        case Keyboard::KEY_PG_DOWN:
            rotate(0, -MATH_PIOVER4);
            break;
        }
    }
    else if (evt == Keyboard::KEY_RELEASE)
    {
        switch (key)
        {
        case Keyboard::KEY_UP_ARROW:
        case Keyboard::KEY_W:
            _moveFlags &= ~MOVE_FORWARD;
            break;
        case Keyboard::KEY_DOWN_ARROW:
        case Keyboard::KEY_S:
            _moveFlags &= ~MOVE_BACKWARD;
            break;
        case Keyboard::KEY_LEFT_ARROW:
        case Keyboard::KEY_A:
            _moveFlags &= ~MOVE_LEFT;
            break;
        case Keyboard::KEY_RIGHT_ARROW:
        case Keyboard::KEY_D:
            _moveFlags &= ~MOVE_RIGHT;
            break;
        case Keyboard::KEY_Q:
            _moveFlags &= ~MOVE_DOWN;
            break;
        case Keyboard::KEY_E:
            _moveFlags &= ~MOVE_UP;
            break;
        }
    }
}

bool FirstPersonCamera::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
    switch (evt)
    {
    case Mouse::MOUSE_WHEEL:
        moveForward(wheelDelta * MOVE_SPEED / 2.0f );
        return true;
    }
    return false;
}

