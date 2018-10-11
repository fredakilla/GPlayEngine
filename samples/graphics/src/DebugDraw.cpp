#include <gplay-engine.h>
#include "Example.h"
#include "SamplesGame.h"
#include "FirstPersonCamera.h"
#include <imgui/imgui.h>

using namespace gplay;

#include <LinearMath/btIDebugDraw.h>

/**
 * Use the btIDebugDraw interface for generating shapes using bullet DebugDraw and rendering with gplay-engine.
 * You shouln't use this class directly to avoid to use bullet types prefer the DebugDrawer class below that use gplay types.
 */
class BulletDebugDraw : public btIDebugDraw
{
public:

    BulletDebugDraw()
    {
        Effect* effect = Effect::createFromFile("res/core/shaders/physics.vert", "res/core/shaders/physics.frag");

        Material* material = Material::create(effect);
        GP_ASSERT(material && material->getStateBlock());
        material->getStateBlock()->setDepthTest(true);
        material->getStateBlock()->setDepthFunction(RenderState::DEPTH_LEQUAL);

        VertexFormat::Element elements[] =
        {
            VertexFormat::Element(VertexFormat::POSITION, 3),
            VertexFormat::Element(VertexFormat::COLOR, 4),
        };
        _meshBatch = MeshBatch::create(VertexFormat(elements, 2), Mesh::LINES, material, false, 4096, 4096);
        SAFE_RELEASE(material);
        SAFE_RELEASE(effect);
    }

    ~BulletDebugDraw()
    {
        SAFE_DELETE(_meshBatch);
    }

    void begin(const Matrix& viewProjection)
    {
        GP_ASSERT(_meshBatch);
        _meshBatch->start();
        _meshBatch->getMaterial()->getParameter("u_viewProjectionMatrix")->setValue(viewProjection);
    }

    void end()
    {
        GP_ASSERT(_meshBatch && _meshBatch->getMaterial());
        _meshBatch->finish();
        _meshBatch->draw();
        _lineCount = 0;
    }

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override
    {
        GP_ASSERT(_meshBatch);

        static BulletDebugDraw::DebugVertex vertices[2];

        vertices[0].x = from.getX();
        vertices[0].y = from.getY();
        vertices[0].z = from.getZ();
        vertices[0].r = fromColor.getX();
        vertices[0].g = fromColor.getY();
        vertices[0].b = fromColor.getZ();
        vertices[0].a = 1.0f;

        vertices[1].x = to.getX();
        vertices[1].y = to.getY();
        vertices[1].z = to.getZ();
        vertices[1].r = toColor.getX();
        vertices[1].g = toColor.getY();
        vertices[1].b = toColor.getZ();
        vertices[1].a = 1.0f;

        _meshBatch->add(vertices, 2);

        ++_lineCount;
        if (_lineCount >= 4096)
        {
            // Flush the batch when it gets full (don't want to to grow infinitely)
            end();
            _meshBatch->start();
        }
    }

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
    {
        drawLine(from, to, color, color);
    }

    static const btTransform toBtTransform(const Matrix& matrix)
    {
        Vector3 s;      // scale is not used in btTransfrom, shape will no be scaled.
        Quaternion r;
        Vector3 t;
        matrix.decompose(&s, &r, &t);

        /*btMatrix3x3 btMat;
        btMat.setFromOpenGLSubMatrix(&matrix.m[0]);
        //btMat.setRotation(BQ(r));
        //btMat.scaled(BV(s));
        return btTransform(btMat, BV(t));*/

        return btTransform(BQ(r), BV(t));
    }

    virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) override {}
    virtual void reportErrorWarning(const char* warningString) override {}
    virtual void draw3dText(const btVector3& location,const char* textString) override {}
    virtual void setDebugMode(int debugMode) override {}
    virtual int	getDebugMode() const override {}

private:

    struct DebugVertex
    {
        float x;
        float y;
        float z;
        float r;
        float g;
        float b;
        float a;
    };

    MeshBatch* _meshBatch;
    int _lineCount;
};



/**
 * Wrapper to use the BulletDebugDraw with gplay types.
 */
class DebugDrawer
{
private:
    BulletDebugDraw dd;

public:
    void begin(const Matrix& viewProjection);
    void end();
    void drawLine(const Vector3& from, const Vector3& to, const Vector3& color);
    void drawLine(const Vector3& from, const Vector3& to, const Vector3& fromColor, const Vector3& toColor);
    void drawBox(const Vector3& bbMin, const Vector3& bbMax, const Vector3& color);
    void drawBox(const Vector3& bbMin, const Vector3& bbMax, const Matrix& matrix, const Vector3& color);
    void drawTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& color);
    void drawSphere(float radius, const Matrix& matrix, const Vector3& color);
    void drawSphere(const Vector3& p, float radius, const Vector3& color);
    void drawCapsule(float radius, float halfHeight, int upAxis, const Matrix& matrix, const Vector3& color);
    void drawCylinder(float radius, float halfHeight, int upAxis, const Matrix& matrix, const Vector3& color);
    void drawCone(float radius, float height, int upAxis, const Matrix& matrix, const Vector3& color);
    void drawPlane(const Vector3& planeNormal, float planeConst, const Matrix& matrix, const Vector3 &color);
    void drawArc(const Vector3& center, const Vector3& normal, const Vector3& axis, float radiusA, float radiusB, float minAngle, float maxAngle,
                 const Vector3& color, bool drawSect, float stepDegrees = float(10.f));
    void drawTransform(const Matrix& matrix, float orthoLen);
    void drawAabb(const Vector3& from, const Vector3& to, const Vector3& color);
};


void DebugDrawer::begin(const Matrix& viewProjection)
{
    dd.begin(viewProjection);
}

void DebugDrawer::end()
{
    dd.end();
}

void DebugDrawer::drawLine(const Vector3& from, const Vector3& to, const Vector3& color)
{
    dd.drawLine(BV(from), BV(to), BV(color), BV(color));
}

void DebugDrawer::drawLine(const Vector3& from, const Vector3& to, const Vector3& fromColor, const Vector3& toColor)
{
    dd.drawLine(BV(from), BV(to), BV(fromColor), BV(toColor));
}

void DebugDrawer::drawBox(const Vector3& bbMin, const Vector3& bbMax, const Vector3& color)
{
    dd.drawBox(BV(bbMin), BV(bbMax), BV(color));
}

void DebugDrawer::drawBox(const Vector3& bbMin, const Vector3& bbMax, const Matrix& matrix, const Vector3& color)
{
    dd.drawBox(BV(bbMin), BV(bbMax), BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& color)
{
    drawLine(v0,v1,color);
    drawLine(v1,v2,color);
    drawLine(v2,v0,color);
}

void DebugDrawer::drawSphere(float radius, const Matrix& matrix, const Vector3& color)
{
    dd.drawSphere(radius, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawSphere(const Vector3& p, float radius, const Vector3& color)
{
    dd.drawSphere(BV(p), radius, BV(color));
}

void DebugDrawer::drawCapsule(float radius, float halfHeight, int upAxis, const Matrix& matrix, const Vector3& color)
{
    dd.drawCapsule(radius, halfHeight, upAxis, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawCylinder(float radius, float halfHeight, int upAxis, const Matrix& matrix, const Vector3& color)
{
    dd.drawCylinder(radius, halfHeight, upAxis, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawCone(float radius, float height, int upAxis, const Matrix& matrix, const Vector3& color)
{
    dd.drawCone(radius, height, upAxis, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawPlane(const Vector3& planeNormal, float planeConst, const Matrix& matrix, const Vector3& color)
{
    dd.drawPlane(BV(planeNormal), planeConst, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawer::drawArc(const Vector3& center, const Vector3& normal, const Vector3& axis, float radiusA, float radiusB, float minAngle, float maxAngle,
             const Vector3& color, bool drawSect, float stepDegrees)
{
    dd.drawArc(BV(center), BV(normal), BV(axis), radiusA, radiusB, minAngle, maxAngle, BV(color), drawSect, stepDegrees);
}

void DebugDrawer::drawTransform(const Matrix& matrix, float orthoLen)
{
    dd.drawTransform(BulletDebugDraw::toBtTransform(matrix), orthoLen);
}

void DebugDrawer::drawAabb(const Vector3& from, const Vector3& to, const Vector3& color)
{
    dd.drawAabb(BV(from), BV(to), BV(color));
}





//---------------------------------------------------------------------------------
// DebugDrawing sample
//---------------------------------------------------------------------------------
class DebugDrawing : public Example
{
    FirstPersonCamera _fpCamera;
    Font* _font;
    Scene* _scene;

    DebugDrawer debugDrawer;
    //GPDebugDrawer dd;


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

        debugDrawer.begin(_scene->getActiveCamera()->getViewProjectionMatrix());

        //debugDrawer.drawLine(Vector3(2,1,-5), Vector3(4,2,-10), Vector3(1,1,1), Vector3(1,0,0));
        //debugDrawer.drawBox(Vector3(-0.5,-0.5,-0.5), Vector3(0.5,0.5,0.5), Vector3(1,1,1));
        //debugDrawer.drawBox(Vector3(-1,-1,-1), Vector3(1,1,1), mat, Vector3(1,0,0));
        debugDrawer.drawSphere(Vector3(0,0,0), 1.0f, Vector3(1,1,1));
        //debugDrawer.drawCapsule(1.0f, 1.0f, 1, mat, Vector3(0,1,0));
        //debugDrawer.drawPlane(Vector3(0,1,0), 0.0f, Matrix::identity(), Vector3(1,1,1));
        //debugDrawer.drawCone(1.0f, 2.0f, 1, mat, Vector3(1,0.5,0.25));
        //debugDrawer.drawTransform(mat.tr, 10.0f);
        //debugDrawer.drawAabb(Vector3(0,0,0), Vector3(1,1,1), Vector3(1,1,1));
        //debugDrawer.drawBox(Vector3(-1,-1,-1), Vector3(1,1,1), mat, Vector3(1,1,0));
        //debugDrawer.drawSphere(1.0f, Vector3(0,0,0), mat, Vector3(1,1,1));

        debugDrawer.end();


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


