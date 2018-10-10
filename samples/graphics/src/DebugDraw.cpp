#include <gplay-engine.h>
#include "Example.h"
#include "SamplesGame.h"
#include "FirstPersonCamera.h"
#include <imgui/imgui.h>

using namespace gplay;

// code from btIDebugDraw
class DebugDrawer
{
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

public:
    DebugDrawer()
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

    ~DebugDrawer()
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

    void drawLine(const Vector3& from, const Vector3& to, const Vector3& fromColor, const Vector3& toColor)
    {
        GP_ASSERT(_meshBatch);

        static DebugDrawer::DebugVertex vertices[2];

        vertices[0].x = from.x;
        vertices[0].y = from.y;
        vertices[0].z = from.z;
        vertices[0].r = fromColor.x;
        vertices[0].g = fromColor.y;
        vertices[0].b = fromColor.z;
        vertices[0].a = 1.0f;

        vertices[1].x = to.x;
        vertices[1].y = to.y;
        vertices[1].z = to.z;
        vertices[1].r = toColor.x;
        vertices[1].g = toColor.y;
        vertices[1].b = toColor.z;
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

    void drawLine(const Vector3& from, const Vector3& to, const Vector3& color)
    {
        drawLine(from, to, color, color);
    }




    void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
    {
        GP_ASSERT(_meshBatch);

        static DebugDrawer::DebugVertex vertices[2];

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

    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        drawLine(from, to, color, color);
    }


    void drawBox(const Vector3& bbMin, const Vector3& bbMax, const Vector3& color)
    {
        drawLine(Vector3(bbMin.x, bbMin.y, bbMin.z), Vector3(bbMax.x, bbMin.y, bbMin.z), color);
        drawLine(Vector3(bbMax.x, bbMin.y, bbMin.z), Vector3(bbMax.x, bbMax.y, bbMin.z), color);
        drawLine(Vector3(bbMax.x, bbMax.y, bbMin.z), Vector3(bbMin.x, bbMax.y, bbMin.z), color);
        drawLine(Vector3(bbMin.x, bbMax.y, bbMin.z), Vector3(bbMin.x, bbMin.y, bbMin.z), color);
        drawLine(Vector3(bbMin.x, bbMin.y, bbMin.z), Vector3(bbMin.x, bbMin.y, bbMax.z), color);
        drawLine(Vector3(bbMax.x, bbMin.y, bbMin.z), Vector3(bbMax.x, bbMin.y, bbMax.z), color);
        drawLine(Vector3(bbMax.x, bbMax.y, bbMin.z), Vector3(bbMax.x, bbMax.y, bbMax.z), color);
        drawLine(Vector3(bbMin.x, bbMax.y, bbMin.z), Vector3(bbMin.x, bbMax.y, bbMax.z), color);
        drawLine(Vector3(bbMin.x, bbMin.y, bbMax.z), Vector3(bbMax.x, bbMin.y, bbMax.z), color);
        drawLine(Vector3(bbMax.x, bbMin.y, bbMax.z), Vector3(bbMax.x, bbMax.y, bbMax.z), color);
        drawLine(Vector3(bbMax.x, bbMax.y, bbMax.z), Vector3(bbMin.x, bbMax.y, bbMax.z), color);
        drawLine(Vector3(bbMin.x, bbMax.y, bbMax.z), Vector3(bbMin.x, bbMin.y, bbMax.z), color);
    }

    void drawBox(const Vector3& bbMin, const Vector3& bbMax, const Matrix& trans, const Vector3& color)
    {
        drawLine(trans * Vector3(bbMin.x, bbMin.y, bbMin.z), trans * Vector3(bbMax.x, bbMin.y, bbMin.z), color);
        drawLine(trans * Vector3(bbMax.x, bbMin.y, bbMin.z), trans * Vector3(bbMax.x, bbMax.y, bbMin.z), color);
        drawLine(trans * Vector3(bbMax.x, bbMax.y, bbMin.z), trans * Vector3(bbMin.x, bbMax.y, bbMin.z), color);
        drawLine(trans * Vector3(bbMin.x, bbMax.y, bbMin.z), trans * Vector3(bbMin.x, bbMin.y, bbMin.z), color);
        drawLine(trans * Vector3(bbMin.x, bbMin.y, bbMin.z), trans * Vector3(bbMin.x, bbMin.y, bbMax.z), color);
        drawLine(trans * Vector3(bbMax.x, bbMin.y, bbMin.z), trans * Vector3(bbMax.x, bbMin.y, bbMax.z), color);
        drawLine(trans * Vector3(bbMax.x, bbMax.y, bbMin.z), trans * Vector3(bbMax.x, bbMax.y, bbMax.z), color);
        drawLine(trans * Vector3(bbMin.x, bbMax.y, bbMin.z), trans * Vector3(bbMin.x, bbMax.y, bbMax.z), color);
        drawLine(trans * Vector3(bbMin.x, bbMin.y, bbMax.z), trans * Vector3(bbMax.x, bbMin.y, bbMax.z), color);
        drawLine(trans * Vector3(bbMax.x, bbMin.y, bbMax.z), trans * Vector3(bbMax.x, bbMax.y, bbMax.z), color);
        drawLine(trans * Vector3(bbMax.x, bbMax.y, bbMax.z), trans * Vector3(bbMin.x, bbMax.y, bbMax.z), color);
        drawLine(trans * Vector3(bbMin.x, bbMax.y, bbMax.z), trans * Vector3(bbMin.x, bbMin.y, bbMax.z), color);
    }







    void drawTriangle(const btVector3& v0,const btVector3& v1,const btVector3& v2,const btVector3& /*n0*/,const btVector3& /*n1*/,const btVector3& /*n2*/,const btVector3& color, btScalar alpha)
    {
        drawTriangle(v0,v1,v2,color,alpha);
    }

    void drawTriangle(const btVector3& v0,const btVector3& v1,const btVector3& v2,const btVector3& color, btScalar /*alpha*/)
    {
        drawLine(v0,v1,color);
        drawLine(v1,v2,color);
        drawLine(v2,v0,color);
    }

    void drawSphere(btScalar radius, const btTransform& transform, const btVector3& color)
    {
        btVector3 center = transform.getOrigin();
        btVector3 up = transform.getBasis().getColumn(1);
        btVector3 axis = transform.getBasis().getColumn(0);
        btScalar minTh = -SIMD_HALF_PI;
        btScalar maxTh = SIMD_HALF_PI;
        btScalar minPs = -SIMD_HALF_PI;
        btScalar maxPs = SIMD_HALF_PI;
        btScalar stepDegrees = 30.f;
        drawSpherePatch(center, up, axis, radius,minTh, maxTh, minPs, maxPs, color, stepDegrees ,false);
        drawSpherePatch(center, up, -axis, radius,minTh, maxTh, minPs, maxPs, color, stepDegrees,false );
    }

    void drawSphere (const btVector3& p, btScalar radius, const btVector3& color)
    {
        btTransform tr;
        tr.setIdentity();
        tr.setOrigin(p);
        drawSphere(radius,tr,color);
    }

    void drawSpherePatch(const btVector3& center, const btVector3& up, const btVector3& axis, btScalar radius,
                                 btScalar minTh, btScalar maxTh, btScalar minPs, btScalar maxPs, const btVector3& color, btScalar stepDegrees = btScalar(10.f),bool drawCenter = true)
    {
        btVector3 vA[74];
        btVector3 vB[74];
        btVector3 *pvA = vA, *pvB = vB, *pT;
        btVector3 npole = center + up * radius;
        btVector3 spole = center - up * radius;
        btVector3 arcStart;
        btScalar step = stepDegrees * SIMD_RADS_PER_DEG;
        const btVector3& kv = up;
        const btVector3& iv = axis;
        btVector3 jv = kv.cross(iv);
        bool drawN = false;
        bool drawS = false;
        if(minTh <= -SIMD_HALF_PI)
        {
            minTh = -SIMD_HALF_PI + step;
            drawN = true;
        }
        if(maxTh >= SIMD_HALF_PI)
        {
            maxTh = SIMD_HALF_PI - step;
            drawS = true;
        }
        if(minTh > maxTh)
        {
            minTh = -SIMD_HALF_PI + step;
            maxTh =  SIMD_HALF_PI - step;
            drawN = drawS = true;
        }
        int n_hor = (int)((maxTh - minTh) / step) + 1;
        if(n_hor < 2) n_hor = 2;
        btScalar step_h = (maxTh - minTh) / btScalar(n_hor - 1);
        bool isClosed = false;
        if(minPs > maxPs)
        {
            minPs = -SIMD_PI + step;
            maxPs =  SIMD_PI;
            isClosed = true;
        }
        else if((maxPs - minPs) >= SIMD_PI * btScalar(2.f))
        {
            isClosed = true;
        }
        else
        {
            isClosed = false;
        }
        int n_vert = (int)((maxPs - minPs) / step) + 1;
        if(n_vert < 2) n_vert = 2;
        btScalar step_v = (maxPs - minPs) / btScalar(n_vert - 1);
        for(int i = 0; i < n_hor; i++)
        {
            btScalar th = minTh + btScalar(i) * step_h;
            btScalar sth = radius * btSin(th);
            btScalar cth = radius * btCos(th);
            for(int j = 0; j < n_vert; j++)
            {
                btScalar psi = minPs + btScalar(j) * step_v;
                btScalar sps = btSin(psi);
                btScalar cps = btCos(psi);
                pvB[j] = center + cth * cps * iv + cth * sps * jv + sth * kv;
                if(i)
                {
                    drawLine(pvA[j], pvB[j], color);
                }
                else if(drawS)
                {
                    drawLine(spole, pvB[j], color);
                }
                if(j)
                {
                    drawLine(pvB[j-1], pvB[j], color);
                }
                else
                {
                    arcStart = pvB[j];
                }
                if((i == (n_hor - 1)) && drawN)
                {
                    drawLine(npole, pvB[j], color);
                }

                if (drawCenter)
                {
                    if(isClosed)
                    {
                        if(j == (n_vert-1))
                        {
                            drawLine(arcStart, pvB[j], color);
                        }
                    }
                    else
                    {
                        if(((!i) || (i == (n_hor-1))) && ((!j) || (j == (n_vert-1))))
                        {
                            drawLine(center, pvB[j], color);
                        }
                    }
                }
            }
            pT = pvA; pvA = pvB; pvB = pT;
        }
    }


    void drawBox(const btVector3& bbMin, const btVector3& bbMax, const btVector3& color)
    {
        drawLine(btVector3(bbMin[0], bbMin[1], bbMin[2]), btVector3(bbMax[0], bbMin[1], bbMin[2]), color);
        drawLine(btVector3(bbMax[0], bbMin[1], bbMin[2]), btVector3(bbMax[0], bbMax[1], bbMin[2]), color);
        drawLine(btVector3(bbMax[0], bbMax[1], bbMin[2]), btVector3(bbMin[0], bbMax[1], bbMin[2]), color);
        drawLine(btVector3(bbMin[0], bbMax[1], bbMin[2]), btVector3(bbMin[0], bbMin[1], bbMin[2]), color);
        drawLine(btVector3(bbMin[0], bbMin[1], bbMin[2]), btVector3(bbMin[0], bbMin[1], bbMax[2]), color);
        drawLine(btVector3(bbMax[0], bbMin[1], bbMin[2]), btVector3(bbMax[0], bbMin[1], bbMax[2]), color);
        drawLine(btVector3(bbMax[0], bbMax[1], bbMin[2]), btVector3(bbMax[0], bbMax[1], bbMax[2]), color);
        drawLine(btVector3(bbMin[0], bbMax[1], bbMin[2]), btVector3(bbMin[0], bbMax[1], bbMax[2]), color);
        drawLine(btVector3(bbMin[0], bbMin[1], bbMax[2]), btVector3(bbMax[0], bbMin[1], bbMax[2]), color);
        drawLine(btVector3(bbMax[0], bbMin[1], bbMax[2]), btVector3(bbMax[0], bbMax[1], bbMax[2]), color);
        drawLine(btVector3(bbMax[0], bbMax[1], bbMax[2]), btVector3(bbMin[0], bbMax[1], bbMax[2]), color);
        drawLine(btVector3(bbMin[0], bbMax[1], bbMax[2]), btVector3(bbMin[0], bbMin[1], bbMax[2]), color);
    }

    void drawBox(const btVector3& bbMin, const btVector3& bbMax, const btTransform& trans, const btVector3& color)
    {
        drawLine(trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), color);
        drawLine(trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), color);
        drawLine(trans * btVector3(bbMin[0], bbMin[1], bbMin[2]), trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMin[1], bbMin[2]), trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMax[1], bbMin[2]), trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMin[0], bbMax[1], bbMin[2]), trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMin[1], bbMax[2]), trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMax[0], bbMax[1], bbMax[2]), trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), color);
        drawLine(trans * btVector3(bbMin[0], bbMax[1], bbMax[2]), trans * btVector3(bbMin[0], bbMin[1], bbMax[2]), color);
    }

    void drawCapsule(btScalar radius, btScalar halfHeight, int upAxis, const btTransform& transform, const btVector3& color)
    {
        int stepDegrees = 30;

        btVector3 capStart(0.f,0.f,0.f);
        capStart[upAxis] = -halfHeight;

        btVector3 capEnd(0.f,0.f,0.f);
        capEnd[upAxis] = halfHeight;

        // Draw the ends
        {

            btTransform childTransform = transform;
            childTransform.getOrigin() = transform * capStart;
            {
                btVector3 center = childTransform.getOrigin();
                btVector3 up = childTransform.getBasis().getColumn((upAxis+1)%3);
                btVector3 axis = -childTransform.getBasis().getColumn(upAxis);
                btScalar minTh = -SIMD_HALF_PI;
                btScalar maxTh = SIMD_HALF_PI;
                btScalar minPs = -SIMD_HALF_PI;
                btScalar maxPs = SIMD_HALF_PI;

                drawSpherePatch(center, up, axis, radius,minTh, maxTh, minPs, maxPs, color, btScalar(stepDegrees) ,false);
            }



        }

        {
            btTransform childTransform = transform;
            childTransform.getOrigin() = transform * capEnd;
            {
                btVector3 center = childTransform.getOrigin();
                btVector3 up = childTransform.getBasis().getColumn((upAxis+1)%3);
                btVector3 axis = childTransform.getBasis().getColumn(upAxis);
                btScalar minTh = -SIMD_HALF_PI;
                btScalar maxTh = SIMD_HALF_PI;
                btScalar minPs = -SIMD_HALF_PI;
                btScalar maxPs = SIMD_HALF_PI;
                drawSpherePatch(center, up, axis, radius,minTh, maxTh, minPs, maxPs, color, btScalar(stepDegrees) ,false);
            }
        }

        // Draw some additional lines
        btVector3 start = transform.getOrigin();

        for (int i=0;i<360;i+=stepDegrees)
        {
            capEnd[(upAxis+1)%3] = capStart[(upAxis+1)%3] = btSin(btScalar(i)*SIMD_RADS_PER_DEG)*radius;
            capEnd[(upAxis+2)%3] = capStart[(upAxis+2)%3]  = btCos(btScalar(i)*SIMD_RADS_PER_DEG)*radius;
            drawLine(start+transform.getBasis() * capStart,start+transform.getBasis() * capEnd, color);
        }

    }

    void drawCylinder(btScalar radius, btScalar halfHeight, int upAxis, const btTransform& transform, const btVector3& color)
    {
        btVector3 start = transform.getOrigin();
        btVector3	offsetHeight(0,0,0);
        offsetHeight[upAxis] = halfHeight;
        int stepDegrees=30;
        btVector3 capStart(0.f,0.f,0.f);
        capStart[upAxis] = -halfHeight;
        btVector3 capEnd(0.f,0.f,0.f);
        capEnd[upAxis] = halfHeight;

        for (int i=0;i<360;i+=stepDegrees)
        {
            capEnd[(upAxis+1)%3] = capStart[(upAxis+1)%3] = btSin(btScalar(i)*SIMD_RADS_PER_DEG)*radius;
            capEnd[(upAxis+2)%3] = capStart[(upAxis+2)%3]  = btCos(btScalar(i)*SIMD_RADS_PER_DEG)*radius;
            drawLine(start+transform.getBasis() * capStart,start+transform.getBasis() * capEnd, color);
        }
        // Drawing top and bottom caps of the cylinder
        btVector3 yaxis(0,0,0);
        yaxis[upAxis] = btScalar(1.0);
        btVector3 xaxis(0,0,0);
        xaxis[(upAxis+1)%3] = btScalar(1.0);
        drawArc(start-transform.getBasis()*(offsetHeight),transform.getBasis()*yaxis,transform.getBasis()*xaxis,radius,radius,0,SIMD_2_PI,color,false,btScalar(10.0));
        drawArc(start+transform.getBasis()*(offsetHeight),transform.getBasis()*yaxis,transform.getBasis()*xaxis,radius,radius,0,SIMD_2_PI,color,false,btScalar(10.0));
    }

    void drawCone(btScalar radius, btScalar height, int upAxis, const btTransform& transform, const btVector3& color)
    {
        int stepDegrees = 30;
        btVector3 start = transform.getOrigin();

        btVector3	offsetHeight(0,0,0);
        btScalar halfHeight = height * btScalar(0.5);
        offsetHeight[upAxis] = halfHeight;
        btVector3	offsetRadius(0,0,0);
        offsetRadius[(upAxis+1)%3] = radius;
        btVector3	offset2Radius(0,0,0);
        offset2Radius[(upAxis+2)%3] = radius;


        btVector3 capEnd(0.f,0.f,0.f);
        capEnd[upAxis] = -halfHeight;

        for (int i=0;i<360;i+=stepDegrees)
        {
            capEnd[(upAxis+1)%3] = btSin(btScalar(i)*SIMD_RADS_PER_DEG)*radius;
            capEnd[(upAxis+2)%3] = btCos(btScalar(i)*SIMD_RADS_PER_DEG)*radius;
            drawLine(start+transform.getBasis() * (offsetHeight),start+transform.getBasis() * capEnd, color);
        }

        drawLine(start+transform.getBasis() * (offsetHeight),start+transform.getBasis() * (-offsetHeight+offsetRadius),color);
        drawLine(start+transform.getBasis() * (offsetHeight),start+transform.getBasis() * (-offsetHeight-offsetRadius),color);
        drawLine(start+transform.getBasis() * (offsetHeight),start+transform.getBasis() * (-offsetHeight+offset2Radius),color);
        drawLine(start+transform.getBasis() * (offsetHeight),start+transform.getBasis() * (-offsetHeight-offset2Radius),color);

        // Drawing the base of the cone
        btVector3 yaxis(0,0,0);
        yaxis[upAxis] = btScalar(1.0);
        btVector3 xaxis(0,0,0);
        xaxis[(upAxis+1)%3] = btScalar(1.0);
        drawArc(start-transform.getBasis()*(offsetHeight),transform.getBasis()*yaxis,transform.getBasis()*xaxis,radius,radius,0,SIMD_2_PI,color,false,10.0);
    }

    void drawPlane(const btVector3& planeNormal, btScalar planeConst, const btTransform& transform, const btVector3& color)
    {
        btVector3 planeOrigin = planeNormal * planeConst;
        btVector3 vec0,vec1;
        btPlaneSpace1(planeNormal,vec0,vec1);
        btScalar vecLen = 100.f;
        btVector3 pt0 = planeOrigin + vec0*vecLen;
        btVector3 pt1 = planeOrigin - vec0*vecLen;
        btVector3 pt2 = planeOrigin + vec1*vecLen;
        btVector3 pt3 = planeOrigin - vec1*vecLen;
        drawLine(transform*pt0,transform*pt1,color);
        drawLine(transform*pt2,transform*pt3,color);
    }

    void drawArc(const btVector3& center, const btVector3& normal, const btVector3& axis, btScalar radiusA, btScalar radiusB, btScalar minAngle, btScalar maxAngle,
                 const btVector3& color, bool drawSect, btScalar stepDegrees = btScalar(10.f))
    {
        const btVector3& vx = axis;
        btVector3 vy = normal.cross(axis);
        btScalar step = stepDegrees * SIMD_RADS_PER_DEG;
        int nSteps = (int)btFabs((maxAngle - minAngle) / step);
        if(!nSteps) nSteps = 1;
        btVector3 prev = center + radiusA * vx * btCos(minAngle) + radiusB * vy * btSin(minAngle);
        if(drawSect)
        {
            drawLine(center, prev, color);
        }
        for(int i = 1; i <= nSteps; i++)
        {
            btScalar angle = minAngle + (maxAngle - minAngle) * btScalar(i) / btScalar(nSteps);
            btVector3 next = center + radiusA * vx * btCos(angle) + radiusB * vy * btSin(angle);
            drawLine(prev, next, color);
            prev = next;
        }
        if(drawSect)
        {
            drawLine(center, prev, color);
        }
    }

    void drawTransform(const btTransform& transform, btScalar orthoLen)
    {
        btVector3 start = transform.getOrigin();
        drawLine(start, start+transform.getBasis() * btVector3(orthoLen, 0, 0), btVector3(1.f,0.3,0.3));
        drawLine(start, start+transform.getBasis() * btVector3(0, orthoLen, 0), btVector3(0.3,1.f, 0.3));
        drawLine(start, start+transform.getBasis() * btVector3(0, 0, orthoLen), btVector3(0.3, 0.3,1.f));
    }

    void drawAabb(const btVector3& from,const btVector3& to,const btVector3& color)
    {

        btVector3 halfExtents = (to-from)* 0.5f;
        btVector3 center = (to+from) *0.5f;
        int i,j;

        btVector3 edgecoord(1.f,1.f,1.f),pa,pb;
        for (i=0;i<4;i++)
        {
            for (j=0;j<3;j++)
            {
                pa = btVector3(edgecoord[0]*halfExtents[0], edgecoord[1]*halfExtents[1],
                        edgecoord[2]*halfExtents[2]);
                pa+=center;

                int othercoord = j%3;
                edgecoord[othercoord]*=-1.f;
                pb = btVector3(edgecoord[0]*halfExtents[0], edgecoord[1]*halfExtents[1],
                        edgecoord[2]*halfExtents[2]);
                pb+=center;

                drawLine(pa,pb,color);
            }
            edgecoord = btVector3(-1.f,-1.f,-1.f);
            if (i<3)
                edgecoord[i]*=-1.f;
        }
    }

};



class DebugDrawing : public Example
{
    FirstPersonCamera _fpCamera;
    Font* _font;
    Scene* _scene;

    DebugDrawer debugDrawer;


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
        //_scene->visit(this, &DebugDrawing::drawScene);


        debugDrawer.begin(_scene->getActiveCamera()->getViewProjectionMatrix());
        //debugDrawer.drawLine(Vector3(0,0,0), Vector3(1,1,0), Vector3(1,1,1));
        Matrix mat; mat.rotateX(MATH_DEG_TO_RAD(45.0f));
        //debugDrawer.drawBox(Vector3(-1,-1,-1), Vector3(1,1,1), mat, Vector3(1,1,1));
        //debugDrawer.drawSphere(btVector3(0,0,0), 12.0f, btVector3(1,1,1));
        //debugDrawer.drawPlane(btVector3(0,1,0), 0.0f, btTransform::getIdentity(), btVector3(1,1,1));
        debugDrawer.drawCone(1.0f, 2.0f, 1, btTransform::getIdentity(), btVector3(1,0.5,0.25));

        /*btTransform trans;
        trans.setIdentity();
        trans.setRotation(btQuaternion(btVector3(1,0,0), MATH_DEG_TO_RAD(45.0f)));
        debugDrawer.drawTransform(trans, 1.0f);*/

        //debugDrawer.drawAabb(btVector3(0,0,0), btVector3(1,1,1), btVector3(1,1,1));

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

