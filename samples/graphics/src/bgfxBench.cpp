#include "gplay-engine.h"
#include "Example.h"
#include "SamplesGame.h"
#include "FirstPersonCamera.h"
#include <imgui/imgui.h>
#include <brtshaderc/brtshaderc.h>
#include <bx/math.h>
#include <bx/timer.h>

using namespace gplay;


#define BATCH_BGFX

#define CUBE_DIM 22


//----------------------------------------------------------------------------------
// CUBE DATA
//----------------------------------------------------------------------------------

struct PosColorVertex
{
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;

    static void init()
    {
        ms_decl
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
            .end();
    }

    static bgfx::VertexDecl ms_decl;
};

bgfx::VertexDecl PosColorVertex::ms_decl;

static PosColorVertex s_cubeVertices[] =
{
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t s_cubeTriStrip[] =
{
    0, 1, 2,
    3,
    7,
    1,
    5,
    0,
    4,
    2,
    6,
    7,
    4,
    5,
};


const char* vshPath = "res/core/shaders/test/cubes.vert";
const char* fshPath = "res/core/shaders/test/cubes.frag";
const char* varyingDef = "res/core/shaders/test/cubes.io";


class BgfxBench : public Example
{
    Model* _cubeModel;

    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;
    bgfx::ProgramHandle m_program;
    bgfx::UniformHandle m_uniform;

    int64_t m_timeOffset;

public:

    BgfxBench()
    {
    }

    void finalize()
    {
#ifdef BATCH_BGFX
        finalizeBenchBGFX();
#else
        finalizeBenchGP3D();
#endif
    }

    void initialize()
    {
        m_timeOffset = bx::getHPCounter();

#ifdef BATCH_BGFX
        initBenchBGFX();
#else
        initBenchGP3D();
#endif
    }

    void update(float elapsedTime)
    {
#ifdef BATCH_BGFX
        updateBenchBGFX(elapsedTime);
#else
        updateBenchGP3D(elapsedTime);
#endif
    }

    void render(float elapsedTime)
    {
    }


    void finalizeBenchGP3D()
    {
        SAFE_RELEASE(_cubeModel);
    }

    void initBenchGP3D()
    {
        // Vertex format
        VertexFormat::Element elements[] =
        {
            VertexFormat::Element(VertexFormat::POSITION, 3),
            VertexFormat::Element(VertexFormat::COLOR, 4, VertexFormat::AttribType::Uint8, true)
        };

        unsigned int vertexCount = 8;
        unsigned int indexCount = 14;

        // Create mesh.
        Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 2), vertexCount);

        // Set vertices.
        mesh->setPrimitiveType(Mesh::TRIANGLES);
        mesh->setVertexData(s_cubeVertices, 0, vertexCount);

        // Set indices.
        MeshPart * part = mesh->addPart(Mesh::TRIANGLE_STRIP, Mesh::INDEX16, indexCount);
        part->setIndexData(s_cubeTriStrip, 0, indexCount);

        // create material
        Material* material = Material::create(vshPath, fshPath);
        material->getStateBlock()->setCullFace(true);
        material->getStateBlock()->setCullFaceSide(RenderState::CULL_FACE_SIDE_FRONT);
        material->getStateBlock()->setDepthWrite(true);
        material->getStateBlock()->setDepthTest(true);
        material->getStateBlock()->setDepthFunction(RenderState::DEPTH_LESS);

        // create model from mesh
        _cubeModel = Model::create(mesh);
        _cubeModel->setMaterial(material);

        SAFE_RELEASE(material);

    }

    void updateBenchGP3D(float elapsedTime)
    {
        float time = (float)( (bx::getHPCounter()-m_timeOffset)/double(bx::getHPFrequency() ) );

        Matrix projMatrix;
        Matrix::createPerspective(60.0f, float(getWidth())/float(getHeight()), 0.1f, 100.0f, &projMatrix);

        float at[3]  = { 0.0f, 0.0f,   0.0f };
        float eye[3] = { 0.0f, 0.0f, -75.0f };

        Matrix viewMatrix;
        Matrix::createLookAt(Vector3(eye), Vector3(at), Vector3::unitY(), &viewMatrix);

        Matrix mv = projMatrix * viewMatrix;
        Matrix mvp;

        // Submit 11x11 cubes.
        for (uint32_t yy = 0; yy < CUBE_DIM; ++yy)
        {
            for (uint32_t xx = 0; xx < CUBE_DIM; ++xx)
            {
                float mtx[16];
                bx::mtxRotateXY(mtx, time + xx*0.21f, time + yy*0.37f);
                mtx[12] = -15.0f + float(xx)*3.0f;
                mtx[13] = -15.0f + float(yy)*3.0f;
                mtx[14] = 0.0f;

                Matrix model(mtx);
                mvp = mv * model;
                _cubeModel->getMaterial()->getParameter("u_mvp")->setValue(mvp);

                _cubeModel->draw();
            }
        }
    }











    void finalizeBenchBGFX()
    {
        bgfx::destroy(m_ibh);
        bgfx::destroy(m_vbh);
        bgfx::destroy(m_program);
        bgfx::destroy(m_uniform);
    }

    void initBenchBGFX()
    {
        // Set view 0 clear state.
        bgfx::setViewClear(0
                           , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
                           , 0x303030ff
                           , 1.0f
                           , 0
                           );

        // Create vertex stream declaration.
        PosColorVertex::init();

        // Create static vertex buffer.
        m_vbh = bgfx::createVertexBuffer(
                    // Static data can be passed with bgfx::makeRef
                    bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) )
                    , PosColorVertex::ms_decl
                    );

        // Create static index buffer.
        m_ibh = bgfx::createIndexBuffer(
                    // Static data can be passed with bgfx::makeRef
                    bgfx::makeRef(s_cubeTriStrip, sizeof(s_cubeTriStrip) )
        );

        // create uniform mvp matrix
        m_uniform = bgfx::createUniform("u_mvp", bgfx::UniformType::Mat4);

        // compile shader
        const bgfx::Memory* memVsh = shaderc::compileShader(shaderc::ST_VERTEX, vshPath, nullptr, varyingDef);
        const bgfx::Memory* memFsh = shaderc::compileShader(shaderc::ST_FRAGMENT, fshPath, nullptr, varyingDef);
        GP_ASSERT(memVsh && memFsh);
        bgfx::ShaderHandle vsh = bgfx::createShader(memVsh);
        bgfx::ShaderHandle fsh = bgfx::createShader(memFsh);
        m_program = bgfx::createProgram(vsh, fsh, true);
    }

    void updateBenchBGFX(float elapsedTime)
    {
        float time = (float)( (bx::getHPCounter()-m_timeOffset)/double(bx::getHPFrequency() ) );

        float at[3]  = { 0.0f, 0.0f,   0.0f };
        float eye[3] = { 0.0f, 0.0f, -75.0f };

        float view[16];
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(getWidth())/float(getHeight()), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(getWidth()), uint16_t(getHeight()) );

        bgfx::touch(0);


        Matrix projMatrix(proj);
        Matrix viewMatrix(view);
        Matrix mv = projMatrix * viewMatrix;
        Matrix mvp;


        // Submit 11x11 cubes.
        for (uint32_t yy = 0; yy < CUBE_DIM; ++yy)
        {
            for (uint32_t xx = 0; xx < CUBE_DIM; ++xx)
            {
                float mtx[16];
                bx::mtxRotateXY(mtx, time + xx*0.21f, time + yy*0.37f);
                mtx[12] = -15.0f + float(xx)*3.0f;
                mtx[13] = -15.0f + float(yy)*3.0f;
                mtx[14] = 0.0f;


                // Set model matrix for rendering.
                //bgfx::setTransform(mtx);
                Matrix model(mtx);
                mvp = mv * model;
                bgfx::setUniform(m_uniform, &mvp);

                // Set vertex and index buffer.
                bgfx::setVertexBuffer(0, m_vbh);
                bgfx::setIndexBuffer(m_ibh);

                // Set render states.
                bgfx::setState(0
                               | BGFX_STATE_WRITE_RGB
                               | BGFX_STATE_WRITE_A
                               | BGFX_STATE_WRITE_Z
                               | BGFX_STATE_DEPTH_TEST_LESS
                               | BGFX_STATE_CULL_CW
                               | BGFX_STATE_MSAA
                               | BGFX_STATE_PT_TRISTRIP
                               );

                // Submit primitive for rendering to view 0.
                bgfx::submit(0, m_program);
            }
        }
    }


    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
    {
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
    }

};

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Benchmark", "Bgfx Bench", BgfxBench, 255);
#endif

