#include "gplay-engine.h"
#include "Example.h"
#include "SamplesGame.h"
#include "FirstPersonCamera.h"
#include <imgui/imgui.h>

using namespace gplay;

/**
 * Base sample for test about blending and transparency.
 */

#define FRAMEBUFFER_WIDTH 1024
#define FRAMEBUFFER_HEIGHT 1024

class Transparency : public Example
{
    FirstPersonCamera _fpCamera;
    Font* _font;
    Scene* _scene;

    FrameBuffer* _frameBuffer;
    Model* _quadModel;

public:

    Transparency()
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
        _fpCamera.initialize(0.1f, 100.0f);
        _fpCamera.setPosition(cameraPosition);
        _scene->addNode(_fpCamera.getRootNode());
        _scene->setActiveCamera(_fpCamera.getCamera());
        _scene->getActiveCamera()->setAspectRatio(getAspectRatio());


        // create views
        View::create(0, Game::getInstance()->getViewport(), View::ClearFlags::COLOR_DEPTH, 0x111122ff, 1.0f, 0);
        View::create(1, Rectangle(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT), View::ClearFlags::COLOR_DEPTH, 0x55555555, 1.0f, 0);

        // Create a framebuffer with rgba and depth textures
        Texture* texColor = Texture::create("targetColor", FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, Texture::Format::RGBA, Texture::Type::TEXTURE_RT);
        Texture* texDepth = Texture::create("targetDepth", FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, Texture::Format::D16, Texture::Type::TEXTURE_RT);
        std::vector<Texture*> textures;
        textures.push_back(texColor);
        textures.push_back(texDepth);
        _frameBuffer = FrameBuffer::create("MyFrameBuffer", textures);

        // Create a quad for framebuffer preview
        Mesh* mesh = Mesh::createQuad(0,0,256,256);
        _quadModel = Model::create(mesh);
        SAFE_RELEASE(mesh);
        _quadModel->setMaterial("res/core/shaders/debug.vert", "res/core/shaders/debug.frag", "SHOW_TEXTURE");
        _quadModel->getMaterial()->getParameter("u_texture")->setValue(_frameBuffer->getRenderTarget(0));



        // load box shape
        Bundle* bundle = Bundle::create("res/data/scenes/box.gpb");
        Mesh* meshBox = bundle->loadMesh("box_Mesh");
        SAFE_RELEASE(bundle);

        // Create a plane.
        Model* planeModel = Model::create(meshBox);
        planeModel->setMaterial("res/data/materials/box.material");
        Node* planeNode = Node::create("plane");
        planeNode->setDrawable(planeModel);
        planeNode->setScale(8.0f, 0.001f, 8.0f);
        _scene->addNode(planeNode);


        // Create a base colored material with alpha blending
        Material* material = Material::create("res/core/shaders/forward/colored.vert", "res/core/shaders/forward/colored.frag");
        material->setParameterAutoBinding("u_worldViewProjectionMatrix", "WORLD_VIEW_PROJECTION_MATRIX");
        material->getParameter("u_texture")->setValue(_frameBuffer->getRenderTarget(0));
        material->getStateBlock()->setCullFace(false);
        material->getStateBlock()->setDepthTest(true);
        material->getStateBlock()->setDepthWrite(true);
        material->getStateBlock()->setBlend(true);
        material->getStateBlock()->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
        material->getStateBlock()->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);



        // create cubes
        int maxDim = 4;
        float scale = 0.8f;
        float offset = maxDim / 2.0f * 0.8f;

        for(int y=0; y<maxDim; y++)
            for(int z=0; z<maxDim; z++)
                for(int x=0; x<maxDim; x++)
                {
                        Material* materialClone = material->clone();
                        materialClone->getParameter("u_diffuseColor")->setValue(Vector4(x/float(maxDim), y/float(maxDim), 1, 0.45));
                        materialClone->getStateBlock()->setBlend(true);
                        materialClone->getStateBlock()->setBlendSrc(RenderState::BLEND_SRC_ALPHA);
                        materialClone->getStateBlock()->setBlendDst(RenderState::BLEND_ONE_MINUS_SRC_ALPHA);

                        Model* model = Model::create(meshBox);
                        model->setMaterial(materialClone);
                        Node* node = Node::create("box");
                        node->setDrawable(model);
                        node->setScale(scale);
                        node->setTranslation(-offset+x, maxDim+1.0f-y, -offset+z);
                        _scene->addNode(node);
                }




        // Create 3 wide boxes
        {
            Material* materialClone = material->clone();
            materialClone->getParameter("u_diffuseColor")->setValue(Vector4(1.0f, 0.0f, 0.0, 0.3f));            

            Model* model = Model::create(meshBox);
            model->setMaterial(materialClone);
            Node* node = Node::create("box");
            node->setDrawable(model);
            node->setScale(6.0f, 1.0f, 0.25f);
            node->setTranslation(0, 0.5, 0);
            _scene->addNode(node);
        }

        {
            Material* materialClone = material->clone();
            materialClone->getParameter("u_diffuseColor")->setValue(Vector4(0.0f, 1.0f, 0.0, 0.3f));

            Model* model = Model::create(meshBox);
            model->setMaterial(materialClone);
            Node* node = Node::create("box");
            node->setDrawable(model);
            node->setScale(6.0f, 1.0f, 0.25f);
            node->setTranslation(0, 0.5, 0);
            node->rotate(Vector3(0,1,0), MATH_DEG_TO_RAD(40.0f));
            _scene->addNode(node);
        }

        {
            Material* materialClone = material->clone();
            materialClone->getParameter("u_diffuseColor")->setValue(Vector4(0.0f, 0.0f, 1.0, 0.3f));

            Model* model = Model::create(meshBox);
            model->setMaterial(materialClone);
            Node* node = Node::create("box");
            node->setDrawable(model);
            node->setScale(6.0f, 1.0f, 0.25f);
            node->setTranslation(0, 0.5, 0);
            node->rotate(Vector3(0,1,0), MATH_DEG_TO_RAD(-40.0f));
            _scene->addNode(node);
        }

        SAFE_RELEASE(material);
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
        // render scene in the frame buffer
        View::getView(1)->bind();
        _frameBuffer->bind();
        _scene->visit(this, &Transparency::drawScene);


        // render scene in main view
        View::getView(0)->bind();
        _scene->visit(this, &Transparency::drawScene);
        _quadModel->draw();
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
    }

    void keyEvent(Keyboard::KeyEvent evt, int key)
    {
        _fpCamera.keyEvent(evt, key);
    }

};

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Graphics", "Transparency", Transparency, 8)
#endif

