#include "ImGuiTest.h"
#include "SamplesGame.h"
#include <dear-imgui/imgui.h>

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Graphics", "ImGui", ImGuiTest, 6);
#endif


static Mesh* createTexturedCube(float size = 1.0f)
{
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


    short indices[] =
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
    mesh->setVertexData(vertices, 0, vertexCount);

    MeshPart* meshPart = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX16, indexCount, true);
    meshPart->setIndexData(indices, 0, indexCount);
    return mesh;
}




ImGuiTest::ImGuiTest()
    : _font(NULL)
    , _triangleModel(NULL)
    , _cubeModel(NULL)
    , _spinDirection(-1.0f)
{    
}

void ImGuiTest::initialize()
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

void ImGuiTest::finalize()
{
    SAFE_RELEASE(_cubeModel);
    SAFE_RELEASE(_triangleModel);
    SAFE_RELEASE(_font);
}

void ImGuiTest::update(float elapsedTime)
{
    // Show ImGui test window
    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    ImGui::ShowDemoWindow();

    // Create some ImGui controls to manage cube rotation
    static float axis[] = { 0.2f, 0.4f, 0.3f };
    static float speed = { 0.5f };
    ImGui::SetNextWindowSize(ImVec2(200,200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Cube Controls");
    ImGui::SliderFloat3("Axis", axis, 0.0f, 1.0f);
    ImGui::SliderFloat("Speed", &speed, -10.0f, 10.0f);
    ImGui::End();


    // Update the rotation of the cube with ImGui controls.
    float dt = elapsedTime * 0.001f;
    float dx = _spinDirection * axis[0];
    float dy = _spinDirection * axis[1];
    float dz = _spinDirection * axis[2];
    Quaternion rot = Quaternion(Vector3(dx, dy, dz), speed * dt);
    _worldViewProjectionMatrix.rotate(rot);
}

void ImGuiTest::render(float elapsedTime)
{
    // draw the cube
    _cubeModel->getMaterial()->getParameter("u_worldViewProjectionMatrix")->setValue(_worldViewProjectionMatrix);
    _cubeModel->draw();

    // draw frame rate
    drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
}

void ImGuiTest::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
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


