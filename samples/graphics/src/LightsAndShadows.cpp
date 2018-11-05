#include "gplay-engine.h"
#include "Example.h"
#include "SamplesGame.h"
#include "FirstPersonCamera.h"
#include <dear-imgui/imgui.h>
#include <core/FileWatcher.h>

using namespace gplay;


class LightManager2
{
public:

    void setScene(Scene* scene)
    {
        _scene = scene;
    }

    void addDirectionnalLight(Light* light, const char* id = nullptr)
    {
        createLightNode(light);
        _dirLights.push_back(light);
    }

    void addPointLight(Light* light, const char* id = nullptr)
    {
        createLightNode(light);
        _pointLights.push_back(light);
    }

    void addSpotLight(Light* light, const char* id = nullptr)
    {
        createLightNode(light);
        _spotLights.push_back(light);
    }

    void initializeMaterials()
    {
         _scene->visit(this, &LightManager2::applyLightForNode);
    }

    bool applyLightForNode(Node* node)
    {
        Model* model = dynamic_cast<Model*>(node->getDrawable());
        if (model)
        {
            for(unsigned int i=0; i<model->getMeshPartCount(); i++)
            {
                Material* material = model->getMaterial(i);
                if(material)
                    applyLightForMaterial(material);
            }

        }
        return true;
    }

    void applyLightForMaterial(Material* material)
    {
        material->setParameterAutoBinding("u_worldViewProjectionMatrix", RenderState::WORLD_VIEW_PROJECTION_MATRIX);
        material->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", RenderState::INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX);
        material->setParameterAutoBinding("u_viewMatrix", RenderState::VIEW_MATRIX);
        material->setParameterAutoBinding("u_worldMatrix", RenderState::WORLD_MATRIX);
        material->setParameterAutoBinding("u_worldViewMatrix", RenderState::WORLD_VIEW_MATRIX);
        material->setParameterAutoBinding("u_cameraPosition", RenderState::CAMERA_WORLD_POSITION);

        material->getParameter("u_ambientColor")->bindValue(this, &LightManager2::getAmbientColor);
        material->getParameter("u_shininess")->setValue(0.5f);

        for(unsigned int i=0; i<_dirLights.size(); ++i)
        {
            material->getParameter(formatName("u_directionalLightDirection", i).c_str())->bindValue(_dirLights[i]->getNode(), &Node::getForwardVectorView);
            material->getParameter(formatName("u_directionalLightColor", i).c_str())->bindValue(_dirLights[i], &Light::getColor);
        }

        for(unsigned int i=0; i<_pointLights.size(); ++i)
        {
            material->getParameter(formatName("u_pointLightPosition", i).c_str())->bindValue(_pointLights[i]->getNode(), &Node::getTranslationView);
            material->getParameter(formatName("u_pointLightColor", i).c_str())->bindValue(_pointLights[i], &Light::getColor);
            material->getParameter(formatName("u_pointLightAttenuation", i).c_str())->bindValue(_pointLights[i], &Light::getAttenuation);
        }

        for(unsigned int i=0; i<_spotLights.size(); ++i)
        {
            material->getParameter(formatName("u_spotLightPosition", i).c_str())->bindValue(_spotLights[i]->getNode(), &Node::getTranslationView);
            material->getParameter(formatName("u_spotLightDirection", i).c_str())->bindValue(_spotLights[i]->getNode(), &Node::getForwardVectorView);
            material->getParameter(formatName("u_spotLightColor", i).c_str())->bindValue(_spotLights[i], &Light::getColor);
            material->getParameter(formatName("u_spotLightAttenuation", i).c_str())->bindValue(_spotLights[i], &Light::getAttenuation);
        }
    }

    void setAmbientColor(Vector3 ambient)
    {
        _ambientColor = ambient;
    }

    const Vector3 getAmbientColor() const
    {
        return _ambientColor;
    }


    Light* getDirectionnalLight(unsigned int index)
    {
        GP_ASSERT(index < _dirLights.size());
        return _dirLights[index];
    }

    Light* getPointLight(unsigned int index)
    {
        GP_ASSERT(index < _pointLights.size());
        return _pointLights[index];
    }

    Light* getSpotLight(unsigned int index)
    {
        GP_ASSERT(index < _spotLights.size());
        return _spotLights[index];
    }

private:

    std::string formatName(const char* name, unsigned int index)
    {
        std::string s(name);
        s += "[";
        s += std::to_string(index);
        s += "]";
        return s;
    }

    void createLightNode(Light* light, const char* id = nullptr)
    {
        Node* node = Node::create(id);
        node->setLight(light);
        _scene->addNode(node);
    }

    Scene* _scene;
    Vector3 _ambientColor;
    float _shininess;
    std::vector<Light*> _dirLights;
    std::vector<Light*> _pointLights;
    std::vector<Light*> _spotLights;
};







class LightsAndShadows : public Example
{
    FirstPersonCamera _fpCamera;
    Font* _font;
    Scene* _scene;
    LightManager2 _lightManager;
    Node* _node;
    Model* _quadModel;
    FrameBuffer* _frameBuffer;
    Matrix _lightSpaceMatrix;
    Vector4 _shadowProjection;

public:

    LightsAndShadows()
        : _font(nullptr), _scene(nullptr), _node(nullptr)
    {
        _shadowProjection = Vector4(-10,10,-10,10);
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
        _fpCamera.initialize(0.01f, 10000.0f);
        _fpCamera.setPosition(cameraPosition);
        _scene->addNode(_fpCamera.getRootNode());
        _scene->setActiveCamera(_fpCamera.getCamera());
        _scene->getActiveCamera()->setAspectRatio(getAspectRatio());




#define FRAMEBUFFER_WIDTH 1024
#define FRAMEBUFFER_HEIGHT 1024

        // create views
        Game * game = Game::getInstance();

        View::create(0, Rectangle(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT), View::ClearFlags::COLOR_DEPTH, 0x111122ff, 1.0f, 0);
        View::create(1, Game::getInstance()->getViewport(), View::ClearFlags::COLOR_DEPTH, 0x303030ff, 1.0f, 0);


        // Create a framebuffer with a depth texture
        Texture::TextureInfo texInfo;
        texInfo.id = "targetDepth";
        texInfo.width = FRAMEBUFFER_WIDTH;
        texInfo.height = FRAMEBUFFER_HEIGHT;
        texInfo.type = Texture::TEXTURE_RT;
        texInfo.format = Texture::Format::D16;
        texInfo.flags = BGFX_TEXTURE_RT ;
        Texture* texDepth = Texture::create(texInfo);

        std::vector<Texture*> textures;
        textures.push_back(texDepth);
        //_frameBuffer2 = FrameBuffer::create("ShadowFrameBuffer", textures);
        _frameBuffer = FrameBuffer::create("ShadowFrameBuffer", FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, Texture::Format::D24);





        // material

        Material* material = Material::create("res/data/materials/test.material");
        /*material->getStateBlock()->setCullFace(false);
        material->getStateBlock()->setDepthTest(true);
        material->getStateBlock()->setDepthWrite(true);*/

        Texture::Sampler* sampler = material->getParameter("u_diffuseTexture")->setValue("res/data/textures/brick.png", true);
        sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);

        Texture::Sampler* bumpSampler = material->getParameter("u_normalTexture")->setValue("res/data/textures/brickn.png", true);
        bumpSampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);

        Texture::Sampler* specSampler = material->getParameter("u_specularTexture")->setValue("res/data/textures/spec.png", true);
        specSampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);

        Texture::Sampler* shadowSampler = _frameBuffer->getRenderTarget(0);
        shadowSampler->setWrapMode(Texture::BORDER, Texture::BORDER);
        material->getParameter("s_shadowMap")->setSampler(shadowSampler);






        // load shapes bundle
        Bundle* bundle = Bundle::create("res/data/scenes/shapes.gpb");

        // create a plane
        Model* modelPlane = Model::create(bundle->loadMesh("Plane_Mesh"));
        modelPlane->setMaterial(material->clone());
        Node* nodePlane = Node::create("plane");
        nodePlane->setDrawable(modelPlane);
        nodePlane->setScale(5.0f);
        _scene->addNode(nodePlane);

        // create a teapot
        Model* modelTeapot = Model::create(bundle->loadMesh("Cube_Mesh"));
        modelTeapot->setMaterial(material->clone());
        Node* nodeTeapot = Node::create("cube");
        nodeTeapot->setDrawable(modelTeapot);
        nodeTeapot->setScale(1.0f);
        nodeTeapot->setTranslation(0, 1, 0);
        _scene->addNode(nodeTeapot);

        // create a torus
        Model* modelTorus = Model::create(bundle->loadMesh("Torus_Mesh"));
        modelTorus->setMaterial(material->clone());
        Node* nodeTorus = Node::create("torus");
        nodeTorus->setDrawable(modelTorus);
        nodeTorus->setScale(0.5f);
        nodeTorus->setTranslation(-2, 0.5, -2);
        _scene->addNode(nodeTorus);

        // create a torus knot
        Model* modelTorusKnot = Model::create(bundle->loadMesh("TorusKnot_Mesh"));
        modelTorusKnot->setMaterial(material->clone());
        Node* nodeTorusKnot = Node::create("torusKnot");
        nodeTorusKnot->setDrawable(modelTorusKnot);
        nodeTorusKnot->setScale(0.25f);
        nodeTorusKnot->setTranslation(2, 0.5, -2);
        _scene->addNode(nodeTorusKnot);

        // create a cone
       Model* modelCone = Model::create(bundle->loadMesh("Cone_Mesh"));
        modelCone->setMaterial(material->clone());
        Node* nodeCone = Node::create("cone");
        nodeCone->setDrawable(modelCone);
        nodeCone->setScale(0.5f);
        nodeCone->setTranslation(2, 0.5, 2);
        _scene->addNode(nodeCone);

        // create suzanne
        Model* modelMonkey = Model::create(bundle->loadMesh("Monkey_Mesh"));
        modelMonkey->setMaterial(material->clone());
        Node* nodeMonkey = Node::create("monkey");
        nodeMonkey->setDrawable(modelMonkey);
        nodeMonkey->setScale(0.5f);
        nodeMonkey->setTranslation(-2, 0.5, 2);
        _scene->addNode(nodeMonkey);


        // Use a light manager to declare lights
        _lightManager.setScene(_scene);
        _lightManager.setAmbientColor(Vector3(0.3, 0.3, 0.3));
        _lightManager.addDirectionnalLight(Light::createDirectional(Vector3(0.5, 0.5, 0.2)));
        _lightManager.addPointLight(Light::createPoint(Vector3(0.0, 0.0, 1.0), 50));
        _lightManager.addSpotLight(Light::createSpot(Vector3(0.0, 0.0, 1.0), 50, MATH_DEG_TO_RAD(30.0f),  MATH_DEG_TO_RAD(45.0f)));
        _lightManager.initializeMaterials();

        _lightManager.getDirectionnalLight(0)->setColor(Vector3(1,1,1));


        _node = nodeMonkey;


        SAFE_RELEASE(bundle);
        SAFE_RELEASE(material);


        // Add watcher to shader directory and bind event to be notified on changes
        FileWatcher::getInstance()->addDirectory("res/core/shaders", true);
        EventManager::getInstance()->addListener(GP_EVENT_LISTENER(this, LightsAndShadows::onShaderDirectoryEvent), FileWatcherEvent::ID());


        // Create a quad for framebuffer preview
        Mesh* meshQuad = Mesh::createQuad(0,0,256,256);
        _quadModel = Model::create(meshQuad);
        SAFE_RELEASE(meshQuad);
        _quadModel->setMaterial("res/core/shaders/debug.vert", "res/core/shaders/debug.frag", "SHOW_DEPTH");
        _quadModel->getMaterial()->getParameter("u_texture")->setValue(_frameBuffer->getRenderTarget(0));

        Matrix proj;
        Matrix::createOrthographicOffCenter(0, game->getViewport().width, game->getViewport().height, 0, -1, 1, &proj);
        _quadModel->getMaterial()->getParameter("u_projectionMatrix")->setValue(proj);



    }

    void update(float elapsedTime)
    {
        // show toolbox
        showToolbox(elapsedTime);

        // update camera
        _fpCamera.updateCamera(elapsedTime);
    }

    void render(float elapsedTime)
    {
        View::getView(0)->bind();
        _frameBuffer->bind();
        _scene->visit(this, &LightsAndShadows::drawScene, (void*)1);

        View::getView(1)->bind();
        _scene->visit(this, &LightsAndShadows::drawScene, (void*)0);
        _quadModel->draw();

        drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
    }

    bool drawScene(Node* node, void* cookie)
    {
        Drawable* drawable = node->getDrawable();
        if (drawable)
        {
            Model* model = dynamic_cast<Model*>(drawable);

            if((int*)cookie == (int*)1)
            {
                model->getMaterial(0)->setTechnique("shadowmap");
            }
            else
            {
                model->getMaterial(0)->setTechnique("forward");
            }


            model->getMaterial(0)->getParameter("u_worldMatrix")->setValue(model->getNode()->getWorldMatrix());
            model->getMaterial(0)->getParameter("u_lightSpaceMatrix")->setValue(_lightSpaceMatrix);

            drawable->draw();
        }
        return true;
    }

    void showToolbox(float elapsedTime)
    {
        static float dirLightDirection[3] = { -1.0f, -1.0f, -1.0f };
        static float orthoProj[4] = { -10.0f, 10.0f, -10.0f, 10.0f };

        ImGui::Begin("Toolbox");
        ImGui::SliderFloat4("shadow projection", orthoProj, -100.0f, 100.0f);
        ImGui::SliderFloat3("light direction", dirLightDirection, -1.0f, 1.0f);
        ImGui::End();

        if(orthoProj[0] < 0 && orthoProj[1] > 0 && orthoProj[2] != orthoProj[3])
            _shadowProjection.set(orthoProj);



        static float accu = 0;
        accu += elapsedTime;
        //dirLightDirection[2] = sin(accu * 0.01);


        _lightManager.getDirectionnalLight(0)->getNode()->setDirection(Vector3(dirLightDirection));

        if(_node)
            _node->setDirection(Vector3(dirLightDirection));


        // set light matrix for shadows
        Vector3 lightDir = _lightManager.getDirectionnalLight(0)->getNode()->getForwardVector();
        Matrix lightProjection, lightView;
        Matrix::createOrthographic(_shadowProjection.x, _shadowProjection.y, _shadowProjection.z, _shadowProjection.w, &lightProjection);
        Matrix::createLookAt(Vector3(0.0, 0.0, 0.0), lightDir, Vector3(0.0, 1.0, 0.0), &lightView);
        _lightSpaceMatrix = lightProjection * lightView;
    }

    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
    {
        _fpCamera.touchEvent(evt, x, y, contactIndex);

        switch (evt)
        {
        case Touch::TOUCH_PRESS:
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


    void onShaderDirectoryEvent(EventDataRef eventData)
    {
        auto watchFileEvent = std::dynamic_pointer_cast<FileWatcherEvent>(eventData);

        // if change on shader directory reload material for all scene nodes
        if(watchFileEvent)
            _scene->visit(this, &LightsAndShadows::reloadMaterials);
    }

    bool reloadMaterials(Node* node)
    {
        Model* model = dynamic_cast<Model*>(node->getDrawable());
        if (model)
        {
            Material* material = model->getMaterial(0);
            material->reload();
        }
        return true;
    }
};

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Graphics", "Lights and Shadows", LightsAndShadows, 9)
#endif

