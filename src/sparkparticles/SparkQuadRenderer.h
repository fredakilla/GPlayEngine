#pragma once

#include <spark/Extensions/Renderers/SPK_QuadRenderBehavior.h>
#include <spark/Extensions/Renderers/SPK_Oriented3DRenderBehavior.h>
#include "../sparkparticles/SparkUtility.h"
#include "../sparkparticles/SparkBaseRenderer.h"

namespace SPK {
namespace GP3D {


/**
 * Quad render buffer
 */
class SparkQuadRenderBuffer : public RenderBuffer
{

public :

    SparkQuadRenderBuffer(size_t nbParticles, size_t nbVerticesPerParticle, size_t nbIndicesPerParticle);
    ~SparkQuadRenderBuffer();

    //const gplay::IndexBuffer* getIndexBuffer(unsigned int index) { return _mesh->getPart(index)->getIndexBuffer(); }
    gplay::Mesh* getMesh() { return _mesh; }



private:

    // Use Mesh class to store particle geometry.
    gplay::Mesh* _mesh;

};








/**
 * A renderer for sprites
 */
class SparkQuadRenderer :
        public SparkBaseRenderer,
        public QuadRenderBehavior,
        public Oriented3DRenderBehavior
{
    SPK_IMPLEMENT_OBJECT(SparkQuadRenderer)

    SPK_START_DESCRIPTION
    SPK_PARENT_ATTRIBUTES(Renderer)
    SPK_ATTRIBUTE("material",ATTRIBUTE_TYPE_STRING)
    SPK_ATTRIBUTE("texture",ATTRIBUTE_TYPE_STRING)
    SPK_ATTRIBUTE("scale",ATTRIBUTE_TYPE_FLOATS)
    SPK_ATTRIBUTE("atlasdim",ATTRIBUTE_TYPE_UINT32S)
    SPK_END_DESCRIPTION

public:        
    static Ref<SparkQuadRenderer> create(float scaleX = 1.0f,float scaleY = 1.0f);

protected:
    virtual void innerImport(const IO::Descriptor& descriptor) override;
    virtual void innerExport(IO::Descriptor& descriptor) const override;

private:

    static const size_t NB_INDICES_PER_PARTICLE = 6;
    static const size_t NB_VERTICES_PER_PARTICLE = 4;
    mutable float _u0, _u1, _v0, _v1;

    SparkQuadRenderer(float scaleX = 1.0f,float scaleY = 1.0f);
    SparkQuadRenderer(const SparkQuadRenderer& renderer);

    virtual RenderBuffer* attachRenderBuffer(const Group& group) const override;
    virtual void render(const Group& group,const DataSet* dataSet,RenderBuffer* renderBuffer) const override;
    virtual void computeAABB(Vector3D& AABBMin,Vector3D& AABBMax,const Group& group,const DataSet* dataSet) const override;

    /// pointer of funtion to a render method
    mutable void (SparkQuadRenderer::*renderParticle)(const Particle&, SparkQuadRenderBuffer& renderBuffer) const;

    /// Rendering for particles with texture or no texture
    void renderBasic(const Particle& particle,SparkQuadRenderBuffer& renderBuffer) const;
    /// Rendering for particles with texture or no texture and rotation
    void renderRot(const Particle& particle,SparkQuadRenderBuffer& renderBuffer) const;
    /// Rendering for particles with texture atlas
    void renderAtlas(const Particle& particle,SparkQuadRenderBuffer& renderBuffer) const;
    /// Rendering for particles with texture atlas and rotation
    void renderAtlasRot(const Particle& particle,SparkQuadRenderBuffer& renderBuffer) const;
};


inline Ref<SparkQuadRenderer> SparkQuadRenderer::create(float scaleX, float scaleY)
{
    return SPK_NEW(SparkQuadRenderer, scaleX, scaleY);
}


}}
