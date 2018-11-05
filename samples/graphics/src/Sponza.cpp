#include "SamplesGame.h"
#include "renderer/BGFXRenderer.h"
#include "FirstPersonCamera.h"
#include <dear-imgui/imgui.h>
#include <events/EventManager.h>
#include <core/FileWatcher.h>


FrameBuffer* _frameBuffer;
Matrix _lightSpaceMatrix;

/**
 * A manager for lights in scene.
 */
class LightManager
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
         _scene->visit(this, &LightManager::initializeMaterials);
    }

    bool initializeMaterials(Node* node)
    {
        Model* model = dynamic_cast<Model*>(node->getDrawable());
        if (model)
        {
            for(unsigned int i=0; i<model->getMeshPartCount(); i++)
            {
                Material* material = model->getMaterial(i);
                if(material)
                    initLightForMaterial(material);
            }

        }
        return true;
    }

    void initLightForMaterial(Material* material)
    {
        //material->setParameterAutoBinding("u_worldViewProjectionMatrix", RenderState::WORLD_VIEW_PROJECTION_MATRIX);
        material->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", RenderState::INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX);
        material->setParameterAutoBinding("u_viewMatrix", RenderState::VIEW_MATRIX);
        material->setParameterAutoBinding("u_worldMatrix", RenderState::WORLD_MATRIX);
        material->setParameterAutoBinding("u_worldViewMatrix", RenderState::WORLD_VIEW_MATRIX);
        material->setParameterAutoBinding("u_cameraPosition", RenderState::CAMERA_WORLD_POSITION);

        material->getParameter("u_ambientColor")->bindValue(this, &LightManager::getAmbientColor);
        material->getParameter("u_shininess")->setValue(0.5f);

        for(unsigned int i=0; i<_dirLights.size(); ++i)
        {
            material->getParameter(formatName("u_directionalLightDirection", i).c_str())->bindValue(_dirLights[i]->getNode(), &Node::getForwardVectorView);
            ///material->getParameter(formatName("u_directionalLightDirection", i).c_str())->bindValue(this, &LightManager::getDirection);
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


        //material->getStateBlock()->setDepthTest(true);
        //material->getStateBlock()->setDepthFunction(RenderState::DEPTH_LESS);

        // bind frame buffer depth target to sampler
        Texture::Sampler* sampler = _frameBuffer->getRenderTarget(0);
        sampler->setWrapMode(Texture::BORDER, Texture::BORDER);
        material->getParameter("s_shadowMap")->setSampler(sampler);

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






/**
 * Sponza scene for rendering experiments.
 * Material reloading for all drawable nodes is performed when
 * detecting any changes in the shader directory (live shaders coding)
 */
class Sponza : public Example
{
    Scene* _scene;
    Font* _font;
    FirstPersonCamera _fpCamera;
    LightManager _lightManager;


    Model* _quadModel;


public:

    Sponza()
        : _font(nullptr)
        , _scene(nullptr)
    {
    }

    void finalize()
    {
        SAFE_RELEASE(_scene);
        SAFE_RELEASE(_font);
    }

    void initialize()
    {
        // Create the font for drawing the framerate.
        _font = Font::create("res/core/ui/arial.gpb");

        // Load sponza scene
        _scene = Scene::load("res/data/samples/examples/sponza/sponza.gpb");

        // Use a light manager to declare lights
        _lightManager.setScene(_scene);
        _lightManager.setAmbientColor(Vector3(0.1, 0.1, 0.1));
        _lightManager.addDirectionnalLight(Light::createDirectional(Vector3(0.5, 0.2, 0.2)));
        _lightManager.addPointLight(Light::createPoint(Vector3(0.0, 0.0, 1.0), 50));
        _lightManager.addSpotLight(Light::createSpot(Vector3(0.0, 0.0, 1.0), 50, MATH_DEG_TO_RAD(30.0f),  MATH_DEG_TO_RAD(45.0f)));


        // create a cube
        Bundle* bundle = Bundle::create("res/data/scenes/box.gpb");
        Mesh* mesh = bundle->loadMesh("box_Mesh");
        Model* model = Model::create(mesh);
        model->setMaterial("res/data/materials/color.material");
        SAFE_RELEASE(mesh);
        SAFE_RELEASE(bundle);

        // attach the cube to the point light 0
        _lightManager.getPointLight(0)->getNode()->setDrawable(model);
        _lightManager.getPointLight(0)->getNode()->setScale(3.0f);
        model->getMaterial()->getParameter("u_color")->bindValue(_lightManager.getPointLight(0), &Light::getColor);

        // set fps camera
        _fpCamera.initialize(1.0, 100000.0f);
        _fpCamera.setPosition(Vector3(0,10,0));
        _fpCamera.rotate(MATH_DEG_TO_RAD(90.0f), 0.0f);
        _scene->addNode(_fpCamera.getRootNode());
        _scene->setActiveCamera(_fpCamera.getCamera());
        _scene->getActiveCamera()->setAspectRatio(getAspectRatio());

        // Add watcher to shader directory and bind event to be notified on changes
        FileWatcher::getInstance()->addDirectory("res/core/shaders", true);
        EventManager::getInstance()->addListener(GP_EVENT_LISTENER(this, Sponza::onShaderDirectoryEvent), FileWatcherEvent::ID());



#define FRAMEBUFFER_WIDTH 1024
#define FRAMEBUFFER_HEIGHT 1024

        // create views
        View::create(0, Game::getInstance()->getViewport(), View::ClearFlags::COLOR_DEPTH, 0x111122ff, 1.0f, 0);
        View::create(1, Rectangle(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT), View::ClearFlags::COLOR_DEPTH, 0x303030ff, 1.0f, 0);


        // Create a framebuffer with a depth texture
        Texture::TextureInfo texInfo;
        texInfo.id = "targetDepth";
        texInfo.width = FRAMEBUFFER_WIDTH;
        texInfo.height = FRAMEBUFFER_HEIGHT;
        texInfo.type = Texture::TEXTURE_RT;
        texInfo.format = Texture::Format::D16;
        texInfo.flags = BGFX_TEXTURE_RT | BGFX_SAMPLER_COMPARE_LEQUAL;
        Texture* texDepth = Texture::create(texInfo);

        std::vector<Texture*> textures;
        textures.push_back(texDepth);
        _frameBuffer = FrameBuffer::create("ShadowFrameBuffer", textures);



        // Create a quad for framebuffer preview
        Mesh* meshQuad = Mesh::createQuad(0,0,256,256);
        _quadModel = Model::create(meshQuad);
        SAFE_RELEASE(meshQuad);
        _quadModel->setMaterial("res/core/shaders/debug.vert", "res/core/shaders/debug.frag", "SHOW_DEPTH");
        _quadModel->getMaterial()->getParameter("u_texture")->setValue(_frameBuffer->getRenderTarget(0));


        // Initialise materials for all models in scene
         //_scene->visit(this, &Sponza::initializeMaterials);
        _lightManager.initializeMaterials();
    }

    bool initializeMaterials(Node* node)
    {
        Model* model = dynamic_cast<Model*>(node->getDrawable());
        if (model)
        {
            Material* material = model->getMaterial(0);
            if(material)
            {
                _lightManager.initLightForMaterial(material);
            }
        }
        return true;
    }

    void update(float elapsedTime)
    {
        // update camera
        _fpCamera.updateCamera(elapsedTime);

        // Create some ImGui controls
        static float ambient[3] = { 0.2f, 0.2f, 0.2f };
        static float dirLightDirection[3] = { 0.0, -1.0f, -0.1f };
        static float dirLightColor[3] = { 0.75f, 0.75f, 0.75f };
        static float pointLightPos[3] = { 0.0f, 1.0f, 0.0f };
        static float pointLightColor[3] = { 0.75f, 0.75f, 0.75f };
        static float spotLightPos[3] = { 0.0f, 1.0f, 0.0f };
        static float spotLightDir[3] = { 0.0f, -1.0f, 0.0f };
        static float spotLightColor[3] = { 0.75f, 0.75f, 0.75f };

        ImGui::SetNextWindowSize(ImVec2(200,200), ImGuiCond_FirstUseEver);
        ImGui::Begin("Light control");
        ImGui::SliderFloat3("Ambient", ambient, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::SliderFloat3("DirDirection", dirLightDirection, -1.0f, 1.0f);
        ImGui::SliderFloat3("DirColor", dirLightColor, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::SliderFloat3("PointPos", pointLightPos, -100.0f, 100.0f);
        ImGui::SliderFloat3("PointColor", pointLightColor, 0.0f, 1.0f);
        ImGui::Separator();
        ImGui::SliderFloat3("SpotPos", spotLightPos, -100.0f, 100.0f);
        ImGui::SliderFloat3("SpotDir", spotLightDir, -1.0f, 1.0f);
        ImGui::SliderFloat3("SpotColor", spotLightColor, 0.0f, 1.0f);
        ImGui::End();

        // update lights with gui settings

        _lightManager.setAmbientColor(ambient);

        _lightManager.getDirectionnalLight(0)->getNode()->setDirection(dirLightDirection);
        _lightManager.getDirectionnalLight(0)->setColor(dirLightColor);

        _lightManager.getPointLight(0)->getNode()->setTranslation(pointLightPos);
        _lightManager.getPointLight(0)->setColor(pointLightColor);

        _lightManager.getSpotLight(0)->getNode()->setTranslation(spotLightPos);
        _lightManager.getSpotLight(0)->getNode()->setDirection(spotLightDir);
        _lightManager.getSpotLight(0)->setColor(spotLightColor);



        // set light matrix for shadows

        Vector3 lightPos = _lightManager.getDirectionnalLight(0)->getNode()->getForwardVector();
        lightPos.negate();

        Matrix lightProjection, lightView;
        float near_plane = -500.0f, far_plane = 500.0f;
        Matrix::createOrthographic(-100, 100, near_plane, far_plane, &lightProjection);
        Matrix::createLookAt(lightPos, Vector3(0.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0), &lightView);
        _lightSpaceMatrix = lightProjection * lightView;

    }

    void render(float elapsedTime)
    {
        View::getView(1)->bind();
        _frameBuffer->bind();
        _scene->visit(this, &Sponza::drawSceneForShadow);

        View::getView(0)->bind();
        _scene->visit(this, &Sponza::drawScene);
        _quadModel->draw();

        drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
    }

    bool drawSceneForShadow(Node* node)
    {
        Drawable* drawable = node->getDrawable();
        if (drawable)
        {
            Model* model = dynamic_cast<Model*>(drawable);
            model->getMaterial(0)->setTechnique("mytech2");
            model->getMaterial(0)->getParameter("u_worldMatrix")->setValue(model->getNode()->getWorldMatrix());
            model->getMaterial(0)->getParameter("u_lightSpaceMatrix")->setValue(_lightSpaceMatrix);
            drawable->draw();
        }
        return true;
    }

    bool drawScene(Node* node)
    {
        Drawable* drawable = node->getDrawable();
        if (drawable)
        {
            Model* model = dynamic_cast<Model*>(drawable);
            model->getMaterial(0)->setTechnique("mytech1");
            model->getMaterial(0)->getParameter("u_worldMatrix")->setValue(model->getNode()->getWorldMatrix());
            model->getMaterial(0)->getParameter("u_lightSpaceMatrix")->setValue(_lightSpaceMatrix);
            drawable->draw();
        }
        return true;
    }

    void onShaderDirectoryEvent(EventDataRef eventData)
    {
        auto watchFileEvent = std::dynamic_pointer_cast<FileWatcherEvent>(eventData);

        // if change on shader directory reload material for all scene nodes
        if(watchFileEvent)
            _scene->visit(this, &Sponza::reloadMaterials);
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
ADD_SAMPLE("Graphics", "Sponza", Sponza, 4)
#endif
