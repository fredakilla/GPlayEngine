#include "gplay-engine.h"
#include "Example.h"
#include "SamplesGame.h"
#include "FirstPersonCamera.h"
#include <dear-imgui/imgui.h>

using namespace gplay;

#define SHADOW_RES 2048

#define BLUR_RES 256


class DeferredRenderer
{
private:

    static const int PASS_GEOMETRY_ID   = 0;
    static const int PASS_SHADOW_ID     = 1;
    static const int PASS_LIGHT_ID      = 2;
    static const int PASS_POST_ID       = 3;
    static const int PASS_COMBINE_ID    = 4;
    static const int PASS_DEBUG_ID      = 5;


    FrameBuffer* _gBuffer;
    FrameBuffer* _lightBuffer;
    FrameBuffer* _postProcessBuffer;

    Model* _finalQuad;
    Model* _lightQuad;

    Scene* _scene;

    std::vector<Light*> _dirLights;
    std::vector<Light*> _pointLights;
    std::vector<Light*> _spotLights;

    Technique* _techniqueShadow;

    Model* _quadModel[4];
    SpriteBatch* _spriteBatch;

    FrameBuffer* _shadowBuffer;
    Matrix _lightSpaceMatrix;


public:


    void create(Scene* scene)
    {
        _scene = scene;
        _scene->addRef();


        Game* game = Game::getInstance();

        Rectangle viewRect = game->getViewport();

        // create views
        View::create(PASS_GEOMETRY_ID, viewRect, View::ClearFlags::COLOR_DEPTH, 0x000000ff, 1.0f, 0);
        View::create(PASS_LIGHT_ID, viewRect, View::ClearFlags::COLOR_DEPTH, 0x00000000, 1.0f, 0);
        View::create(PASS_COMBINE_ID, viewRect, View::ClearFlags::COLOR_DEPTH, 0x00000000, 1.0f, 0);
        View::create(PASS_SHADOW_ID, Rectangle(SHADOW_RES, SHADOW_RES), View::ClearFlags::COLOR_DEPTH, 0x00000000, 1.0f, 0);
        View::create(PASS_POST_ID, Rectangle(BLUR_RES,BLUR_RES), View::ClearFlags::COLOR_DEPTH, 0x00000000, 1.0f, 0);


        // create the gbuffer

        std::vector<Texture*> textures;
        {
        // color + specular buffer
        Texture::TextureInfo texInfo;
        texInfo.id = "AlbedoSpecBuffer";
        texInfo.width = viewRect.width;
        texInfo.height = viewRect.height;
        texInfo.type = Texture::TEXTURE_RT;
        texInfo.format = Texture::Format::RGBA;
        texInfo.flags = BGFX_TEXTURE_RT;
        Texture* tex = Texture::create(texInfo);
        textures.push_back(tex);
        }

        {
        // normal buffer
        Texture::TextureInfo texInfo;
        texInfo.id = "NormalBuffer";
        texInfo.width = viewRect.width;
        texInfo.height = viewRect.height;
        texInfo.type = Texture::TEXTURE_RT;
        texInfo.format = Texture::Format::RGBA16F;
        texInfo.flags = BGFX_TEXTURE_RT;
        Texture* tex = Texture::create(texInfo);
        textures.push_back(tex);
        }

        {
        // depth buffer
        Texture::TextureInfo texInfo;
        texInfo.id = "DepthBuffer";
        texInfo.width = viewRect.width;
        texInfo.height = viewRect.height;
        texInfo.type = Texture::TEXTURE_RT;
        texInfo.format = Texture::Format::D24F;
        texInfo.flags = BGFX_TEXTURE_RT;
        Texture* tex = Texture::create(texInfo);
        textures.push_back(tex);
        }

        // create gbuffer mrt
        _gBuffer = FrameBuffer::create("GBuffer", textures);






        // load material from file
        //Material* lightingMaterial = Material::create("res/core/materials/lighting.material");
        // or create material from code
        Material* lightingMaterial = Material::create();
        {
        Effect* effectPointLight = Effect::createFromFile("res/core/shaders/deferred/pointLight.vert", "res/core/shaders/deferred/pointLight.frag");
        Technique* techPointLight = Technique::create("PointLight");
        techPointLight->addPass(Pass::create(effectPointLight, "pass0"));

        Effect* effectDirLight = Effect::createFromFile("res/core/shaders/deferred/pointLight.vert", "res/core/shaders/deferred/directionnalLight.frag");
        Technique* techDirectionnalLight = Technique::create("DirectionnalLight");
        techDirectionnalLight->addPass(Pass::create(effectDirLight, "pass0"));

        lightingMaterial->addTechnique(techPointLight);
        lightingMaterial->addTechnique(techDirectionnalLight);
        }

        lightingMaterial->getStateBlock()->setCullFace(true);
        lightingMaterial->getStateBlock()->setDepthTest(false);
        lightingMaterial->getStateBlock()->setDepthWrite(false);
        lightingMaterial->getStateBlock()->setBlend(true);
        lightingMaterial->getStateBlock()->setBlendSrc(RenderState::BLEND_ONE);
        lightingMaterial->getStateBlock()->setBlendDst(RenderState::BLEND_ONE);

        lightingMaterial->getParameter("u_viewPos")->bindValue(_scene->getActiveCamera()->getNode(), &Node::getTranslationWorld);
        lightingMaterial->getParameter("u_inverseProjectionMatrix")->bindValue(_scene->getActiveCamera()->getNode(), &Node::getInverseProjectionMatrix);
        lightingMaterial->getParameter("u_inverseViewMatrix")->bindValue(_scene->getActiveCamera()->getNode(), &Node::getInverseViewMatrix);

        lightingMaterial->getParameter("gAlbedoSpec")->setValue(_gBuffer->getRenderTarget("AlbedoSpecBuffer"));
        lightingMaterial->getParameter("gDepth")->setValue(_gBuffer->getRenderTarget("DepthBuffer"));
        lightingMaterial->getParameter("gNormal")->setValue(_gBuffer->getRenderTarget("NormalBuffer"));













        std::vector<Texture*> lightBufferTextures;

        {
        // light buffer
        Texture::TextureInfo texInfo;
        texInfo.id = "LightBuffer";
        texInfo.width = viewRect.width;
        texInfo.height = viewRect.height;
        texInfo.type = Texture::TEXTURE_RT;
        texInfo.format = Texture::Format::RGBA16F;
        texInfo.flags = BGFX_TEXTURE_RT;
        Texture* tex = Texture::create(texInfo);
        lightBufferTextures.push_back(tex);
        }

        {
        // bright buffer
        Texture::TextureInfo texInfo;
        texInfo.id = "BrightBuffer";
        texInfo.width = viewRect.width;
        texInfo.height = viewRect.height;
        texInfo.type = Texture::TEXTURE_RT;
        texInfo.format = Texture::Format::RGBA16F;
        texInfo.flags = BGFX_TEXTURE_RT;
        Texture* tex = Texture::create(texInfo);
        lightBufferTextures.push_back(tex);
        }

        // create gbuffer mrt
        _lightBuffer = FrameBuffer::create("LightBuffer", lightBufferTextures);
        //_lightBuffer = FrameBuffer::create("LightBuffer", viewRect.width, viewRect.height, Texture::Format::RGBA16F);

        Mesh* fullScreenQuad = Mesh::createQuadFullscreen();
        _lightQuad = Model::create(fullScreenQuad);
        _lightQuad->setMaterial(lightingMaterial);







        Material* _matCombine;
        _matCombine = Material::create("res/core/shaders/deferred/viewport.vert", "res/core/shaders/deferred/viewport.frag");
        _matCombine->getStateBlock()->setCullFace(true);
        _matCombine->getStateBlock()->setDepthTest(false);
        _matCombine->getStateBlock()->setDepthWrite(false);
        _matCombine->setParameterAutoBinding("u_worldViewProjectionMatrix", RenderState::WORLD_VIEW_PROJECTION_MATRIX);
        _matCombine->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", RenderState::INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX);

        _matCombine->getParameter("s_albedo")->setValue(_gBuffer->getRenderTarget("AlbedoSpecBuffer"));
        _matCombine->getParameter("s_light")->setValue(_lightBuffer->getRenderTarget(0));
        ///_matCombine->getParameter("s_bloom")->setValue(_lightBuffer->getRenderTarget(1));

        _matCombine->getTechnique()->setId("default");





        //Mesh* meshQuad = Mesh::createQuad(-1,-1,2,2);
        Mesh* meshQuad = Mesh::createQuadFullscreen();

        _finalQuad = Model::create(meshQuad);
        _finalQuad->setMaterial(_matCombine);
        //Texture::Sampler* sampler = Texture::Sampler::create(_gBuffer->getRenderTarget(i));
        //_quadModel[i]->getMaterial()->getParameter("u_texture")->setValue(sampler);
        SAFE_RELEASE(meshQuad);






        // Create quads for gbuffer preview
        for(int i=0; i<3; i++)
        {
            Mesh* meshQuad = Mesh::createQuad(-1 + i*0.5, -1, 0.5 ,0.5);
            _quadModel[i] = Model::create(meshQuad);
            _quadModel[i]->setMaterial("res/core/shaders/debug/texture.vert", "res/core/shaders/debug/texture.frag");
            _quadModel[i]->getMaterial()->getParameter("s_texture")->setValue(_gBuffer->getRenderTarget(i));
            SAFE_RELEASE(meshQuad);
        }

        _spriteBatch = SpriteBatch::create("res/data/textures/logo.png");






        // --------- SHADOW



        _techniqueShadow = Technique::create("shadow");
        _techniqueShadow->addPass(Pass::create(Effect::createFromFile("res/core/shaders/shadow.vert", "res/core/shaders/shadow.frag")));
        _techniqueShadow->getStateBlock()->setCullFace(true);
        _techniqueShadow->getStateBlock()->setCullFaceSide(RenderState::CULL_FACE_SIDE_FRONT);
        _techniqueShadow->getStateBlock()->setDepthTest(true);
        _techniqueShadow->getStateBlock()->setDepthWrite(true);


        _shadowBuffer = FrameBuffer::create("ShadowBuffer", SHADOW_RES, SHADOW_RES, Texture::Format::D16);

        Renderer::getInstance().setPaletteColor(0, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

        Texture::Sampler* shadowSampler = _shadowBuffer->getRenderTarget(0);
        shadowSampler->setWrapMode(Texture::BORDER, Texture::BORDER);
        shadowSampler->setFilterMode(Texture::NEAREST, Texture::NEAREST);
        shadowSampler->setCustomFlags(BGFX_SAMPLER_COMPARE_LESS | BGFX_SAMPLER_BORDER_COLOR(0));

        lightingMaterial->getParameter("s_shadowMap")->setSampler(shadowSampler);




        //--------------------















        // bloom post-process
        {
            std::vector<Texture*> textures;

            {
            Texture::TextureInfo texInfo;
            texInfo.id = "tex1";
            texInfo.width = BLUR_RES;//viewRect.width;
            texInfo.height = BLUR_RES;//viewRect.height;
            texInfo.type = Texture::TEXTURE_RT;
            texInfo.format = Texture::Format::RGBA16F;
            texInfo.flags = BGFX_TEXTURE_RT;
            Texture* tex = Texture::create(texInfo);
            textures.push_back(tex);
            }

           /* {
            Texture::TextureInfo texInfo;
            texInfo.id = "tex2";
            texInfo.width = 1024;
            texInfo.height = 1024;
            texInfo.type = Texture::TEXTURE_RT;
            texInfo.format = Texture::Format::RGBA;
            texInfo.flags = BGFX_TEXTURE_RT;
            Texture* tex = Texture::create(texInfo);
            textures.push_back(tex);
            }*/

            _postProcessBuffer = FrameBuffer::create("PostProcessBuffer", textures);



            Effect* effect = Effect::createFromFile("res/core/shaders/postprocess/blur.vert", "res/core/shaders/postprocess/blur.frag");
            Technique* technique = Technique::create("blur");
            technique->addPass(Pass::create(effect, "pass0"));
            _matCombine->addTechnique(technique);

            //Texture::Sampler* sampler = Texture::Sampler::create(_lightBuffer->getRenderTarget(1));
            //technique->getParameter("image")->setSampler(sampler);

            //Texture::Sampler* samplerBlur = Texture::Sampler::create(_postProcessBuffer->getRenderTarget(0));
            _matCombine->getTechnique("default")->getParameter("s_bloom")->setSampler(_postProcessBuffer->getRenderTarget(0));

        }



        // preview shadow
        {
        Mesh* meshQuad = Mesh::createQuad(-1 + 3*0.5, -1, 0.5 ,0.5);
        _quadModel[3] = Model::create(meshQuad);
        _quadModel[3]->setMaterial("res/core/shaders/debug/texture.vert", "res/core/shaders/debug/texture.frag");
        _quadModel[3]->getMaterial()->getParameter("s_texture")->setValue(_postProcessBuffer->getRenderTarget(0));
        }




        // analyse scene and add lights to the light manager
        _scene->visit(this, &DeferredRenderer::initializeScene);
    }


    bool initializeScene(Node* node)
    {
        // if node is a light, collect light
        Light* light = dynamic_cast<Light*>(node->getLight());
        if (light)
        {
            switch(light->getLightType())
            {
            case Light::POINT:
                _pointLights.push_back(light);
                break;
            case Light::DIRECTIONAL:
                _dirLights.push_back(light);
                break;
            case Light::SPOT:
                _spotLights.push_back(light);
                break;
            }

            return true;
        }


        // if node is model, add shadow technique
        Drawable* drawable = node->getDrawable();
        if (drawable)
        {
            Model* model = dynamic_cast<Model*>(drawable);
            if(model)
            {
                Material* material = model->getMaterial();
                if(material)
                {
                     material->addTechnique(_techniqueShadow->clone());
                }
            }          
        }

        return true;
    }






    bool drawNode(Node* node)
    {
        Drawable* drawable = node->getDrawable();
        if (drawable)
        {
            Model* model = dynamic_cast<Model*>(drawable);
            model->getMaterial()->setTechnique("");
            drawable->draw();
        }
        return true;
    }

    bool drawNodeShadow(Node* node)
    {
        Drawable* drawable = node->getDrawable();
        if (drawable)
        {
            Model* model = dynamic_cast<Model*>(drawable);
            model->getMaterial()->setTechnique("shadow");
            model->getMaterial()->getParameter("u_worldMatrix")->setValue(node->getWorldMatrix());
            model->getMaterial()->getParameter("u_lightSpaceMatrix")->setValue(_lightSpaceMatrix);

            drawable->draw();
        }
        return true;
    }


    void draw()
    {
        static int dim = 5;
        static float offset = 1.0f;
        static float pointlightRadius = 5.0f;
        static float pointLightPosition[4] = { 0.0, 0.0f, 0.0f, 5.0f };
        static float pointLightColor[3] = { 1.0f, 1.0f, 1.0f };
        static bool showDebugScissorRect = false;
        static bool useScissor = true;

        ImGui::Begin("Toolbox");
        ImGui::SliderInt("Dim", &dim, 1, 100);
        ImGui::SliderFloat("Offset", &offset, -20.0f, 20.0f);
        ImGui::SliderFloat4("position", pointLightPosition, -10.0f, 10.0f);
        ImGui::SliderFloat3("color", pointLightColor, 0.0f, 10.0f);
        ImGui::SliderFloat("radius", &pointlightRadius, 0.0f, 1000.0f);
        ImGui::Checkbox("show scissor rect", &showDebugScissorRect);
        ImGui::Checkbox("apply scissor", &useScissor);
        ImGui::End();



        Rectangle src(0, 0, 256, 256);
        if(showDebugScissorRect)
        {
            _spriteBatch->start();
        }




        // move light node with gui
        /*if(_dirLights.size() > 0)
            _dirLights[0]->getNode()->setDirection(Vector3(pointLightPosition));*/
        if(_pointLights.size() > 0) {
            _pointLights[0]->getNode()->setTranslation(Vector3(pointLightPosition));
            _pointLights[0]->setRange(pointLightPosition[3]);
        }





        // set light matrix for shadows
        Vector4 _shadowProjection = Vector4(-10.0f, 10.0f, -10.0f, 10.0f);
        Vector3 lightDir = _dirLights[0]->getNode()->getForwardVector();
        Matrix lightProjection, lightView;
        Matrix::createOrthographic(_shadowProjection.x, _shadowProjection.y, _shadowProjection.z, _shadowProjection.w, &lightProjection);
        Matrix::createLookAt(Vector3(0.0, 0.0, 0.0), lightDir, Vector3(0.0, 1.0, 0.0), &lightView);
        _lightSpaceMatrix = lightProjection * lightView;








        // Geometry pass
        View::getView(PASS_GEOMETRY_ID)->bind();
        _gBuffer->bind();
        _scene->visit(this, &DeferredRenderer::drawNode);



        // Shadow pass
        View::getView(PASS_SHADOW_ID)->bind();
        _shadowBuffer->bind();
        _scene->visit(this, &DeferredRenderer::drawNodeShadow);



        // Lighting pass
        View::getView(PASS_LIGHT_ID)->bind();
        _lightBuffer->bind();

        if(_pointLights.size() > 0)
        {
            // activate PointLight technique
            _lightQuad->getMaterial()->setTechnique("PointLight");

            for(int i=0; i<_pointLights.size(); ++i)
            {
                Light* pointLight = _pointLights[i];
                const BoundingSphere& boundingSphere = pointLight->getNode()->getBoundingSphere();

                // perform frustum culling with light bounds
                if(boundingSphere.intersects(_scene->getActiveCamera()->getFrustum()))
                {
                    BoundingBox bbox;
                    bbox.set(boundingSphere);

                    // get 2D rectangle area of light influence
                    Rectangle region = getScissorRegion(bbox);

                    // set scissor to only draw on this area
                    bgfx::setScissor(region.x, region.y, region.width, region.height);

                    // show scissor rectangle for debug
                    if(showDebugScissorRect)
                        _spriteBatch->draw(region, src, Vector4::fromColor(0xffffff55));


                    _lightQuad->getMaterial()->getParameter("u_lightPos")->setValue(pointLight->getNode()->getTranslation());
                    _lightQuad->getMaterial()->getParameter("u_lightColor")->setValue(pointLight->getColor());
                    _lightQuad->getMaterial()->getParameter("u_lightRadius")->setValue(pointLight->getRange());

                    _lightQuad->draw();
                }
            }
        }

        if(_dirLights.size() > 0)
        {
            // activate DirectionnalLight technique
            _lightQuad->getMaterial()->setTechnique("DirectionnalLight");

            for(int i=0; i<_dirLights.size(); ++i)
            {
                Light* dirLight = _dirLights[i];

                _lightQuad->getMaterial()->getParameter("u_lightDirection")->setValue(dirLight->getNode()->getForwardVectorWorld());
                _lightQuad->getMaterial()->getParameter("u_lightColor")->setValue(dirLight->getColor());

                _lightQuad->draw();
            }
        }

        /*if(_lightManager.getSpotLightCount() > 0)
        {
            //...
        }*/




        // post process
        View::getView(PASS_POST_ID)->bind();
        _postProcessBuffer->bind();
        _finalQuad->getMaterial()->setTechnique("blur");

        int x = 0;
        int y = 1;

        _finalQuad->getMaterial()->getParameter("u_direction")->setValue(Vector2(x, y));
        _finalQuad->getMaterial()->getParameter("image")->setSampler(_lightBuffer->getRenderTarget(1));

        for(int i=0; i<10; ++i) {

            _finalQuad->draw();

            _finalQuad->getMaterial()->getParameter("image")->setValue(_postProcessBuffer->getRenderTarget(0));
            _finalQuad->getMaterial()->getParameter("u_direction")->setValue(Vector2(x, y));

            x = 1 - x;
            y = 1 - y;
        }





        // Final pass, render to viewport, combine light buffer + diffuse and apply gamma and tonemapping        
        View::getView(PASS_COMBINE_ID)->bind();
        _finalQuad->getMaterial()->setTechnique("default");
        _finalQuad->draw();




        // show rect scissor for debug
        if(showDebugScissorRect)
            _spriteBatch->finish();

        // show gbuffer for debug
        for(int i=0; i<4; i++)
            _quadModel[i]->draw();

    }


    Rectangle getScissorRegion(const BoundingBox& bbox)
    {
        Camera* camera = _scene->getActiveCamera();
        Game* game = Game::getInstance();
        Rectangle vp = game->getViewport();

        Vector3 corners[8];
        Vector2 min(FLT_MAX, FLT_MAX);
        Vector2 max(-FLT_MAX, -FLT_MAX);
        bbox.getCorners(corners);
        for (unsigned int i = 0; i < 8; ++i)
        {
            const Vector3& corner = corners[i];
            float x, y;
            camera->project(vp, corner, &x, &y);
            if (x < min.x)
                min.x = x;
            if (y < min.y)
                min.y = y;
            if (x > max.x)
                max.x = x;
            if (y > max.y)
                max.y = y;
        }

        // define final rectangle
        float x = min.x;
        float y = min.y;
        float width = max.x - min.x;
        float height = max.y - min.y;

        // clamp origin to viewport
        if(x < 0.0f)
        {
            width += x;
            x = 0.0f;
        }
        if(y < 0.0f)
        {
            height += y;
            y = 0.0f;
        }

        if(width > vp.width)
            width = vp.width;
        if(height > vp.height)
            height = vp.height;


        return Rectangle(x,y,width,height);
    }





    DeferredRenderer()
    {

    }

    ~DeferredRenderer()
    {
        _scene->release();
    }






};







class DeferredTestSample : public Example
{
    FirstPersonCamera _fpCamera;
    Font* _font;
    Scene* _scene;  
    DeferredRenderer* _deferredRenderer;

public:

    DeferredTestSample()
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

        /*// Create a new empty scene.
        _scene = Scene::create();

        // set fps camera
        Vector3 cameraPosition(0, 1, 5);
        _fpCamera.initialize(0.1f, 10000.0f);
        _fpCamera.setPosition(cameraPosition);
        _scene->addNode(_fpCamera.getRootNode());
        _scene->setActiveCamera(_fpCamera.getCamera());
        _scene->getActiveCamera()->setAspectRatio(getAspectRatio());*/


#if 0
        // Load box shape
       /* Bundle* bundle = Bundle::create("res/data/scenes/shapes.gpb");
        //Model* model = Model::create(bundle->loadMesh("Cube_Mesh"));
        //model->setMaterial(material);

        for(int i=0; i<25; i++)
            for(int j=0; j<25; j++)
        {
        Model* model = Model::create(bundle->loadMesh("Cube_Mesh"));
        Node* node = _scene->addNode();
        node->setScale(1.0f);
        node->setDrawable(model);
        node->setTranslation(Vector3(i*3,0,-j*3));
        }*/


        _scene = Scene::load("res/data/samples/examples/sponza/sponza.gpb");

        // set fps camera
        Vector3 cameraPosition(0, 1, 5);
        _fpCamera.initialize(0.1f, 10000.0f);
        _fpCamera.setPosition(cameraPosition);
        _scene->addNode(_fpCamera.getRootNode());
        _scene->setActiveCamera(_fpCamera.getCamera());
        _scene->getActiveCamera()->setAspectRatio(getAspectRatio());

        Node* node = _scene->findNode("Point");
        _scene->removeNode(node);

#else


        // Create a new empty scene.
        _scene = Scene::create();

        // set fps camera
        Vector3 cameraPosition(0, 1, 5);
        _fpCamera.initialize(0.1f, 10000.0f);
        _fpCamera.setPosition(cameraPosition);
        _scene->addNode(_fpCamera.getRootNode());
        _scene->setActiveCamera(_fpCamera.getCamera());
        _scene->getActiveCamera()->setAspectRatio(getAspectRatio());






        Material* matDeferred = Material::create("res/core/shaders/deferred/gbuffer.vert", "res/core/shaders/deferred/gbuffer.frag", "NORMAL_MAP");
        matDeferred->getStateBlock()->setCullFace(true);
        matDeferred->getStateBlock()->setDepthTest(true);
        matDeferred->getStateBlock()->setDepthWrite(true);
        matDeferred->setParameterAutoBinding("u_worldViewProjectionMatrix", RenderState::WORLD_VIEW_PROJECTION_MATRIX);
        matDeferred->setParameterAutoBinding("u_worldMatrix", RenderState::WORLD_MATRIX);
        matDeferred->setParameterAutoBinding("u_inverseTransposeWorldMatrix", RenderState::INVERSE_TRANSPOSE_WORLD_MATRIX);

        Texture::Sampler* diffuseSampler = matDeferred->getParameter("u_diffuseTexture")->setValue("res/data/textures/brick.png", true);
        diffuseSampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);

        Texture::Sampler* specularSampler = matDeferred->getParameter("u_specularTexture")->setValue("res/data/textures/spec.png", true);
        specularSampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);

        Texture::Sampler* normalSampler = matDeferred->getParameter("u_normalTexture")->setValue("res/data/textures/brickn.png", true);
        normalSampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);







        // load shapes bundle
        Bundle* bundle = Bundle::create("res/data/scenes/shapes.gpb");

        // create a plane
        Model* modelPlane = Model::create(bundle->loadMesh("Plane_Mesh"));
        modelPlane->setMaterial(matDeferred->clone());
        Node* nodePlane = Node::create("plane");
        nodePlane->setDrawable(modelPlane);
        nodePlane->setScale(40.0f);        
        _scene->addNode(nodePlane);

        // create a teapot
        Model* modelTeapot = Model::create(bundle->loadMesh("Cube_Mesh"));
        {
        Material* material = matDeferred->clone();
        material->getParameter("s_diffuseTexture")->setValue("res/data/textures/crate.png", true);
        material->getParameter("s_normalTexture")->setValue("res/data/textures/crate_mn.png", true);
        material->getParameter("s_specularTexture")->setValue("res/data/textures/crate_s.png", true);
        modelTeapot->setMaterial(material);
        }
        Node* nodeTeapot = Node::create("teapot");
        nodeTeapot->setDrawable(modelTeapot);
        nodeTeapot->setScale(0.5f);
        nodeTeapot->setTranslation(0,0.5,0);
        _scene->addNode(nodeTeapot);

        // create a torus
        Model* modelTorus = Model::create(bundle->loadMesh("Torus_Mesh"));
        modelTorus->setMaterial(matDeferred->clone());
        Node* nodeTorus = Node::create("torus");
        nodeTorus->setDrawable(modelTorus);
        nodeTorus->setScale(0.5f);
        nodeTorus->setTranslation(-2, 0.5, -2);
        _scene->addNode(nodeTorus);

        // create a torus knot
        Model* modelTorusKnot = Model::create(bundle->loadMesh("TorusKnot_Mesh"));
        modelTorusKnot->setMaterial(matDeferred->clone());
        Node* nodeTorusKnot = Node::create("torusKnot");
        nodeTorusKnot->setDrawable(modelTorusKnot);
        nodeTorusKnot->setScale(0.25f);
        nodeTorusKnot->setTranslation(2, 0.5, -2);
        _scene->addNode(nodeTorusKnot);

        // create a cone
        Model* modelCone = Model::create(bundle->loadMesh("Cone_Mesh"));
        modelCone->setMaterial(matDeferred->clone());
        Node* nodeCone = Node::create("cone");
        nodeCone->setDrawable(modelCone);
        nodeCone->setScale(0.5f);
        nodeCone->setTranslation(2, 0.5, 2);
        _scene->addNode(nodeCone);

        // create suzanne
        Model* modelMonkey = Model::create(bundle->loadMesh("Monkey_Mesh"));
        modelMonkey->setMaterial(matDeferred->clone());
        Node* nodeMonkey = Node::create("monkey");
        nodeMonkey->setDrawable(modelMonkey);
        nodeMonkey->setScale(0.5f);
        nodeMonkey->setTranslation(-2, 0.5, 2);
        _scene->addNode(nodeMonkey);




        //SAFE_RELEASE(bundle);


#endif




        /*for(int i=0; i<50; i++)
            for(int j=0; j<50; j++)
        {
            float r = MATH_RANDOM_0_1() * 10;
            float g = MATH_RANDOM_0_1() * 10;
            float b = MATH_RANDOM_0_1() * 10;

            Light* pointLight = Light::createPoint(Vector3(r,g,b), 1.5f);
            Node* pointLightNode = Node::create("pointLight");
            pointLightNode->setLight(pointLight);
            pointLightNode->setTranslation(Vector3(-40+i*2, 1, -40+j*2));
            _scene->addNode(pointLightNode);
            SAFE_RELEASE(pointLight);
        }*/


        Light* pointLight = Light::createPoint(Vector3(1,1,1), 5.0f);
        Node* pointLightNode = Node::create("pointLight");
        pointLightNode->setLight(pointLight);
        pointLightNode->setTranslation(Vector3(2, 3, 3));
        _scene->addNode(pointLightNode);
        SAFE_RELEASE(pointLight);




        // light cube
        {
        Model* modelCubeLight = Model::create(bundle->loadMesh("Cube_Mesh"));
        {
            Material* material = Material::create("res/core/shaders/color.vert", "res/core/shaders/color.frag");
            material->getParameter("u_color")->setValue(Vector3(10,10,0));
            material->setParameterAutoBinding("u_worldViewProjectionMatrix", RenderState::WORLD_VIEW_PROJECTION_MATRIX);
            material->getStateBlock()->setCullFace(true);
            material->getStateBlock()->setDepthTest(true);
            material->getStateBlock()->setDepthWrite(true);
            modelCubeLight->setMaterial(material);
        }

        Node* pointLightNodeCube = Node::create("LightCubeNode");
        pointLightNodeCube->setScale(0.25);
        pointLightNodeCube->setDrawable(modelCubeLight);
        pointLightNode->addChild(pointLightNodeCube);
        }





       {
        Light* dirLight = Light::createDirectional(Vector3(1.0, 1.0, 1.0));
        Node* dirLightNode = Node::create("dirLight");
        dirLightNode->setLight(dirLight);
        dirLightNode->setDirection(Vector3(-1,-1,-1));
        _scene->addNode(dirLightNode);
        SAFE_RELEASE(dirLight);
        }







        _deferredRenderer = new DeferredRenderer();
        _deferredRenderer->create(_scene);
    }



    void update(float elapsedTime)
    {
        // update camera
        _fpCamera.updateCamera(elapsedTime);
    }



    void render(float elapsedTime)
    {
        _deferredRenderer->draw();

        drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
    }



    void touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
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

    void keyEvent(Keyboard::KeyEvent evt, int key)
    {
        _fpCamera.keyEvent(evt, key);
    }

};

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Graphics", "Deferred rendering", DeferredTestSample, 255);
#endif

