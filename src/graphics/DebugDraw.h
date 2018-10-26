#ifndef DEBUGDRAW_H
#define DEBUGDRAW_H

#include <LinearMath/btIDebugDraw.h>
#include "../core/Singleton.h"

namespace gplay {

class MeshBatch;
class Vector3;
class Matrix;

/**
 * Use the btIDebugDraw interface for generating shapes using bullet DebugDraw and rendering with gplay-engine.
 * You shouln't use this class directly to avoid to use bullet types prefer the DebugDrawer class below that use gplay types.
 */
class BulletDebugDraw : public btIDebugDraw
{
public:

    BulletDebugDraw();
    ~BulletDebugDraw();

    void begin(const Matrix& viewProjection);
    void end();

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;
    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

    static const btTransform toBtTransform(const Matrix& matrix);

    virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) override {}
    virtual void reportErrorWarning(const char* warningString) override {}
    virtual void draw3dText(const btVector3& location,const char* textString) override {}
    virtual void setDebugMode(int debugMode) override {}
    virtual int	getDebugMode() const override { return 0; }

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
class DebugDrawImpl
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



typedef gplay::Singleton<DebugDrawImpl> DebugDraw;


} // end namespace gplay


#endif // DEBUGDRAW_H
