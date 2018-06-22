#include "gplay-engine.h"
#include "Example.h"
#include "SamplesGame.h"
#include "FirstPersonCamera.h"
#include <imgui/imgui.h>

using namespace gplay;

class NewRenderer : public Example
{
    FirstPersonCamera _fpCamera;
    Font* _font;
    Scene* _scene;

    FrameBuffer* _gBuffer;
    Material* _matDeferred;
    Material* _matGBuffer;
    Model* _quadModel[4];
    Model* _screenQuad;

    FrameBuffer* _lightBuffer;
    Model* _lightQuad;

    SpriteBatch* _spriteBatch;

public:

    NewRenderer()
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
        _fpCamera.initialize(0.1f, 10000.0f);
        _fpCamera.setPosition(cameraPosition);
        _scene->addNode(_fpCamera.getRootNode());
        _scene->setActiveCamera(_fpCamera.getCamera());
        _scene->getActiveCamera()->setAspectRatio(getAspectRatio());



        // create GBuffer

        _matGBuffer = Material::create("res/core/shaders/gbuffer/gbuffer.vert", "res/core/shaders/gbuffer/gbuffer.frag");
        _matGBuffer->getStateBlock()->setCullFace(true);
        _matGBuffer->getStateBlock()->setDepthTest(true);
        _matGBuffer->getStateBlock()->setDepthWrite(true);
        _matGBuffer->setParameterAutoBinding("u_worldViewProjectionMatrix", RenderState::WORLD_VIEW_PROJECTION_MATRIX);
        _matGBuffer->setParameterAutoBinding("u_worldMatrix", RenderState::WORLD_MATRIX);

        Texture::Sampler* sampler = _matGBuffer->getParameter("u_diffuseTexture")->setValue("res/data/textures/brick.png", true);
        sampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);

        Texture::Sampler* specSampler = _matGBuffer->getParameter("s_specular")->setValue("res/data/textures/spec.png", true);
        specSampler->setFilterMode(Texture::LINEAR_MIPMAP_LINEAR, Texture::LINEAR);



#if 0
        // Load box shape
        Bundle* bundle = Bundle::create("res/data/scenes/shapes.gpb");
        //Model* model = Model::create(bundle->loadMesh("Cube_Mesh"));
        //model->setMaterial(material);

        for(int i=0; i<25; i++)
            for(int j=0; j<25; j++)
        {
        Model* model = Model::create(bundle->loadMesh("Cube_Mesh"));
        model->setMaterial(_matGBuffer->clone());
        Node* node = _scene->addNode();
        node->setScale(1.0f);
        node->setDrawable(model);
        node->setTranslation(Vector3(i*3,0,-j*3));
        }

#else




        // load shapes bundle
        Bundle* bundle = Bundle::create("res/data/scenes/shapes.gpb");

        // create a plane
        Model* modelPlane = Model::create(bundle->loadMesh("Plane_Mesh"));
        modelPlane->setMaterial(_matGBuffer->clone());
        Node* nodePlane = Node::create("plane");
        nodePlane->setDrawable(modelPlane);
        nodePlane->setScale(10.0f);
        _scene->addNode(nodePlane);

        // create a teapot
        Model* modelTeapot = Model::create(bundle->loadMesh("Teapot_Mesh"));
        modelTeapot->setMaterial(_matGBuffer->clone());
        Node* nodeTeapot = Node::create("teapot");
        nodeTeapot->setDrawable(modelTeapot);
        nodeTeapot->setScale(0.5f);
        _scene->addNode(nodeTeapot);

        // create a torus
        Model* modelTorus = Model::create(bundle->loadMesh("Torus_Mesh"));
        modelTorus->setMaterial(_matGBuffer->clone());
        Node* nodeTorus = Node::create("torus");
        nodeTorus->setDrawable(modelTorus);
        nodeTorus->setScale(0.5f);
        nodeTorus->setTranslation(-2, 0.5, -2);
        _scene->addNode(nodeTorus);

        // create a torus knot
        Model* modelTorusKnot = Model::create(bundle->loadMesh("TorusKnot_Mesh"));
        modelTorusKnot->setMaterial(_matGBuffer->clone());
        Node* nodeTorusKnot = Node::create("torusKnot");
        nodeTorusKnot->setDrawable(modelTorusKnot);
        nodeTorusKnot->setScale(0.25f);
        nodeTorusKnot->setTranslation(2, 0.5, -2);
        _scene->addNode(nodeTorusKnot);

        // create a cone
       Model* modelCone = Model::create(bundle->loadMesh("Cone_Mesh"));
        modelCone->setMaterial(_matGBuffer->clone());
        Node* nodeCone = Node::create("cone");
        nodeCone->setDrawable(modelCone);
        nodeCone->setScale(0.5f);
        nodeCone->setTranslation(2, 0.5, 2);
        _scene->addNode(nodeCone);

        // create suzanne
        Model* modelMonkey = Model::create(bundle->loadMesh("Monkey_Mesh"));
        modelMonkey->setMaterial(_matGBuffer->clone());
        Node* nodeMonkey = Node::create("monkey");
        nodeMonkey->setDrawable(modelMonkey);
        nodeMonkey->setScale(0.5f);
        nodeMonkey->setTranslation(-2, 0.5, 2);
        _scene->addNode(nodeMonkey);

        SAFE_RELEASE(bundle);

#endif





    const int FRAMEBUFFER_WIDTH = getWidth();
    const int FRAMEBUFFER_HEIGHT = getHeight();

        // create views
        View::create(0, Game::getInstance()->getViewport(), View::ClearFlags::COLOR_DEPTH, 0x111122ff, 1.0f, 0);
        View::create(1, Rectangle(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT), View::ClearFlags::COLOR_DEPTH, 0x303030ff, 1.0f, 0);
        View::create(2, Rectangle(FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT), View::ClearFlags::COLOR_DEPTH, 0x303030ff, 1.0f, 0);



        std::vector<Texture*> textures;      

        {
        // - normal buffer
        Texture::TextureInfo texInfo;
        texInfo.id = "NormalBuffer";
        texInfo.width = FRAMEBUFFER_WIDTH;
        texInfo.height = FRAMEBUFFER_HEIGHT;
        texInfo.type = Texture::TEXTURE_RT;
        texInfo.format = Texture::Format::RGB;
        texInfo.flags = BGFX_TEXTURE_RT;
        Texture* tex = Texture::create(texInfo);
        textures.push_back(tex);
        }

        {
        // - color + specular buffer
        Texture::TextureInfo texInfo;
        texInfo.id = "AlbedoSpecBuffer";
        texInfo.width = FRAMEBUFFER_WIDTH;
        texInfo.height = FRAMEBUFFER_HEIGHT;
        texInfo.type = Texture::TEXTURE_RT;
        texInfo.format = Texture::Format::RGBA;
        texInfo.flags = BGFX_TEXTURE_RT;
        Texture* tex = Texture::create(texInfo);
        textures.push_back(tex);
        }


        {
        // - depth buffer
        Texture::TextureInfo texInfo;
        texInfo.id = "DepthBuffer";
        texInfo.width = FRAMEBUFFER_WIDTH;
        texInfo.height = FRAMEBUFFER_HEIGHT;
        texInfo.type = Texture::TEXTURE_RT;
        texInfo.format = Texture::Format::D24F;
        texInfo.flags = BGFX_TEXTURE_RT;
        Texture* tex = Texture::create(texInfo);
        textures.push_back(tex);
        }

        // create gbuffer mrt
        _gBuffer = FrameBuffer::create("GBuffer", textures);





        // Create quads for gbuffer preview
        for(int i=0; i<3; i++)
        {
            Mesh* meshQuad = Mesh::createQuad(-1 + i*0.5, -1, 0.5 ,0.5);
            _quadModel[i] = Model::create(meshQuad);
            _quadModel[i]->setMaterial("res/core/shaders/debug/texture.vert", "res/core/shaders/debug/texture.frag");
            _quadModel[i]->getMaterial()->getParameter("s_texture")->setValue(_gBuffer->getRenderTarget(i));
            SAFE_RELEASE(meshQuad);
        }




        // TODO: implement methods to create material with pass and technique at runtime.

        Material* lightingMaterial = Material::create("res/core/materials/lighting.material");

        lightingMaterial->getStateBlock()->setCullFace(true);
        lightingMaterial->getStateBlock()->setDepthTest(false);
        lightingMaterial->getStateBlock()->setDepthWrite(false);

        lightingMaterial->getParameter("u_viewPos")->bindValue(_fpCamera.getRootNode(), &Node::getTranslationWorld);
        lightingMaterial->getParameter("u_inverseProjectionMatrix")->bindValue(_fpCamera.getRootNode(), &Node::getInverseProjectionMatrix);
        lightingMaterial->getParameter("u_inverseViewMatrix")->bindValue(_fpCamera.getRootNode(), &Node::getInverseViewMatrix);

        lightingMaterial->getParameter("gNormal")->setValue(_gBuffer->getRenderTarget("NormalBuffer"));
        lightingMaterial->getParameter("gAlbedoSpec")->setValue(_gBuffer->getRenderTarget("AlbedoSpecBuffer"));
        lightingMaterial->getParameter("gDepth")->setValue(_gBuffer->getRenderTarget("DepthBuffer"));



        /*

        _matDeferred = Material::create("res/core/shaders/deferred.vert", "res/core/shaders/deferred.frag");
        _matDeferred->getStateBlock()->setCullFace(false);
        _matDeferred->getStateBlock()->setDepthTest(true);
        _matDeferred->getStateBlock()->setDepthWrite(true);

        _matDeferred->getStateBlock()->setCullFace(false);
        _matDeferred->getStateBlock()->setDepthTest(false); //false
        _matDeferred->getStateBlock()->setDepthWrite(false);
        _matDeferred->getStateBlock()->setBlend(true);
        _matDeferred->getStateBlock()->setBlendSrc(RenderState::BLEND_ONE);
        _matDeferred->getStateBlock()->setBlendDst(RenderState::BLEND_ONE);


            // NOTE : this does not works because ther is no node attached for node binding
            _matDeferred->setParameterAutoBinding("u_worldViewProjectionMatrix", RenderState::WORLD_VIEW_PROJECTION_MATRIX);
            _matDeferred->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", RenderState::INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX);

        _matDeferred->getParameter("viewPos")->bindValue(_fpCamera.getRootNode(), &Node::getTranslationWorld);
        _matDeferred->getParameter("u_inverseProjectionMatrix")->bindValue(_fpCamera.getRootNode(), &Node::getInverseProjectionMatrix);
        _matDeferred->getParameter("u_inverseViewMatrix")->bindValue(_fpCamera.getRootNode(), &Node::getInverseViewMatrix);



        Texture::Sampler* sampler1 = Texture::Sampler::create(_gBuffer->getRenderTarget(0));
        _matDeferred->getParameter("gPosition")->setValue(sampler1);
        Texture::Sampler* sampler2 = Texture::Sampler::create(_gBuffer->getRenderTarget(1));
        _matDeferred->getParameter("gNormal")->setValue(sampler2);
        Texture::Sampler* sampler3 = Texture::Sampler::create(_gBuffer->getRenderTarget(2));
        _matDeferred->getParameter("gAlbedoSpec")->setValue(sampler3);
        Texture::Sampler* sampler4 = Texture::Sampler::create(_gBuffer->getRenderTarget(3));
        _matDeferred->getParameter("s_depthBuffer")->setValue(sampler4);

*/








        //--------


        _lightBuffer = FrameBuffer::create("LightBuffer", FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, Texture::Format::RGB);

        Mesh* fullScreenQuad = Mesh::createQuadFullscreen();
        _lightQuad = Model::create(fullScreenQuad);
        _lightQuad->setMaterial(lightingMaterial);



        // combine

            Material* _matCombine;
            _matCombine = Material::create("res/core/shaders/gbuffer/viewport.vert", "res/core/shaders/gbuffer/viewport.frag");
            _matCombine->getStateBlock()->setCullFace(true);
            _matCombine->getStateBlock()->setDepthTest(false); //false
            _matCombine->getStateBlock()->setDepthWrite(false);
            /*_matCombine->getStateBlock()->setBlend(true);
            _matCombine->getStateBlock()->setBlendSrc(RenderState::BLEND_ONE);
            _matCombine->getStateBlock()->setBlendDst(RenderState::BLEND_ONE);*/
            _matCombine->setParameterAutoBinding("u_worldViewProjectionMatrix", RenderState::WORLD_VIEW_PROJECTION_MATRIX);
            _matCombine->setParameterAutoBinding("u_inverseTransposeWorldViewMatrix", RenderState::INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX);


            _matCombine->getParameter("s_albedo")->setValue(_gBuffer->getRenderTarget("AlbedoSpecBuffer"));
            _matCombine->getParameter("s_light")->setValue(_lightBuffer->getRenderTarget(0));

            //_matCombine->getParameter("u_projectionMatrix")->setValue(uporj);


        //--------------














        //Mesh* meshQuad = Mesh::createQuad(-1,-1,2,2);
        Mesh* meshQuad = Mesh::createQuadFullscreen();

        _screenQuad = Model::create(meshQuad);
        _screenQuad->setMaterial(_matCombine);
        //Texture::Sampler* sampler = Texture::Sampler::create(_gBuffer->getRenderTarget(i));
        //_quadModel[i]->getMaterial()->getParameter("u_texture")->setValue(sampler);
        SAFE_RELEASE(meshQuad);





        // create quad mesh lines
        {
            float vertices[] =
            {
                -1.0f, -1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f,
                 1.0f,  1.0f, 0.0f,
                 1.0f, -1.0f, 0.0f
            };

            VertexFormat::Element elements[] =
            {
                VertexFormat::Element(VertexFormat::POSITION, 3)
            };
            Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 1), 4, false);
            mesh->setPrimitiveType(Mesh::LINES);
            mesh->setVertexData(&vertices[0], 0, 4);

        }




         _spriteBatch = SpriteBatch::create("res/data/textures/logo.png");


    }



    void update(float elapsedTime)
    {
        // show toolbox
        showToolbox();

        // update camera
        _fpCamera.updateCamera(elapsedTime);
    }





    Rectangle getScissorRegion(BoundingBox bbox)
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





    void render(float elapsedTime)
    {
        static int dim = 5;
        static float offset = 1.0f;
        static float pointlightRadius = 5.0f;
        static float pointLightPosition[3] = { 0.0, 3.0f, 0.0f };
        static float pointLightColor[3] = { 1.0f, 1.0f, 1.0f };
        static bool showScissorRect = false;
        static bool useScissor = true;

        ImGui::Begin("Toolbox");
        ImGui::SliderInt("Dim", &dim, 1, 100);
        ImGui::SliderFloat("Offset", &offset, -20.0f, 20.0f);
        ImGui::SliderFloat4("position", pointLightPosition, -10.0f, 10.0f);
        ImGui::SliderFloat3("color", pointLightColor, 0.0f, 10.0f);
        ImGui::SliderFloat("radius", &pointlightRadius, -10.0f, 100.0f);
        ImGui::Checkbox("show scissor rect", &showScissorRect);
        ImGui::Checkbox("apply scissor", &useScissor);
        ImGui::End();



        // 1. Geometry pass ---------

        View::getView(0)->bind();
        _gBuffer->bind();
        _scene->visit(this, &NewRenderer::drawScene);


        // 2. Lighting pass ---------

        View::getView(1)->bind();
        _lightBuffer->bind();

        // if point lighting, activate point light shader technique
        _lightQuad->getMaterial()->setTechnique("PointLight");


        _spriteBatch->start();
        Rectangle src(0, 0, 256, 256);


        // for each point lights
        for(int i=0; i<dim; i++)
            for(int j=0; j<dim; j++)
        {
            pointLightPosition[0] = -8 + i*offset;
            pointLightPosition[1] = pointLightPosition[1];
            pointLightPosition[2] = -8 + j*offset;



            // compute bounding sphere of point light
            BoundingBox bounds;
            bounds.set(BoundingSphere(pointLightPosition, pointlightRadius));

            // perform frustum culling with light bounds
            if(bounds.intersects(_scene->getActiveCamera()->getFrustum()))
            {
                // get 2D rectangle region of light influence
                Rectangle region = getScissorRegion(bounds);

                // set scissor on light region
                if(useScissor)
                    bgfx::setScissor(region.x, region.y, region.width, region.height);


                // show scissor region for debug
                if(showScissorRect)
                    _spriteBatch->draw(region, src, Vector4::fromColor(0xffffffff));


                // pass to shader light parameters
                _lightQuad->getMaterial()->getParameter("u_lightPos")->setValue(Vector3(pointLightPosition));
                _lightQuad->getMaterial()->getParameter("u_lightColor")->setValue(Vector3(pointLightColor));
                _lightQuad->getMaterial()->getParameter("u_lightRadius")->setValue(pointlightRadius);


                // draw quad of light influence
                _lightQuad->draw();

            }
        }

        // if directionnal lighting, activate directionnal light shader technique
        // for each directionnal lights
        // ...



        // 3. Final pass, render to viewport, combine light buffer and diffuse

        View::getView(2)->bind();
        _screenQuad->draw();

        _spriteBatch->finish();

        for(int i=0; i<3; i++)
            _quadModel[i]->draw();

        drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, 1, getFrameRate());
    }

    bool drawScene(Node* node)
    {
        Drawable* drawable = node->getDrawable();
        if (drawable)
        {
            /*Model* model = dynamic_cast<Model*>(drawable);

            if((int*)cookie == (int*)1)
            {
                //->getMaterial(0)->setTechnique("shadowmap");
                model->setMaterial(_matGBuffer);
            }
            else
            {
                //model->getMaterial(0)->setTechnique("forward");
                model->setMaterial(_matDeferred);
            }*/

            /*Model* model = dynamic_cast<Model*>(drawable);
            model->getMaterial()->getParameter("u_worldMatrix")->setValue(node->getWorldMatrix());*/

            drawable->draw();
        }
        return true;
    }

    void showToolbox()
    {

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
    ADD_SAMPLE("Graphics", "Renderer", NewRenderer, 255);
#endif

