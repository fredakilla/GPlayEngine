#ifndef GP_NO_SPARK

#include "SparkDemo.h"
#include "SamplesGame.h"
#include "sparkparticles/SparkQuadRenderer.h"
#include "sparkparticles/SparkParticleEmitter.h"
#include <spark/SPARK.h>
#include <dear-imgui/imgui.h>

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Graphics", "Spark Demo", SparkDemo, 7);
#endif


static Mesh* createCubeMesh(float size = 1.0f)
{
    float a = size * 0.5f;
    float vertices[] =
    {
        -a, -a,  a,    0.0,  0.0,  1.0,   0.0, 0.0,
         a, -a,  a,    0.0,  0.0,  1.0,   1.0, 0.0,
        -a,  a,  a,    0.0,  0.0,  1.0,   0.0, 1.0,
         a,  a,  a,    0.0,  0.0,  1.0,   1.0, 1.0,
        -a,  a,  a,    0.0,  1.0,  0.0,   0.0, 0.0,
         a,  a,  a,    0.0,  1.0,  0.0,   1.0, 0.0,
        -a,  a, -a,    0.0,  1.0,  0.0,   0.0, 1.0,
         a,  a, -a,    0.0,  1.0,  0.0,   1.0, 1.0,
        -a,  a, -a,    0.0,  0.0, -1.0,   0.0, 0.0,
         a,  a, -a,    0.0,  0.0, -1.0,   1.0, 0.0,
        -a, -a, -a,    0.0,  0.0, -1.0,   0.0, 1.0,
         a, -a, -a,    0.0,  0.0, -1.0,   1.0, 1.0,
        -a, -a, -a,    0.0, -1.0,  0.0,   0.0, 0.0,
         a, -a, -a,    0.0, -1.0,  0.0,   1.0, 0.0,
        -a, -a,  a,    0.0, -1.0,  0.0,   0.0, 1.0,
         a, -a,  a,    0.0, -1.0,  0.0,   1.0, 1.0,
         a, -a,  a,    1.0,  0.0,  0.0,   0.0, 0.0,
         a, -a, -a,    1.0,  0.0,  0.0,   1.0, 0.0,
         a,  a,  a,    1.0,  0.0,  0.0,   0.0, 1.0,
         a,  a, -a,    1.0,  0.0,  0.0,   1.0, 1.0,
        -a, -a, -a,   -1.0,  0.0,  0.0,   0.0, 0.0,
        -a, -a,  a,   -1.0,  0.0,  0.0,   1.0, 0.0,
        -a,  a, -a,   -1.0,  0.0,  0.0,   0.0, 1.0,
        -a,  a,  a,   -1.0,  0.0,  0.0,   1.0, 1.0
    };
    short indices[] =
    {
        0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 5, 7, 8, 9, 10, 10, 9, 11, 12, 13, 14, 14, 13, 15, 16, 17, 18, 18, 17, 19, 20, 21, 22, 22, 21, 23
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

SparkDemo::SparkDemo()
    : _font(NULL), _scene(NULL), _cubeNode(NULL)
{
}

void SparkDemo::initialize()
{
    // Create the font for drawing the framerate.
    _font = Font::create("res/core/ui/arial.gpb");

    // Create a new empty scene.
    _scene = Scene::create();

    // set fps camera
    Vector3 cameraPosition(0, 2, 5);
    _fpCamera.initialize(1.0, 100000.0f);
    _fpCamera.setPosition(cameraPosition);
    _scene->addNode(_fpCamera.getRootNode());
    _scene->setActiveCamera(_fpCamera.getCamera());
    _scene->getActiveCamera()->setAspectRatio(getAspectRatio());


    // Create a white light.
    Light* light = Light::createDirectional(0.75f, 0.75f, 0.75f);
    Node* lightNode = _scene->addNode("light");
    lightNode->setLight(light);
    // Release the light because the node now holds a reference to it.
    SAFE_RELEASE(light);
    lightNode->rotateX(MATH_DEG_TO_RAD(-45.0f));


    // Create a cube mesh for next models.
    Mesh* cubeMesh = createCubeMesh();

    // Create a plane from a flattened cube mesh
    {
        Model* planeModel = Model::create(cubeMesh);

        // Create a simple textured material for the plane
        Material* material = planeModel->setMaterial("res/core/shaders/forward/textured.vert", "res/core/shaders/forward/textured.frag");
        material->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");

        // Load the texture from file.
        Texture::Sampler* sampler = material->getParameter("u_diffuseTexture")->setValue("res/data/textures/dirt.png", true);
        sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);

        Node* planeNode = _scene->addNode("plane");
        planeNode->setDrawable(planeModel);
        planeNode->setScale(Vector3(5.0f, 0.001f, 5.0f));
        SAFE_RELEASE(planeModel);
    }

    // Create a small cube
    {
        Model* cubeModel = Model::create(cubeMesh);

        // Create a material with a directional light for the cube
        Material* material = cubeModel->setMaterial("res/core/shaders/forward/textured.vert", "res/core/shaders/forward/textured.frag", "DIRECTIONAL_LIGHT_COUNT=1");
        material->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
        material->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", "INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX");
        material->getParameter("u_ambientColor")->setValue(Vector3(0.2f, 0.2f, 0.2f));
        material->getParameter("u_directionalLightColor[0]")->setValue(lightNode->getLight()->getColor());
        material->getParameter("u_directionalLightDirection[0]")->bindValue(lightNode, &Node::getForwardVectorWorld);
        Texture::Sampler* sampler = material->getParameter("u_diffuseTexture")->setValue("res/data/textures/brick.png", true);
        sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);
        material->getStateBlock()->setCullFace(true);
        material->getStateBlock()->setDepthTest(true);
        material->getStateBlock()->setDepthWrite(true);

        _cubeNode = _scene->addNode("cube");
        _cubeNode->setDrawable(cubeModel);
        _cubeNode->setScale(Vector3(0.25, 0.25, 0.25));
        SAFE_RELEASE(cubeModel);

    }

    // we don't need more the cube mesh.
    SAFE_RELEASE(cubeMesh);




    // Create a material for particles
    _materialParticle = Material::create("res/core/shaders/particle.vert", "res/core/shaders/particle.frag");
    Texture::Sampler* sampler2 = _materialParticle->getParameter("u_diffuseTexture")->setValue("res/data/textures/flare.png", true);
    sampler2->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);
    _materialParticle->getStateBlock()->setCullFace(true);
    _materialParticle->getStateBlock()->setDepthTest(true);
    _materialParticle->getStateBlock()->setDepthWrite(false);
    _materialParticle->getStateBlock()->setBlend(true);
    _materialParticle->getStateBlock()->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
    _materialParticle->getStateBlock()->setBlendDst(RenderState::BLEND_ONE);


    // Create fountain particle effect using spark library
    SPK::Ref<SPK::System> spkEffectFountain = SPK::System::create(true);
    spkEffectFountain->setName("Foutain");
    {
        // Renderer
        SPK::Ref<SPK::GP3D::SparkQuadRenderer> renderer = SPK::GP3D::SparkQuadRenderer::create();
        renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        renderer->setScale(0.1f,0.1f);
        renderer->setMaterial(_materialParticle);
        renderer->setOrientation(SPK::OrientationPreset::CAMERA_PLANE_ALIGNED);

        // Emitter
        SPK::Ref<SPK::SphericEmitter> particleEmitter = SPK::SphericEmitter::create(SPK::Vector3D(0.0f,1.0f,0.0f),0.1f * MATH_PI, 0.1f * MATH_PI);
        particleEmitter->setZone(SPK::Point::create(SPK::Vector3D(0.0f,0.015f,0.0f)));
        particleEmitter->setFlow(150);
        particleEmitter->setForce(1.5f,1.5f);

        // Obstacle
        SPK::Ref<SPK::Plane> groundPlane = SPK::Plane::create();
        SPK::Ref<SPK::Obstacle> obstacle = SPK::Obstacle::create(groundPlane,0.9f,1.0f);
        obstacle->setBouncingRatio(0.6f);
        obstacle->setFriction(1.0f);

        // Group
        SPK::Ref<SPK::Group> particleGroup = spkEffectFountain->createGroup(2500);
        particleGroup->addEmitter(particleEmitter);
        particleGroup->addModifier(obstacle);
        particleGroup->setRenderer(renderer);
        particleGroup->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f,-1.0f,0.0f)));
        particleGroup->setLifeTime(14.2f, 14.5f);
        particleGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xff0000ff,0x0000ffff));
        particleGroup->enableSorting(true);
    }


    // Create fountain particle effect using spark library
    SPK::Ref<SPK::System> spkEffectTrail = SPK::System::create(true);
    spkEffectTrail->setName("Trail");
    {
        // Renderer
        SPK::Ref<SPK::GP3D::SparkQuadRenderer> renderer = SPK::GP3D::SparkQuadRenderer::create();
        renderer->setTexturingMode(SPK::TEXTURE_MODE_2D);
        renderer->setScale(0.1f,0.1f);
        renderer->setMaterial(_materialParticle);
        renderer->setOrientation(SPK::OrientationPreset::CAMERA_PLANE_ALIGNED);

        // Emitter
        SPK::Ref<SPK::SphericEmitter> particleEmitter = SPK::SphericEmitter::create(SPK::Vector3D(0.0f,1.0f,0.0f),0.1f * MATH_PI, 0.1f * MATH_PI);
        particleEmitter->setZone(SPK::Point::create(SPK::Vector3D(0.0f,0.015f,0.0f)));
        particleEmitter->setFlow(50);
        particleEmitter->setForce(1.5f,1.5f);

        // Obstacle
        /*SPK::Ref<SPK::Plane> groundPlane = SPK::Plane::create();
        SPK::Ref<SPK::Obstacle> obstacle = SPK::Obstacle::create(groundPlane,0.9f,1.0f);
        obstacle->setBouncingRatio(0.6f);
        obstacle->setFriction(1.0f);*/

        // Group
        SPK::Ref<SPK::Group> particleGroup = spkEffectTrail->createGroup(2500);
        particleGroup->addEmitter(particleEmitter);
        //particleGroup->addModifier(obstacle);
        particleGroup->setRenderer(renderer);
        //particleGroup->addModifier(SPK::Gravity::create(SPK::Vector3D(0.0f,-1.0f,0.0f)));
        particleGroup->setLifeTime(4.2f, 4.5f);
        particleGroup->setColorInterpolator(SPK::ColorSimpleInterpolator::create(0xffff00ff,0xff00ffff));
        particleGroup->enableSorting(false);
    }


    // Create a node in scene and attach spark foutain effect
    SparkParticleEmitter* foutainEmitter = SparkParticleEmitter::create(spkEffectFountain, true);
    Node* particleNode = _scene->addNode("sparkFoutain");
    particleNode->setDrawable(foutainEmitter);
    particleNode->setTranslation(0.0f, 0.1f, 0.0f);


    // Create a node and attach trail foutain effect
    SparkParticleEmitter* trailEmitter = SparkParticleEmitter::create(spkEffectTrail, true);
    Node* trailNode = Node::create("sparkTrail");
    trailNode->setDrawable(trailEmitter);
    trailNode->setTranslation(0.0f, 0.8f, 0.0f);

    // set this last node a child of the cube node to move with the cube.
    _cubeNode->addChild(trailNode);
}

void SparkDemo::finalize()
{
    SAFE_RELEASE(_font);
    SAFE_RELEASE(_scene);    
}

void SparkDemo::render(float elapsedTime)
{
    // Clear the color and depth buffers
    clear(CLEAR_COLOR_DEPTH, 0.25f, 0.25f, 0.4f, 1.0f, 1.0f, 0);

    // Visit all the nodes in the scene, drawing the models.
    _scene->visit(this, &SparkDemo::drawScene);

    drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
}

void SparkDemo::showToolbox()
{
    ImGui::Begin("Particle material");

    static bool depthTest = true;
    static bool depthWrite = false;

    ImGui::Checkbox("Depth test", &depthTest);
    ImGui::Checkbox("Depth write", &depthWrite);

    _materialParticle->getStateBlock()->setDepthTest(depthTest);
    _materialParticle->getStateBlock()->setDepthWrite(depthWrite);


    static bool usePreset = true;
    ImGui::Checkbox("Use blend presets", &usePreset);

    if(usePreset)
    {
        static int blend_preset = 2;
        const char* blendPredefinedItems[] = { "None", "Alpha", "Additive", "Multiplied" };
        ImGui::Combo("Blend preset", &blend_preset, blendPredefinedItems, IM_ARRAYSIZE(blendPredefinedItems));

        switch(blend_preset)
        {
        default:
        case 0: // None
            _materialParticle->getStateBlock()->setBlend(false);
            break;
        case 1: // Alpha
            _materialParticle->getStateBlock()->setBlend(true);
            _materialParticle->getStateBlock()->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
            _materialParticle->getStateBlock()->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);
            break;
        case 2: // Additive
            _materialParticle->getStateBlock()->setBlend(true);
            _materialParticle->getStateBlock()->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
            _materialParticle->getStateBlock()->setBlendDst(RenderState::BLEND_ONE);
            break;
        case 3: // Multiplied
            _materialParticle->getStateBlock()->setBlend(true);
            _materialParticle->getStateBlock()->setBlendSrc(RenderState::BLEND_ZERO);
            _materialParticle->getStateBlock()->setBlendDst(RenderState::BLEND_SRC_COLOR);
            break;
        }
    }
    else
    {
        const char* blendItems[] = {
            "BLEND_ZERO",
            "BLEND_ONE",
            "BLEND_SRC_COLOR",
            "BLEND_ONE_MINUS_SRC_COLOR",
            "BLEND_DST_COLOR",
            "BLEND_ONE_MINUS_DST_COLOR",
            "BLEND_SRC_ALPHA",
            "BLEND_ONE_MINUS_SRC_ALPHA",
            "BLEND_DST_ALPHA",
            "BLEND_ONE_MINUS_DST_ALPHA",
            "BLEND_CONSTANT_ALPHA",
            "BLEND_ONE_MINUS_CONSTANT_ALPHA",
            "BLEND_SRC_ALPHA_SATURATE"
        };

        static int custom_blend_src = 0;
        static int custom_blend_dst = 0;
        ImGui::Combo("BlendSrc", &custom_blend_src, blendItems, IM_ARRAYSIZE(blendItems));
        ImGui::Combo("BlendDst", &custom_blend_dst, blendItems, IM_ARRAYSIZE(blendItems));

        _materialParticle->getStateBlock()->setBlend(true);
        _materialParticle->getStateBlock()->setBlendSrc(RenderState::Blend(custom_blend_src));
        _materialParticle->getStateBlock()->setBlendDst(RenderState::Blend(custom_blend_dst));
    }

    ImGui::End();
}

void SparkDemo::update(float elapsedTime)
{
    // show toolbox
    showToolbox();

    // update camera
    _fpCamera.updateCamera(elapsedTime);   

    // Visit all the nodes in the scene, to update nodes with particles drawable.
    // TODO: change this to update in another way
    _scene->visit(this, &SparkDemo::updateEmitters, elapsedTime);


    // compute acculumulation time for a linear movement
    static float accuTime = 0;
    accuTime += elapsedTime * 0.001f;

    // compute rotation and translation values
    float speed = 0.85 * accuTime;
    float radius = 1.0f;
    Vector3 axis(0.27f, 0.34f, 0.19f);
    float angle = accuTime;

    // move the small cube around a circle
    Vector3 scale = _cubeNode->getScale();
    Quaternion rotate(axis, angle);
    Vector3 translate(sin(speed) * radius, 1.0f, cos(speed) * radius);
    _cubeNode->set(scale, rotate, translate);
}

bool SparkDemo::updateEmitters(Node* node, float elapsedTime)
{
    SparkParticleEmitter* spkEmitter = dynamic_cast<SparkParticleEmitter*>(node->getDrawable());
    if (spkEmitter)
        spkEmitter->update(elapsedTime);
    return true;
}

bool SparkDemo::drawScene(Node* node)
{
    Drawable* drawable = node->getDrawable();
    if (drawable)
        drawable->draw();
    return true;
}

void SparkDemo::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
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

void SparkDemo::keyEvent(Keyboard::KeyEvent evt, int key)
{
    _fpCamera.keyEvent(evt, key);
}

#endif
