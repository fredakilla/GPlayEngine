#include "DynamicMesh.h"
#include "SamplesGame.h"

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Graphics", "Dyanmic Mesh", DynamicMeshUpdate, 2);
#endif

struct PosColorVertex
{
    Vector3 m_pos;
    Vector3 m_normal;
    Vector2 m_uv;
};

PosColorVertex vertices[] =
{
       // position             // normal                       // texcoord
    {  Vector3(-1, -1,  1),    Vector3(  0.0,  0.0,  1.0),     Vector2(0.0, 0.0)  },
    {  Vector3( 1, -1,  1),    Vector3(  0.0,  0.0,  1.0),     Vector2(1.0, 0.0)  },
    {  Vector3(-1,  1,  1),    Vector3(  0.0,  0.0,  1.0),     Vector2(0.0, 1.0)  },
    {  Vector3( 1,  1,  1),    Vector3(  0.0,  0.0,  1.0),     Vector2(1.0, 1.0)  },
    {  Vector3(-1,  1,  1),    Vector3(  0.0,  1.0,  0.0),     Vector2(0.0, 0.0)  },
    {  Vector3( 1,  1,  1),    Vector3(  0.0,  1.0,  0.0),     Vector2(1.0, 0.0)  },
    {  Vector3(-1,  1, -1),    Vector3(  0.0,  1.0,  0.0),     Vector2(0.0, 1.0)  },
    {  Vector3( 1,  1, -1),    Vector3(  0.0,  1.0,  0.0),     Vector2(1.0, 1.0)  },
    {  Vector3(-1,  1, -1),    Vector3(  0.0,  0.0, -1.0),     Vector2(0.0, 0.0)  },
    {  Vector3( 1,  1, -1),    Vector3(  0.0,  0.0, -1.0),     Vector2(1.0, 0.0)  },
    {  Vector3(-1, -1, -1),    Vector3(  0.0,  0.0, -1.0),     Vector2(0.0, 1.0)  },
    {  Vector3( 1, -1, -1),    Vector3(  0.0,  0.0, -1.0),     Vector2(1.0, 1.0)  },
    {  Vector3(-1, -1, -1),    Vector3(  0.0, -1.0,  0.0),     Vector2(0.0, 0.0)  },
    {  Vector3( 1, -1, -1),    Vector3(  0.0, -1.0,  0.0),     Vector2(1.0, 0.0)  },
    {  Vector3(-1, -1,  1),    Vector3(  0.0, -1.0,  0.0),     Vector2(0.0, 1.0)  },
    {  Vector3( 1, -1,  1),    Vector3(  0.0, -1.0,  0.0),     Vector2(1.0, 1.0)  },
    {  Vector3( 1, -1,  1),    Vector3(  1.0,  0.0,  0.0),     Vector2(0.0, 0.0)  },
    {  Vector3( 1, -1, -1),    Vector3(  1.0,  0.0,  0.0),     Vector2(1.0, 0.0)  },
    {  Vector3( 1,  1,  1),    Vector3(  1.0,  0.0,  0.0),     Vector2(0.0, 1.0)  },
    {  Vector3( 1,  1, -1),    Vector3(  1.0,  0.0,  0.0),     Vector2(1.0, 1.0)  },
    {  Vector3(-1, -1, -1),    Vector3( -1.0,  0.0,  0.0),     Vector2(0.0, 0.0)  },
    {  Vector3(-1, -1,  1),    Vector3( -1.0,  0.0,  0.0),     Vector2(1.0, 0.0)  },
    {  Vector3(-1,  1, -1),    Vector3( -1.0,  0.0,  0.0),     Vector2(0.0, 1.0)  },
    {  Vector3(-1,  1,  1),    Vector3( -1.0,  0.0,  0.0),     Vector2(1.0, 1.0)  }
};

static Mesh* createTexturedCube(float size = 1.0f)
{
    /*short indices[] =
    {
        0, 1, 2,
        2, 1, 3,
        4, 5, 6,
        6, 5, 7,
        8, 9, 10,
        10, 9, 11,
        12, 13, 14,
        14, 13, 15,
        16, 17, 18,
        18, 17, 19,
        20, 21, 22,
        22, 21, 23
    };*/

    unsigned int vertexCount = 24;
    unsigned int indexCount = 36;
    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::NORMAL, 3),
        VertexFormat::Element(VertexFormat::TEXCOORD0, 2)
    };
    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 3), vertexCount, true);
    if (mesh == NULL)
    {
        GP_ERROR("Failed to create mesh.");
        return NULL;
    }
    mesh->setVertexData(0, 0, vertexCount);

    MeshPart* meshPart = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX16, indexCount, true);
    meshPart->setIndexData(0, 0, indexCount);
    return mesh;
}


DynamicMeshUpdate::DynamicMeshUpdate()
    : _font(NULL)
    , _triangleModel(NULL)
    , _cubeModel(NULL)
    , _spinDirection(-1.0f)
{    
}

void DynamicMeshUpdate::initialize()
{
    // Create the font for drawing the framerate.
    _font = Font::create("res/core/ui/arial.gpb");

    // Create a perspective projection matrix.
    Matrix projMatrix;
    Matrix::createPerspective(45.0f, getWidth() / (float)getHeight(), 1.0f, 100.0f, &projMatrix);

    // Create a lookat view matrix.
    Matrix viewMatrix;
    Matrix::createLookAt(Vector3(3,2,-5), Vector3::zero(), Vector3::unitY(), &viewMatrix);

    // set mvp matrix
    _worldViewProjectionMatrix = projMatrix * viewMatrix;

    // Create a material
    Material* material = Material::create("res/core/shaders/forward/textured.vert", "res/core/shaders/forward/textured.frag");
    Texture::Sampler * sampler = Texture::Sampler::create("res/data/textures/brick.png");
    material->getParameter("u_diffuseTexture")->setValue(sampler);
    material->getParameter("u_worldViewProjectionMatrix")->setValue(_worldViewProjectionMatrix);
    material->getStateBlock()->setCullFace(true);
    material->getStateBlock()->setDepthTest(true);
    material->getStateBlock()->setDepthWrite(true);

    // Create a cube.
    Mesh* meshQuad = createTexturedCube();
    _cubeModel = Model::create(meshQuad);
    _cubeModel->setMaterial(material);
    SAFE_RELEASE(meshQuad);
}

void DynamicMeshUpdate::finalize()
{
    SAFE_RELEASE(_cubeModel);
    SAFE_RELEASE(_triangleModel);
    SAFE_RELEASE(_font);
}

void DynamicMeshUpdate::update(float elapsedTime)
{
    // Update the rotation of the cube.
    float dt = elapsedTime * 0.001f;
    float dx = _spinDirection * MATH_PI * 1.5f;
    float dy = _spinDirection * MATH_PI * 1.2f;
    float dz = _spinDirection * MATH_PI * 1.3f;
    Quaternion rot = Quaternion(Vector3(dx, dy, dz), dt);
    _worldViewProjectionMatrix.rotate(rot);


    // Update vertex buffer.

    float t = Game::getAbsoluteTime() * 0.001f;
    float phase = t * 10.0f;

    PosColorVertex * vbPtr = (PosColorVertex*)_cubeModel->getMesh()->mapVertexBuffer();
    //VertexBuffer * vb = (VertexBuffer *)_cubeModel->getMesh()->getVertexBuffer();
    //PosColorVertex * vbPtr = (PosColorVertex*)vb->lock(0, vb->getElementCount());
    if(vbPtr)
    {
        unsigned int vertexCount = _cubeModel->getMesh()->getVertexCount();
        for(int i=0; i<vertexCount; i++)
        {
            Vector3& src = vertices[i].m_pos;
            Vector3& dest = vbPtr->m_pos;
            dest.x = src.x * (1.0f + 0.12f * sin(phase));
            dest.y = src.y * (1.0f + 0.15f * sin(phase + 60.0f));
            dest.z = src.z * (1.0f + 0.08f * sin(phase + 120.0f));

            vbPtr->m_normal = vertices[i].m_normal;
            vbPtr->m_uv = vertices[i].m_uv;

            vbPtr++;
        }
    }
    //vb->unLock();
    _cubeModel->getMesh()->unmapVertexBuffer();


    // Update index buffer.
    // Rewrite always the same, no interest, but only for testing

    unsigned short * ibPtr = (unsigned short *)_cubeModel->getMesh()->getPart(0)->mapIndexBuffer();
    //IndexBuffer * ib = (IndexBuffer *)_cubeModel->getMesh()->getPart(0)->getIndexBuffer();
    //unsigned short * ibPtr = (unsigned short *)ib->lock(0, ib->getElementCount());
    if(ibPtr)
    {
        unsigned short indices[36] =
        {
            0, 1, 2,
            2, 1, 3,
            4, 5, 6,
            6, 5, 7,
            8, 9, 10,
            10, 9, 11,
            12, 13, 14,
            14, 13, 15,
            16, 17, 18,
            18, 17, 19,
            20, 21, 22,
            22, 21, 23
        };
        memcpy(ibPtr, indices, sizeof(unsigned short) * 36);
    }
    //ib->unLock();
    _cubeModel->getMesh()->getPart(0)->unmapIndexBuffer();
}

void DynamicMeshUpdate::render(float elapsedTime)
{
    // draw the cube
    _cubeModel->getMaterial()->getParameter("u_worldViewProjectionMatrix")->setValue(_worldViewProjectionMatrix);
    _cubeModel->draw();

    // draw frame rate
    drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
}

void DynamicMeshUpdate::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        // Reverse the spin direction if the user touches the screen.
        _spinDirection *= -1.0f;
        break;
    case Touch::TOUCH_RELEASE:
        break;
    case Touch::TOUCH_MOVE:
        break;
    };
}
