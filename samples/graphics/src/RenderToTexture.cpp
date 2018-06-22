#include "RenderToTexture.h"
#include "SamplesGame.h"

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Graphics", "Render to Texture", RenderToTexture, 5);
#endif


static Mesh* createTriangleMesh()
{
    // Calculate the vertices of the equilateral triangle.
    float a = 0.5f;     // length of the side
    Vector2 p1(0.0f,       a / sqrtf(3.0f));
    Vector2 p2(-a / 2.0f, -a / (2.0f * sqrtf(3.0f)));
    Vector2 p3( a / 2.0f, -a / (2.0f * sqrtf(3.0f)));

    // Create 3 vertices. Each vertex has position (x, y, z) and color (red, green, blue)
    float vertices[] =
    {
        p1.x, p1.y, 0.0f,     1.0f, 0.0f, 0.0f, 1.0f,
        p2.x, p2.y, 0.0f,     0.0f, 1.0f, 0.0f, 1.0f,
        p3.x, p3.y, 0.0f,     0.0f, 0.0f, 1.0f, 1.0f,
    };
    unsigned int vertexCount = 3;
    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::COLOR, 4)
    };
    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 2), vertexCount, false);
    if (mesh == NULL)
    {
        GP_ERROR("Failed to create mesh.");
        return NULL;
    }
    mesh->setPrimitiveType(Mesh::TRIANGLES);
    mesh->setVertexData(vertices, 0, vertexCount);
    return mesh;
}

static Mesh* createTexturedCube(float size = 1.0f)
{
    float a = size;
    float vertices[] =
    {
         // position     // normal              // texcoord
        -a, -a,  a,      0.0,  0.0,  1.0,       0.0, 0.0,
         a, -a,  a,      0.0,  0.0,  1.0,       1.0, 0.0,
        -a,  a,  a,      0.0,  0.0,  1.0,       0.0, 1.0,
         a,  a,  a,      0.0,  0.0,  1.0,       1.0, 1.0,
        -a,  a,  a,      0.0,  1.0,  0.0,       0.0, 0.0,
         a,  a,  a,      0.0,  1.0,  0.0,       1.0, 0.0,
        -a,  a, -a,      0.0,  1.0,  0.0,       0.0, 1.0,
         a,  a, -a,      0.0,  1.0,  0.0,       1.0, 1.0,
        -a,  a, -a,      0.0,  0.0, -1.0,       0.0, 0.0,
         a,  a, -a,      0.0,  0.0, -1.0,       1.0, 0.0,
        -a, -a, -a,      0.0,  0.0, -1.0,       0.0, 1.0,
         a, -a, -a,      0.0,  0.0, -1.0,       1.0, 1.0,
        -a, -a, -a,      0.0, -1.0,  0.0,       0.0, 0.0,
         a, -a, -a,      0.0, -1.0,  0.0,       1.0, 0.0,
        -a, -a,  a,      0.0, -1.0,  0.0,       0.0, 1.0,
         a, -a,  a,      0.0, -1.0,  0.0,       1.0, 1.0,
         a, -a,  a,      1.0,  0.0,  0.0,       0.0, 0.0,
         a, -a, -a,      1.0,  0.0,  0.0,       1.0, 0.0,
         a,  a,  a,      1.0,  0.0,  0.0,       0.0, 1.0,
         a,  a, -a,      1.0,  0.0,  0.0,       1.0, 1.0,
        -a, -a, -a,     -1.0,  0.0,  0.0,       0.0, 0.0,
        -a, -a,  a,     -1.0,  0.0,  0.0,       1.0, 0.0,
        -a,  a, -a,     -1.0,  0.0,  0.0,       0.0, 1.0,
        -a,  a,  a,     -1.0,  0.0,  0.0,       1.0, 1.0
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
    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 3), vertexCount, false);
    if (mesh == NULL)
    {
        GP_ERROR("Failed to create mesh.");
        return NULL;
    }
    mesh->setVertexData(vertices, 0, vertexCount);
    MeshPart* meshPart = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX16, indexCount, false);
    meshPart->setIndexData(indices, 0, indexCount);
    return mesh;
}


RenderToTexture::RenderToTexture()
    : _font(NULL)
    , _triangleModel(NULL)
    , _cubeModel(NULL)
    , _spinDirection(-1.0f)
{    
}



void RenderToTexture::initialize()
{
    // Create the font for drawing the framerate.
    _font = Font::create("res/core/ui/arial.gpb");


    // 1. Create a colored triangle

    // Create an orthographic projection matrix.
    float width = getWidth() / (float)getHeight();
    float height = 1.0f;
    Matrix::createOrthographic(width, height, -1.0f, 1.0f, &_worldViewProjectionMatrix);
    _worldViewProjectionMatrix.scale(1.5f);

    // Create a triangle mesh.
    Mesh* mesh = createTriangleMesh();
    _triangleModel = Model::create(mesh);
    SAFE_RELEASE(mesh);

    // Create a material for the triangle.
    Material* matTriangle = _triangleModel->setMaterial("res/core/shaders/forward/colored.vert", "res/core/shaders/forward/colored.frag", "VERTEX_COLOR");





    // 2. Create a frame buffer

    unsigned int BUFFER_SIZE = 512;

    Texture* texColor = Texture::create("targetColor", BUFFER_SIZE, BUFFER_SIZE, Texture::Format::RGBA, Texture::Type::TEXTURE_RT);
    //Texture* texDepth = Texture::create("targetDepth", BUFFER_SIZE, BUFFER_SIZE, Texture::Format::DEPTH, Texture::Type::TEXTURE_RT);

    std::vector<Texture*> textures;
    textures.push_back(texColor);
    //textures.push_back(texDepth);

    _frameBuffer = FrameBuffer::create("myFrameBuffer", textures);



    // 3. Create a textured cube

    // Create a perspective projection matrix.
    Matrix projMatrix;
    Matrix::createPerspective(45.0f, getWidth() / (float)getHeight(), 1.0f, 100.0f, &projMatrix);

    // Create a lookat view matrix.
    Matrix viewMatrix;
    Matrix::createLookAt(Vector3(3,2,-5), Vector3::zero(), Vector3::unitY(), &viewMatrix);

    // mvp matrix of the cube
    Matrix mvp = projMatrix * viewMatrix;

    // inverse transpose matrix (used for lights)
    Matrix invTransWorldMatrix = mvp;
    invTransWorldMatrix.invert();
    invTransWorldMatrix.transpose();


    // create a material for the cube
    Material* material = Material::create("res/core/shaders/forward/textured.vert", "res/core/shaders/forward/textured.frag", "DIRECTIONAL_LIGHT_COUNT=1");

    // use the renderTarget of the framebuffer as texture sampler for this material
    Texture::Sampler* sampler = _frameBuffer->getRenderTarget("targetColor");

    material->getParameter("u_worldViewProjectionMatrix")->setValue(mvp);
    material->getParameter("u_inverseTransposeWorldViewMatrix")->setValue(invTransWorldMatrix);
    material->getParameter("u_ambientColor")->setValue(Vector3(0.2f, 0.2f, 0.2f));
    material->getParameter("u_directionalLightColor[0]")->setValue(Vector3(1.0f, 1.0f, 1.0f));
    material->getParameter("u_directionalLightDirection[0]")->setValue(Vector3(0.2,-0.8,0.3));
    material->getParameter("u_diffuseTexture")->setValue(sampler);
    material->getStateBlock()->setCullFace(true);
    material->getStateBlock()->setDepthTest(true);
    material->getStateBlock()->setDepthWrite(true);

    // Create a cube mesh
    Mesh* meshCube = createTexturedCube();
    _cubeModel = Model::create(meshCube);
    _cubeModel->setMaterial(material);
    SAFE_RELEASE(meshCube);



    // Set views
    View::create(0, Game::getInstance()->getViewport(), View::ClearFlags::COLOR_DEPTH, 0x111122ff, 1.0f, 0);
    View::create(1, Rectangle(BUFFER_SIZE, BUFFER_SIZE), View::ClearFlags::COLOR_DEPTH, 0xaaaaaaff, 1.0f, 0);
}

void RenderToTexture::finalize()
{
    SAFE_RELEASE(_cubeModel);
    SAFE_RELEASE(_triangleModel);
    SAFE_RELEASE(_font);
}

void RenderToTexture::update(float elapsedTime)
{
    // Update the rotation of the triangle. The speed is 180 degrees per second.
    _worldViewProjectionMatrix.rotateZ( _spinDirection * MATH_PI * elapsedTime * 0.001f);
    _triangleModel->getMaterial()->getParameter("u_worldViewProjectionMatrix")->setValue(_worldViewProjectionMatrix);
}

void RenderToTexture::render(float elapsedTime)
{
    // 1. render the triangle
    View::getView(1)->bind();
    _frameBuffer->bind();
    _triangleModel->draw();

    // 2. render the cube
    View::getView(0)->bind();
    _cubeModel->draw();

    // draw frame rate
    drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
}

void RenderToTexture::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
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
