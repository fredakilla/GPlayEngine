#include "Example.h"
#include "SamplesGame.h"

const Game::State& Example::UNINITIALIZED = Game::UNINITIALIZED;
const Game::State& Example::RUNNING = Game::RUNNING;
const Game::State& Example::PAUSED = Game::PAUSED;

const Game::ClearFlags& Example::CLEAR_COLOR = Game::CLEAR_COLOR;
const Game::ClearFlags& Example::CLEAR_DEPTH = Game::CLEAR_DEPTH;
const Game::ClearFlags& Example::CLEAR_STENCIL = Game::CLEAR_STENCIL;
const Game::ClearFlags& Example::CLEAR_COLOR_DEPTH = Game::CLEAR_COLOR_DEPTH;
const Game::ClearFlags& Example::CLEAR_COLOR_STENCIL = Game::CLEAR_COLOR_STENCIL;
const Game::ClearFlags& Example::CLEAR_DEPTH_STENCIL = Game::CLEAR_DEPTH_STENCIL;
const Game::ClearFlags& Example::CLEAR_COLOR_DEPTH_STENCIL = Game::CLEAR_COLOR_DEPTH_STENCIL;

Example::Example()
{
}

Example::~Example()
{
}

bool Example::isVsync()
{
    return Game::isVsync();
}

void Example::setVsync(bool enable)
{
    Game::setVsync(enable);
}

long Example::getAbsoluteTime()
{
    return Game::getAbsoluteTime();
}

long Example::getGameTime()
{
    return Game::getGameTime();
}

Game::State Example::getState() const
{
    return Game::getInstance()->getState();
}

int Example::run()
{
    return Game::getInstance()->run();
}

void Example::pause()
{
    Game::getInstance()->pause();
}

void Example::resume()
{
    Game::getInstance()->resume();
}

void Example::exit()
{
    Game::getInstance()->exit();
}

void Example::frame()
{
    Game::getInstance()->frame();
}

unsigned int Example::getFrameRate() const
{
    return Game::getInstance()->getFrameRate();
}

const Rectangle& Example::getViewport() const
{
    return Game::getInstance()->getViewport();
}

void Example::setViewport(const Rectangle& viewport)
{
	Game::getInstance()->setViewport(viewport);
}

unsigned int Example::getWidth() const
{
    return Game::getInstance()->getWidth();
}

unsigned int Example::getHeight() const
{
    return Game::getInstance()->getHeight();
}

float Example::getAspectRatio() const
{
    return Game::getInstance()->getAspectRatio();
}

void Example::clear(Game::ClearFlags flags, const Vector4& clearColor, float clearDepth, int clearStencil)
{
    Game::getInstance()->clear(flags, clearColor, clearDepth, clearStencil);
}

void Example::clear(Game::ClearFlags flags, float red, float green, float blue, float alpha, float clearDepth, int clearStencil)
{
    Game::getInstance()->clear(flags, red, green, blue, alpha, clearDepth, clearStencil);
}

AudioController* Example::getAudioController() const
{
    return Game::getInstance()->getAudioController();
}

AnimationController* Example::getAnimationController() const
{
    return Game::getInstance()->getAnimationController();
}

PhysicsController* Example::getPhysicsController() const
{
    return Game::getInstance()->getPhysicsController();
}

ScriptController* Example::getScriptController() const
{
	return Game::getInstance()->getScriptController();
}

void Example::displayKeyboard(bool display)
{
    Game::getInstance()->displayKeyboard(display);
}

void Example::keyEvent(Keyboard::KeyEvent evt, int key)
{    
}

void Example::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
}

bool Example::mouseEvent(Mouse::MouseEvent evt, int x, int y, int wheelDelta)
{
    return false;
}

bool Example::hasMouse()
{
    return Game::getInstance()->hasMouse();
}

bool Example::isMouseCaptured()
{
    return Game::getInstance()->isMouseCaptured();
}

void Example::setMouseCaptured(bool captured)
{
    Game::getInstance()->setMouseCaptured(captured);
}

void Example::setMultiTouch(bool enabled)
{
    Game::getInstance()->setMultiTouch(enabled);
}

bool Example::isMultiTouch() const
{
    return Game::getInstance()->isMultiTouch();
}

bool Example::hasAccelerometer() const
{
    return Game::getInstance()->hasAccelerometer();
}

void Example::getAccelerometerValues(float* pitch, float* roll)
{
    Game::getInstance()->getAccelerometerValues(pitch, roll);
}

void Example::getSensorValues(float* accelX, float* accelY, float* accelZ, float* gyroX, float* gyroY, float* gyroZ)
{
    Game::getInstance()->getSensorValues(accelX, accelY, accelZ, gyroX, gyroY, gyroZ);
}

void Example::schedule(long timeOffset, TimeListener* timeListener, void* cookie)
{
    Game::getInstance()->schedule(timeOffset, timeListener, cookie);
}

bool Example::isGestureSupported(Gesture::GestureEvent evt)
{
    return Game::getInstance()->isGestureSupported(evt);
}

void Example::registerGesture(Gesture::GestureEvent evt)
{
    Game::getInstance()->registerGesture(evt);
}

void Example::unregisterGesture(Gesture::GestureEvent evt)
{
    Game::getInstance()->unregisterGesture(evt);
}

bool Example::isGestureRegistered(Gesture::GestureEvent evt)
{
    return Game::getInstance()->isGestureRegistered(evt);
}

void Example::gestureSwipeEvent(int x, int y, int direction)
{
}

void Example::gesturePinchEvent(int x, int y, float scale)
{
}

void Example::gestureTapEvent(int x, int y)
{
}

void Example::gestureLongTapEvent(int x, int y, float duration)
{
}

void Example::gestureDragEvent(int x, int y)
{
}

void Example::gestureDropEvent(int x, int y)
{
}

void Example::gamepadEvent(Gamepad::GamepadEvent evt, Gamepad* gamepad)
{
}

unsigned int Example::getGamepadCount() const
{
    return Game::getInstance()->getGamepadCount();
}

Gamepad* Example::getGamepad(unsigned int index, bool preferPhysical) const
{
    return Game::getInstance()->getGamepad(index, preferPhysical);
}

void Example::drawFrameRate(Font* font, const Vector4& color, unsigned int x, unsigned int y, unsigned int fps)
{
    char buffer[10];
    sprintf(buffer, "%u", fps);
    font->start();
    font->drawText(buffer, x, y, color, 18);
    font->finish();
}
