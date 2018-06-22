#include "Cube.h"
#include "SamplesGame.h"

#if defined(ADD_SAMPLE)
    ADD_SAMPLE("Graphics", "Cube", CubeExample, 0);
#endif


struct PosColorVertex
{
    Vector3 m_pos;
    Vector4 m_color;
};

static Mesh* createCube(bool dynamic)
{
    PosColorVertex s_vertices[] =
    {
        {  Vector3(-1.0f, -1.0f, -1.0f),  Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
        {  Vector3( 1.0f, -1.0f, -1.0f),  Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
        {  Vector3( 1.0f,  1.0f, -1.0f),  Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
        {  Vector3( 1.0f,  1.0f, -1.0f),  Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
        {  Vector3(-1.0f,  1.0f, -1.0f),  Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
        {  Vector3(-1.0f, -1.0f, -1.0f),  Vector4(1.0f, 0.0f, 0.0f, 1.0f) },

        {  Vector3(-1.0f, -1.0f,  1.0f),  Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
        {  Vector3( 1.0f, -1.0f,  1.0f),  Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
        {  Vector3( 1.0f,  1.0f,  1.0f),  Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
        {  Vector3( 1.0f,  1.0f,  1.0f),  Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
        {  Vector3(-1.0f,  1.0f,  1.0f),  Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
        {  Vector3(-1.0f, -1.0f,  1.0f),  Vector4(0.0f, 1.0f, 0.0f, 1.0f) },

        {  Vector3(-1.0f,  1.0f,  1.0f),  Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
        {  Vector3(-1.0f,  1.0f, -1.0f),  Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
        {  Vector3(-1.0f, -1.0f, -1.0f),  Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
        {  Vector3(-1.0f, -1.0f, -1.0f),  Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
        {  Vector3(-1.0f, -1.0f,  1.0f),  Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
        {  Vector3(-1.0f,  1.0f,  1.0f),  Vector4(0.0f, 0.0f, 1.0f, 1.0f) },

        {  Vector3( 1.0f,  1.0f,  1.0f),  Vector4(1.0f, 1.0f, 0.0f, 1.0f) },
        {  Vector3( 1.0f,  1.0f, -1.0f),  Vector4(1.0f, 1.0f, 0.0f, 1.0f) },
        {  Vector3( 1.0f, -1.0f, -1.0f),  Vector4(1.0f, 1.0f, 0.0f, 1.0f) },
        {  Vector3( 1.0f, -1.0f, -1.0f),  Vector4(1.0f, 1.0f, 0.0f, 1.0f) },
        {  Vector3( 1.0f, -1.0f,  1.0f),  Vector4(1.0f, 1.0f, 0.0f, 1.0f) },
        {  Vector3( 1.0f,  1.0f,  1.0f),  Vector4(1.0f, 1.0f, 0.0f, 1.0f) },

        {  Vector3(-1.0f, -1.0f, -1.0f),  Vector4(0.0f, 1.0f, 1.0f, 1.0f) },
        {  Vector3( 1.0f, -1.0f, -1.0f),  Vector4(0.0f, 1.0f, 1.0f, 1.0f) },
        {  Vector3( 1.0f, -1.0f,  1.0f),  Vector4(0.0f, 1.0f, 1.0f, 1.0f) },
        {  Vector3( 1.0f, -1.0f,  1.0f),  Vector4(0.0f, 1.0f, 1.0f, 1.0f) },
        {  Vector3(-1.0f, -1.0f,  1.0f),  Vector4(0.0f, 1.0f, 1.0f, 1.0f) },
        {  Vector3(-1.0f, -1.0f, -1.0f),  Vector4(0.0f, 1.0f, 1.0f, 1.0f) },

        {  Vector3(-1.0f,  1.0f, -1.0f),  Vector4(1.0f, 0.0f, 1.0f, 1.0f) },
        {  Vector3( 1.0f,  1.0f, -1.0f),  Vector4(1.0f, 0.0f, 1.0f, 1.0f) },
        {  Vector3( 1.0f,  1.0f,  1.0f),  Vector4(1.0f, 0.0f, 1.0f, 1.0f) },
        {  Vector3( 1.0f,  1.0f,  1.0f),  Vector4(1.0f, 0.0f, 1.0f, 1.0f) },
        {  Vector3(-1.0f,  1.0f,  1.0f),  Vector4(1.0f, 0.0f, 1.0f, 1.0f) },
        {  Vector3(-1.0f,  1.0f, -1.0f),  Vector4(1.0f, 0.0f, 1.0f, 1.0f) }
    };

    unsigned int vertexCount = 36;

    // Vertex format
    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::COLOR, 4)
    };

    // Create mesh.
    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 2), vertexCount, dynamic);
    if (mesh == NULL)
    {
        GP_ERROR("Failed to create mesh.");
        return NULL;
    }

    // Set vertices.
    mesh->setPrimitiveType(Mesh::TRIANGLES);
    mesh->setVertexData(s_vertices, 0, vertexCount);

    return mesh;
}


static Mesh* createIndexedCube(bool dynamic)
{
    static PosColorVertex s_vertices[] =
    {
        { Vector3(-1.0f,  1.0f,  1.0f),   Vector4(1.0, 0.0, 0.0, 1.0 ) },
        { Vector3( 1.0f,  1.0f,  1.0f),   Vector4(0.0, 1.0, 0.0, 1.0 ) },
        { Vector3(-1.0f, -1.0f,  1.0f),   Vector4(0.0, 0.0, 1.0, 1.0 ) },
        { Vector3( 1.0f, -1.0f,  1.0f),   Vector4(1.0, 1.0, 0.0, 1.0 ) },
        { Vector3(-1.0f,  1.0f, -1.0f),   Vector4(1.0, 0.0, 1.0, 1.0 ) },
        { Vector3( 1.0f,  1.0f, -1.0f),   Vector4(0.0, 1.0, 1.0, 1.0 ) },
        { Vector3(-1.0f, -1.0f, -1.0f),   Vector4(1.0, 1.0, 0.0, 1.0 ) },
        { Vector3( 1.0f, -1.0f, -1.0f),   Vector4(0.0, 1.0, 0.0, 1.0 ) },
    };

    static const unsigned short s_indices[] =
    {
        0, 1, 2, // 0
        1, 3, 2,
        4, 6, 5, // 2
        5, 6, 7,
        0, 2, 4, // 4
        4, 2, 6,
        1, 5, 3, // 6
        5, 7, 3,
        0, 4, 1, // 8
        4, 5, 1,
        2, 3, 6, // 10
        6, 3, 7,
    };

    unsigned int vertexCount = 8;
    unsigned int indexCount = 36;

    // Vertex format
    VertexFormat::Element elements[] =
    {
        VertexFormat::Element(VertexFormat::POSITION, 3),
        VertexFormat::Element(VertexFormat::COLOR, 4)
    };

    // Create mesh.
    Mesh* mesh = Mesh::createMesh(VertexFormat(elements, 2), vertexCount, dynamic);
    if (mesh == NULL)
    {
        GP_ERROR("Failed to create mesh.");
        return NULL;
    }

    // Set vertices.
    mesh->setPrimitiveType(Mesh::TRIANGLES);
    mesh->setVertexData(s_vertices, 0, vertexCount);

    // Set indices.
    MeshPart * part = mesh->addPart(Mesh::TRIANGLES, Mesh::INDEX16, indexCount, dynamic);
    part->setIndexData(s_indices, 0, indexCount);

    return mesh;
}


CubeExample::CubeExample()
    : _font(NULL), _model(NULL), _spinDirection(-1.0f)
{    
    _material = nullptr;
    _form = nullptr;
}


void CubeExample::setGeometry(bool dynamic)
{
    GP_ASSERT(_material);

    SAFE_RELEASE(_model);

    Mesh* mesh = createCube(dynamic);
    _model = Model::create(mesh);
    _model->setMaterial(_material);

    SAFE_RELEASE(mesh);
}

void CubeExample::setIndexedGeometry(bool dynamic)
{
    GP_ASSERT(_material);

    SAFE_RELEASE(_model);

    Mesh* mesh = createIndexedCube(dynamic);
    _model = Model::create(mesh);
    _model->setMaterial(_material);

    SAFE_RELEASE(mesh);
}

void CubeExample::initialize()
{
    // Create the font for drawing the framerate.
    _font = Font::create("res/core/ui/arial.gpb");

    // Create a perspective projection matrix.
    float ratio = getWidth() / (float)getHeight();
    Matrix::createPerspective(45.0f, ratio, 1.0f, 1000.0f, &_worldViewProjectionMatrix);
    _worldViewProjectionMatrix.translate(Vector3(0,0,-15.0f));

    // Create a material from the built-in "colored-unlit" vertex and fragment shaders.
    _material = Material::create("res/core/shaders/forward/colored.vert", "res/core/shaders/forward/colored.frag", "VERTEX_COLOR");
    _material->getStateBlock()->setCullFace(false);
    _material->getStateBlock()->setDepthTest(true);
    _material->getStateBlock()->setDepthWrite(true);

    // Set a static mesh non indexed at init.
    setGeometry(false);

    // Create UI

    Theme::Style* style = Theme::getDefault()->getEmptyStyle();

    _form = Form::create("Form", style);
    _form->setLayout(Layout::LAYOUT_VERTICAL);
    _form->setWidth(200);
    _form->setAlignment(Control::Alignment::ALIGN_VCENTER_LEFT);

    RadioButton * radio1 = RadioButton::create("radio_static");
    radio1->setText("Static");
    radio1->setSelected(true);
    radio1->addListener(this, Control::Listener::CLICK);

    RadioButton * radio2 = RadioButton::create("radio_static_indexed");
    radio2->setText("Static Indexed");
    radio2->setSelected(false);
    radio2->addListener(this, Control::Listener::CLICK);

    RadioButton * radio3 = RadioButton::create("radio_dynamic");
    radio3->setText("Dynamic");
    radio3->setSelected(false);
    radio3->addListener(this, Control::Listener::CLICK);

    RadioButton * radio4 = RadioButton::create("radio_dynamic_indexed");
    radio4->setText("Dynamic Indexed");
    radio4->setSelected(false);
    radio4->addListener(this, Control::Listener::CLICK);

    _form->addControl(radio1);
    _form->addControl(radio2);
    _form->addControl(radio3);
    _form->addControl(radio4);
}

void CubeExample::finalize()
{
    SAFE_RELEASE(_model);
    SAFE_RELEASE(_material);
    SAFE_RELEASE(_font);
    SAFE_RELEASE(_form);
}

void CubeExample::update(float elapsedTime)
{
    // Update the rotation of the triangle. The speed is 180 degrees per second.
    _worldViewProjectionMatrix.rotate(Vector3(1,2,3), _spinDirection * MATH_PI * elapsedTime * 0.001f);
}

void CubeExample::render(float elapsedTime)
{
    if(_model)
    {
        _model->getMaterial()->getParameter("u_worldViewProjectionMatrix")->setValue(_worldViewProjectionMatrix);
        _model->draw();
    }

    drawFrameRate(_font, Vector4(0, 0.5f, 1, 1), 5, getHeight()-20, getFrameRate());

    _form->draw();
}
void CubeExample::touchEvent(Touch::TouchEvent evt, int x, int y, unsigned int contactIndex)
{
    switch (evt)
    {
    case Touch::TOUCH_PRESS:
        if (x < 75 && y < 50)
        {
            // Toggle Vsync if the user touches the top left corner
            setVsync(!isVsync());
        }
        else
        {
            // Reverse the spin direction if the user touches the screen.
            _spinDirection *= -1.0f;
        }
        break;
    case Touch::TOUCH_RELEASE:
        break;
    case Touch::TOUCH_MOVE:
        break;
    };
}

void CubeExample::controlEvent(Control* control, EventType evt)
{
    Button* button = static_cast<Button*>(control);

    if (strcmp(button->getId(), "radio_static") == 0)
    {
        setGeometry(false);
    }
    else if (strcmp(button->getId(), "radio_static_indexed") == 0)
    {
        setIndexedGeometry(false);
    }
    else if (strcmp(button->getId(), "radio_dynamic") == 0)
    {
        setGeometry(true);
    }
    else if (strcmp(button->getId(), "radio_dynamic_indexed") == 0)
    {
        setIndexedGeometry(true);
    }
}
