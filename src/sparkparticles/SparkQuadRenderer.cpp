#ifndef GP_NO_SPARK

#include <spark/SPARK_Core.h>
#include "../sparkparticles/SparkQuadRenderer.h"
#include "../graphics/MeshPart.h"
#include "../graphics/Material.h"

namespace SPK {
namespace GP3D {


SparkQuadRenderBuffer::SparkQuadRenderBuffer(size_t nbParticles, size_t nbVerticesPerParticle, size_t nbIndicesPerParticle) :
    RenderBuffer()
{
    SPK_ASSERT(nbParticles > 0, "GPSpriteRenderBuffer - The number of particles cannot be 0");
    SPK_ASSERT(nbVerticesPerParticle > 0, "GPSpriteRenderBuffer- The number of vertices per particle cannot be 0");
    SPK_ASSERT(nbIndicesPerParticle > 0, "GPSpriteRenderBuffer - The number of indices per particle cannot be 0");


    // Create a mesh with one mesh part to store vertex and index buffers.
    // Initialize vertex and index buffers with initial elements count computed from nbparticles.

    unsigned int initialVertexCount = nbParticles * nbVerticesPerParticle;
    unsigned int initialIindexCount = nbParticles * nbIndicesPerParticle;

    gplay::VertexFormat::Element elements[] =
    {
        gplay::VertexFormat::Element(gplay::VertexFormat::POSITION, 3),
        gplay::VertexFormat::Element(gplay::VertexFormat::COLOR, 4, gplay::VertexFormat::Uint8, true),
        gplay::VertexFormat::Element(gplay::VertexFormat::TEXCOORD0, 2)

    };

    _mesh = gplay::Mesh::createMesh(gplay::VertexFormat(elements, 3), initialVertexCount, true);
    _mesh->setVertexData(0, 0, initialVertexCount);

    bool largeIndices = (nbParticles * nbIndicesPerParticle) >= 65536;
    gplay::Mesh::IndexFormat indexFormat = largeIndices ? gplay::Mesh::INDEX32 : gplay::Mesh::INDEX16;

    gplay::MeshPart* meshPart = _mesh->addPart(gplay::Mesh::TRIANGLES, indexFormat, initialIindexCount, true);
    meshPart->setIndexData(0, 0, initialIindexCount);
}

SparkQuadRenderBuffer::~SparkQuadRenderBuffer()
{
    _mesh->release();
}





SparkQuadRenderer::SparkQuadRenderer(float scaleX, float scaleY) :
    SparkBaseRenderer(),
    QuadRenderBehavior(scaleX,scaleY),
    Oriented3DRenderBehavior()
{

    //setTexturingMode(TEXTURE_MODE_2D);


}

SparkQuadRenderer::SparkQuadRenderer(const SparkQuadRenderer &renderer) :
    SparkBaseRenderer(renderer),
    QuadRenderBehavior(renderer),
    Oriented3DRenderBehavior(renderer)
{
    // used in copy mechanism

    /*_camera = renderer._camera;
    _material = renderer._material;
    _elements = renderer._elements;
    setTexturingMode(getTexturingMode());*/
}


RenderBuffer* SparkQuadRenderer::attachRenderBuffer(const Group& group) const
{
    // Creates the render buffer
    SparkQuadRenderBuffer* buffer = SPK_NEW(SparkQuadRenderBuffer,group.getCapacity(),NB_VERTICES_PER_PARTICLE,NB_INDICES_PER_PARTICLE);


    size_t numParticles = group.getCapacity();

    // map index buffer and fill data

    gplay::MeshPart* meshPart = buffer->getMesh()->getPart(0);

    GP_ASSERT(meshPart->getIndexBuffer()->getElementCount() == numParticles * NB_INDICES_PER_PARTICLE);

    void* destPtr = meshPart->mapIndexBuffer();
    if(!destPtr)
        GP_ERROR("Unable to map index buffer.");

    gplay::Mesh::IndexFormat indexFormat = meshPart->getIndexFormat();
    if (indexFormat == gplay::Mesh::INDEX16)
    {
        // 16 bits index format

        unsigned short* dest = (unsigned short*)destPtr;
        unsigned short vertexIndex = 0;
        while (numParticles--)
        {
            dest[0] = vertexIndex + 0;
            dest[1] = vertexIndex + 2;
            dest[2] = vertexIndex + 1;
            dest[3] = vertexIndex + 2;
            dest[4] = vertexIndex + 3;
            dest[5] = vertexIndex + 1;

            dest += NB_INDICES_PER_PARTICLE;
            vertexIndex += NB_VERTICES_PER_PARTICLE;
        }
    }
    else
    {
        // 32 bits index format

        unsigned* dest = (unsigned*)destPtr;
        unsigned vertexIndex = 0;
        while (numParticles--)
        {
            dest[0] = vertexIndex + 0;
            dest[1] = vertexIndex + 2;
            dest[2] = vertexIndex + 1;
            dest[3] = vertexIndex + 2;
            dest[4] = vertexIndex + 3;
            dest[5] = vertexIndex + 1;

            dest += NB_INDICES_PER_PARTICLE;
            vertexIndex += NB_VERTICES_PER_PARTICLE;
        }
    }

    meshPart->unmapIndexBuffer();

    return buffer;
}




void SparkQuadRenderer::render(const Group& group,const DataSet* dataSet,RenderBuffer* renderBuffer) const
{
    SPK_ASSERT(renderBuffer != NULL,"QuadRenderer::render - renderBuffer must not be NULL");
    SparkQuadRenderBuffer& buffer = static_cast<SparkQuadRenderBuffer&>(*renderBuffer);

    unsigned numParticles = group.getNbParticles();

    GP_ASSERT(buffer.getMesh()->getVertexBuffer()->getElementCount() >= numParticles * NB_VERTICES_PER_PARTICLE);

    if(numParticles == 0)
       return;

    // Computes the inverse model view
    gplay::Matrix invModelView = _viewMatrix;
    //invModelView.transpose();
    //invModelView.invert();


    // select render method
    if ((texturingMode == TEXTURE_MODE_2D)&&(group.isEnabled(PARAM_TEXTURE_INDEX)))
    {
        if (group.isEnabled(PARAM_ANGLE))
            renderParticle = &SparkQuadRenderer::renderAtlasRot;
        else
            renderParticle = &SparkQuadRenderer::renderAtlas;
    }
    else
    {
        if (group.isEnabled(PARAM_ANGLE))
            renderParticle = &SparkQuadRenderer::renderRot;
        else
            renderParticle = &SparkQuadRenderer::renderBasic;
    }

    /*bool globalOrientation = precomputeOrientation3D(group,
                Vector3D(invModelView.m[8],  invModelView.m[9],  invModelView.m[10]),
                Vector3D(invModelView.m[4],  invModelView.m[5],  invModelView.m[6]),
                Vector3D(invModelView.m[12], invModelView.m[13], invModelView.m[14]));*/

    bool globalOrientation = precomputeOrientation3D(
                group,
                Vector3D( -invModelView.m[8], -invModelView.m[9], -invModelView.m[10]  ),
                Vector3D(  invModelView.m[4],  invModelView.m[5],  invModelView.m[6]   ),
                Vector3D(  invModelView.m[12], invModelView.m[13], invModelView.m[14]  ));

    if (globalOrientation)
        computeGlobalOrientation3D(group);

    // map vertex buffer and fill data

    float* dest = (float*)buffer.getMesh()->mapVertexBuffer();
    if(!dest)
        GP_ERROR("Unable to map vertex buffer.");

    for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
    {
        const Particle& particle = *particleIt;

        if (!globalOrientation)
            computeSingleOrientation3D(particle);

        // call render method to compute quad
        (this->*renderParticle)(particle,buffer);

        // Vertices are drawn in counter clockwise order.
        // First triangle  : v0,v2,v1
        // Second triangle : v2,v3,v1
        //                 |
        //   v1     v3     |     0,1        1,1
        //     x---x       |        x------x
        //     |\  |       |        |      |
        //     | \ |       |        |      |
        //     |  \|       |        |      |
        //     x---x       |        x------x
        //   v0     v2     |     0,0        1,0
        //                 |
        //     Pos         |           UV


        Vector3D v0 = particle.position() - quadSide() - quadUp(); // bottom left vertex
        Vector3D v1 = particle.position() - quadSide() + quadUp(); // top left vertex
        Vector3D v2 = particle.position() + quadSide() - quadUp(); // bottom right vertex
        Vector3D v3 = particle.position() + quadSide() + quadUp(); // top right vertex


        const unsigned& color =  particle.getColor().getABGR();

        unsigned int index = 0;

        dest[index++] = v0.x;
        dest[index++] = v0.y;
        dest[index++] = v0.z;
        ((unsigned&)dest[index++]) = color;
        dest[index++] = _u0;
        dest[index++] = _v0;

        dest[index++] = v1.x;
        dest[index++] = v1.y;
        dest[index++] = v1.z;
        ((unsigned&)dest[index++]) = color;
        dest[index++] = _u0;
        dest[index++] = _v1;

        dest[index++] = v2.x;
        dest[index++] = v2.y;
        dest[index++] = v2.z;
        ((unsigned&)dest[index++]) = color;
        dest[index++] = _u1;
        dest[index++] = _v0;

        dest[index++] = v3.x;
        dest[index++] = v3.y;
        dest[index++] = v3.z;
        ((unsigned&)dest[index++]) = color;
        dest[index++] = _u1;
        dest[index++] = _v1;

        dest += index;
    }

    buffer.getMesh()->unmapVertexBuffer();


    // Set draw range
    buffer.getMesh()->setDrawRange(0, numParticles * NB_VERTICES_PER_PARTICLE);
    buffer.getMesh()->getPart(0)->setDrawRange(0, numParticles * NB_INDICES_PER_PARTICLE);


    // Bind the material.    
    if (_material)
    {
        gplay::Technique* technique = _material->getTechnique();
        GP_ASSERT(technique);

        gplay::MeshPart* part = buffer.getMesh()->getPart(0);
        unsigned int passCount = technique->getPassCount();

        for (unsigned int i=0; i<passCount; ++i)
        {
            gplay::Pass* pass = technique->getPassByIndex(i);
            GP_ASSERT(pass);
            pass->bind(part->getPrimitiveType());
            buffer.getMesh()->getVertexBuffer()->bind();
            part->getIndexBuffer()->bind();
            part->draw();
            pass->unbind();
        }
    }
}

void SparkQuadRenderer::computeAABB(Vector3D& AABBMin,Vector3D& AABBMax,const Group& group,const DataSet* dataSet) const
{
    float diagonal = group.getGraphicalRadius() * std::sqrt(scaleX * scaleX + scaleY * scaleY);
    Vector3D diagV(diagonal,diagonal,diagonal);

    if (group.isEnabled(PARAM_SCALE))
        for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
        {
            Vector3D scaledDiagV = diagV * particleIt->getParamNC(PARAM_SCALE);
            AABBMin.setMin(particleIt->position() - scaledDiagV);
            AABBMax.setMax(particleIt->position() + scaledDiagV);
        }
    else
    {
        for (ConstGroupIterator particleIt(group); !particleIt.end(); ++particleIt)
        {
            AABBMin.setMin(particleIt->position());
            AABBMax.setMax(particleIt->position());
        }
        AABBMin -= diagV;
        AABBMax += diagV;
    }
}

void SparkQuadRenderer::renderBasic(const Particle& particle,SparkQuadRenderBuffer& renderBuffer) const
{
    scaleQuadVectors(particle,scaleX,scaleY);
    _u0 = _v0 = 0.0f;
    _u1 = _v1 = 1.0f;
}

void SparkQuadRenderer::renderRot(const Particle& particle,SparkQuadRenderBuffer& renderBuffer) const
{
    rotateAndScaleQuadVectors(particle,scaleX,scaleY);
    _u0 = _v0 = 0.0f;
    _u1 = _v1 = 1.0f;
}

void SparkQuadRenderer::renderAtlas(const Particle& particle,SparkQuadRenderBuffer& renderBuffer) const
{
    scaleQuadVectors(particle,scaleX,scaleY);
    computeAtlasCoordinates(particle);
    _u0 = textureAtlasU0();
    _u1 = textureAtlasU1();
    _v0 = textureAtlasV0();
    _v1 = textureAtlasV1();
}

void SparkQuadRenderer::renderAtlasRot(const Particle& particle,SparkQuadRenderBuffer& renderBuffer) const
{
    rotateAndScaleQuadVectors(particle,scaleX,scaleY);
    computeAtlasCoordinates(particle);
    _u0 = textureAtlasU0();
    _u1 = textureAtlasU1();
    _v0 = textureAtlasV0();
    _v1 = textureAtlasV1();
}

void SparkQuadRenderer::innerImport(const IO::Descriptor& descriptor)
{
}

void SparkQuadRenderer::innerExport(IO::Descriptor& descriptor) const
{
}



}}

#endif
