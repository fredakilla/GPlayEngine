#include "main.h"

// Declare our game instance
MinimalSample game;


MinimalSample::MinimalSample() :
    _scene(nullptr)
{

}

void MinimalSample::initialize()
{
    // Create the scene.
    _scene = Scene::create();

    // Create the camera.
    Camera* camera = Camera::createPerspective(45.0f, getAspectRatio(), 1.0f, 10.0f);
    Node* cameraNode = _scene->addNode("camera");

    // Attach the camera to a node. This determines the position of the camera.
    cameraNode->setCamera(camera);

    // Make this the active camera of the scene.
    _scene->setActiveCamera(camera);
    SAFE_RELEASE(camera);

    // Move the camera to look at the origin.
    cameraNode->translate(0, 1, 5);
    cameraNode->rotateX(MATH_DEG_TO_RAD(-11.25f));

    // Load a bundle that contains a box mesh.
    Bundle* bundle = Bundle::create("res/data/scenes/box.gpb");

    // Create model for box mesh and set a material.
    Model* modelBox = Model::create(bundle->loadMesh("box_Mesh"));
    modelBox->setMaterial("res/data/materials/box.material");

    // Create a node for the box model.
    Node* nodeBox = Node::create("nodeBox");
    nodeBox->setDrawable(modelBox);
    SAFE_RELEASE(modelBox);

    // Add node to scene.
    _scene->addNode(nodeBox);
    SAFE_RELEASE(nodeBox);
}

void MinimalSample::finalize()
{
    SAFE_RELEASE(_scene);
}

void MinimalSample::update(float elapsedTime)
{
    static float rotateValue = 0.0f;
    rotateValue += elapsedTime * 0.001f;

    // Rotate node.
    _scene->findNode("nodeBox")->setRotation(Vector3(0,1,0), rotateValue);
}

void MinimalSample::render(float elapsedTime)
{
    clear(ClearFlags::CLEAR_COLOR_DEPTH, Vector4::fromColor(0x4488aaff), 1.0f, 0);

    // Visit scene using MinimalSample::drawScene method for drawing each node.
    _scene->visit(this, &MinimalSample::drawScene);
}

bool MinimalSample::drawScene(Node* node)
{
    // if this node is a model, draw it.
    Model* model = dynamic_cast<Model*>(node->getDrawable());
    if (model)
        model->draw();
    return true;
}
