#include "DebugDraw.h"
#include "../graphics/MeshBatch.h"
#include "../math/Vector3.h"
#include "../math/Matrix.h"
#include "../math/Quaternion.h"
#include "../graphics/Effect.h"
#include "../graphics/Material.h"
#include "../graphics/VertexFormat.h"

namespace gplay {

BulletDebugDraw::BulletDebugDraw()
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

BulletDebugDraw::~BulletDebugDraw()
{
    SAFE_DELETE(_meshBatch);
}

void BulletDebugDraw::begin(const Matrix& viewProjection)
{
    GP_ASSERT(_meshBatch);
    _meshBatch->start();
    _meshBatch->getMaterial()->getParameter("u_viewProjectionMatrix")->setValue(viewProjection);
}

void BulletDebugDraw::end()
{
    GP_ASSERT(_meshBatch && _meshBatch->getMaterial());
    _meshBatch->finish();
    _meshBatch->draw();
    _lineCount = 0;
}

void BulletDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
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

void BulletDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
    drawLine(from, to, color, color);
}

const btTransform BulletDebugDraw::toBtTransform(const Matrix& matrix)
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

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

void DebugDrawImpl::begin(const Matrix& viewProjection)
{
    dd.begin(viewProjection);
}

void DebugDrawImpl::end()
{
    dd.end();
}

void DebugDrawImpl::drawLine(const Vector3& from, const Vector3& to, const Vector3& color)
{
    dd.drawLine(BV(from), BV(to), BV(color), BV(color));
}

void DebugDrawImpl::drawLine(const Vector3& from, const Vector3& to, const Vector3& fromColor, const Vector3& toColor)
{
    dd.drawLine(BV(from), BV(to), BV(fromColor), BV(toColor));
}

void DebugDrawImpl::drawBox(const Vector3& bbMin, const Vector3& bbMax, const Vector3& color)
{
    dd.drawBox(BV(bbMin), BV(bbMax), BV(color));
}

void DebugDrawImpl::drawBox(const Vector3& bbMin, const Vector3& bbMax, const Matrix& matrix, const Vector3& color)
{
    dd.drawBox(BV(bbMin), BV(bbMax), BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawImpl::drawTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& color)
{
    drawLine(v0,v1,color);
    drawLine(v1,v2,color);
    drawLine(v2,v0,color);
}

void DebugDrawImpl::drawSphere(float radius, const Matrix& matrix, const Vector3& color)
{
    dd.drawSphere(radius, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawImpl::drawSphere(const Vector3& p, float radius, const Vector3& color)
{
    dd.drawSphere(BV(p), radius, BV(color));
}

void DebugDrawImpl::drawCapsule(float radius, float halfHeight, int upAxis, const Matrix& matrix, const Vector3& color)
{
    dd.drawCapsule(radius, halfHeight, upAxis, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawImpl::drawCylinder(float radius, float halfHeight, int upAxis, const Matrix& matrix, const Vector3& color)
{
    dd.drawCylinder(radius, halfHeight, upAxis, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawImpl::drawCone(float radius, float height, int upAxis, const Matrix& matrix, const Vector3& color)
{
    dd.drawCone(radius, height, upAxis, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawImpl::drawPlane(const Vector3& planeNormal, float planeConst, const Matrix& matrix, const Vector3& color)
{
    dd.drawPlane(BV(planeNormal), planeConst, BulletDebugDraw::toBtTransform(matrix), BV(color));
}

void DebugDrawImpl::drawArc(const Vector3& center, const Vector3& normal, const Vector3& axis, float radiusA, float radiusB, float minAngle, float maxAngle,
             const Vector3& color, bool drawSect, float stepDegrees)
{
    dd.drawArc(BV(center), BV(normal), BV(axis), radiusA, radiusB, minAngle, maxAngle, BV(color), drawSect, stepDegrees);
}

void DebugDrawImpl::drawTransform(const Matrix& matrix, float orthoLen)
{
    dd.drawTransform(BulletDebugDraw::toBtTransform(matrix), orthoLen);
}

void DebugDrawImpl::drawAabb(const Vector3& from, const Vector3& to, const Vector3& color)
{
    dd.drawAabb(BV(from), BV(to), BV(color));
}

} // end namespace gplay

