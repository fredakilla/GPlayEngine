#include "../editor/InGameEditor.h"
#include "../core/Base.h"
#include "../graphics/Scene.h"
#include <bgfxcommon/imgui/bgfximgui.h>
#include <dear-imgui/widgets/gizmo.h>

using namespace gplay;

namespace gpeditor {

//-----------------------------------------------------------------------------------------------------------------------
//
// UUID generator
// https://gist.github.com/fernandomv3/46a6d7656f50ee8d39dc
//
//-----------------------------------------------------------------------------------------------------------------------


static const std::string CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
static std::string __generateUUID()
{
    std::string uuid = std::string(36,' ');
    int rnd = 0;

    uuid[8] = uuid[13] = uuid[18] = uuid[23] = '-';
    uuid[14] = '4';

    for(int i=0; i<36; i++) {
        if (i != 8 && i != 13 && i != 18 && i != 14 && i != 23) {
            if (rnd <= 0x02) {
                rnd = 0x2000000 + (std::rand() * 0x1000000) | 0;
            }
            rnd >>= 4;
            uuid[i] = CHARS[(i == 19) ? ((rnd & 0xf) & 0x3) | 0x8 : rnd & 0xf];
        }
    }
    return uuid;
}


//-----------------------------------------------------------------------------------------------------------------------
//
// node Inspector Window
//
//-----------------------------------------------------------------------------------------------------------------------

class NodeInspector
{
private:

    struct TransformValues
    {
        float translation[3];
        float rotation[3];
        float scale[3];

        TransformValues()
        {
            reset();
        }

        void reset()
        {
            translation[0] = translation[1] = translation[2] = 0.0f;
            rotation[0] = rotation[1] = rotation[2] = 0.0f;
            scale[0] = scale[1] = scale[2] = 1.0f;
        }
    };

    std::map<Node*, TransformValues> _transformMap;

public:
    NodeInspector()
    {

    }

    void gizmoTransform(Camera& camera, Node* node)
    {
        GP_ASSERT(node);

        Matrix matrix = node->getMatrix();

        ImGui::Begin("Node Inspector");
        ImGui::Separator();
        ImGui::Text("Gizmo");

        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

        if (ImGui::IsKeyPressed(60))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(69))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(82))
            mCurrentGizmoOperation = ImGuizmo::SCALE;

        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;

        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(matrix.m, matrixTranslation, matrixRotation, matrixScale);
        ImGui::DragFloat3("Translation##gizmo", matrixTranslation, 0.025f);
        ImGui::DragFloat3("Rotation##gizmo", matrixRotation, 1.0f);
        ImGui::DragFloat3("Scale##gizmo", matrixScale, 0.025f);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.m);

        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        static bool useSnap(false);
        if (ImGui::IsKeyPressed(83))
            useSnap = !useSnap;
        ImGui::Checkbox("", &useSnap);
        ImGui::SameLine();

        static Vector3 snap = { 1.f, 1.f, 1.f };

        switch (mCurrentGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            ImGui::InputFloat3("Snap", &snap.x);
            break;
        case ImGuizmo::ROTATE:
            ImGui::InputFloat("Angle Snap", &snap.y);
            break;
        case ImGuizmo::SCALE:
            ImGui::InputFloat("Scale Snap", &snap.z);
            break;
        }

        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(camera.getViewMatrix().m, camera.getProjectionMatrix().m, mCurrentGizmoOperation, mCurrentGizmoMode, matrix.m, NULL, useSnap ? &snap.x : NULL);
        ImGui::End();

        // update node matrix from gizmo matrix result
        node->set(matrix);
    }

    void draw(Node* node)
    {
        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(300, 0), ImGuiCond_FirstUseEver);

        ImGui::Begin("Node Inspector");
        ImGui::Separator();

        if(node)
        {
            // if this is the first time we see this node in map
            // get original node transformation and map with editor TransformValues.
            if(_transformMap.count(node) == 0)
            {
                const Matrix& matrix = node->getMatrix();

                Vector3 s,t;
                Quaternion r;
                matrix.decompose(&s,&r,&t);

               _transformMap[node] = TransformValues();

               TransformValues& transformValues = _transformMap[node];

               transformValues.translation[0] = t.x;
               transformValues.translation[1] = t.y;
               transformValues.translation[2] = t.z;

               transformValues.scale[0] = s.x;
               transformValues.scale[1] = s.y;
               transformValues.scale[2] = s.z;

               // method 1 - get euler rotation from quaternion
               Vector3 euler;
               r.toEulerAngles(&euler);
               transformValues.rotation[0] = euler.x;   // roll
               transformValues.rotation[1] = euler.y;   // pitch
               transformValues.rotation[2] = euler.z;   // yaw

               // method 2 - get euler rotation from matrix
               /*Vector3 euler = matrix.getEulerAngles();
               transformValues.rotation[0] = euler.x;
               transformValues.rotation[1] = euler.y;
               transformValues.rotation[2] = euler.z;*/


            }

            // name
            {
                static char str0[128] = "coucou";
                ImGui::InputText("Name", str0, IM_ARRAYSIZE(str0));
            }

            // enabled
            {
                static bool enabled = true;
                enabled = node->isEnabled();
                if(ImGui::Checkbox("Enabled", &enabled))
                    node->setEnabled(enabled);
            }

            ImGui::Separator();
            ImGui::Text("Transformation");

            // reset button
            {
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.6f, 0.6f));
                if(ImGui::SmallButton("Reset"))
                {
                    TransformValues& transformValues = _transformMap[node];
                    transformValues.reset();

                    node->setIdentity();
                }
                ImGui::PopStyleColor();
            }


            // transform
            {
                TransformValues& transformValues = _transformMap[node];

                bool isTranslationUpdated = false;
                bool isRotationUpdated = false;
                bool isScaleUpdated = false;

                if(ImGui::DragFloat3("Translation", transformValues.translation, 0.025f)) isTranslationUpdated = true;
                if(ImGui::DragFloat3("Rotation", transformValues.rotation, 1.0f)) isRotationUpdated = true;
                if(ImGui::DragFloat3("Scale", transformValues.scale, 0.025f)) isScaleUpdated = true;

                if(isScaleUpdated || isRotationUpdated || isTranslationUpdated)
                {
                    Vector3 s(transformValues.scale);
                    Vector3 t(transformValues.translation);
                    Quaternion r;

                    // create rotation from euler and quaternion
                    Quaternion::createFromEulerAngles(
                                Vector3(
                                transformValues.rotation[0],
                                transformValues.rotation[1],
                                transformValues.rotation[2]),
                            &r);


                    // create rotation from euler and matrix
                    // BUGGED
                    //Matrix m;
                    //Matrix::createFromEuler(
                    //        MATH_DEG_TO_RAD(transformValues.rotation[2]),
                    //        MATH_DEG_TO_RAD(transformValues.rotation[1]),
                    //        MATH_DEG_TO_RAD(transformValues.rotation[0]),
                    //        &m);
                    //Quaternion::createFromRotationMatrix(m, &r);

                    Transform transform(s,r,t);
                    node->set(transform);
                }
            }



            // tags
            {

            }

        }

        ImGui::End();
    }
};



//-----------------------------------------------------------------------------------------------------------------------
//
// Scene Hierarchy window
//
//-----------------------------------------------------------------------------------------------------------------------

class SceneHierarchy
{
    enum TreeNodeAction
    {
        TreeNodeAction_None,
        TreeNodeAction_Create,
        TreeNodeAction_Delete
    };

    Node* _treeViewSelectedNode;        // current select node in tree
    int _treeViewSelectionMask;         // used to display selection overlay in tree
    int _treeViewItemCount;             // internally used for selection
    TreeNodeAction _treeNodeAction;     // result of context menu on node

public:

    SceneHierarchy()
    {
        _treeViewSelectedNode = nullptr;
        _treeViewSelectionMask = 0;
        _treeViewItemCount = 0;
        _treeNodeAction = TreeNodeAction_None;
    }

    Node* getSelectedNode()
    {
        return _treeViewSelectedNode;
    }

    void draw(Scene* scene)
    {
        if(!scene)
        {
            print("inGameEditor::SceneHierarchy - scene is null.");
            return;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
        ImGui::SetNextWindowSize(ImVec2(300, 500), ImGuiCond_FirstUseEver);

        ImGui::Begin("Scene Hierarchy");

        // reset selection mask and item count
        _treeViewSelectionMask = 0;
        _treeViewItemCount = 0;
        _treeNodeAction = TreeNodeAction_None;
        bool isCreateNodeClicked = false;
        bool isDeleteNodeClicked  = false;

        // set default scene name if empty
        const char* sceneName = scene->getId();
        if(!sceneName || strlen(sceneName) == 0)
            sceneName = "unnamed_scene";

        // buttons
        ImGui::Text("Selected node :"); ImGui::SameLine();
        isCreateNodeClicked = ImGui::Button("Create Node"); ImGui::SameLine();
        isDeleteNodeClicked = ImGui::Button("Delete Node");

        // scene tree node header
        ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Always);
        bool isRootOpen = ImGui::TreeNodeEx(sceneName);
        bool isRootClicked = ImGui::IsItemClicked();

        // context menu on tree node header
        ImGui::PushID(sceneName);
        if (ImGui::BeginPopupContextItem("item context menu"))
        {
            print("Scene is clicked\n");

            if (ImGui::Selectable("Create node")) _treeNodeAction = TreeNodeAction_Create;
            ImGui::EndPopup();

            _treeViewClearSelection();

        }
        ImGui::PopID();

        // visit scene child nodes
        if(isRootOpen)
        {
            _visitScene(scene);
            ImGui::TreePop();
        }

        if(isRootClicked)
        {
            print("Scene is clicked\n");
            _treeViewClearSelection();
        }


        // tree node action result is set by node context menu or buttons at top of tree view
        if(isCreateNodeClicked)
            _treeNodeAction = TreeNodeAction_Create;
        if(isDeleteNodeClicked)
            _treeNodeAction = TreeNodeAction_Delete;

        // process tree node actions
        switch(_treeNodeAction)
        {
        case TreeNodeAction_Create:
            _createChildNode(_treeViewSelectedNode, scene);
            break;

        case TreeNodeAction_Delete:
            _removeNode(_treeViewSelectedNode, scene);
            break;

        case TreeNodeAction_None:
        default:
            break;
        }


        ImGui::End();
        ImGui::PopStyleVar();
    }


private:

    //! add a scene node to tree view and recurse his childs
    void _addNodeToTreeView(Node* node)
    {
        _treeViewItemCount++;

        ImGuiTreeNodeFlags flags = 0
                | ImGuiTreeNodeFlags_OpenOnArrow
                | ImGuiTreeNodeFlags_OpenOnDoubleClick
                | ImGuiTreeNodeFlags_DefaultOpen
                | ImGuiTreeNodeFlags_NoTreePushOnOpen
                | ((_treeViewSelectionMask & (1 << _treeViewItemCount)) ? ImGuiTreeNodeFlags_Selected : 0)
                ;

        if(node == _treeViewSelectedNode)
            flags |= ImGuiTreeNodeFlags_Selected;

        bool hasChilds = node->getChildCount() > 0;
        if(!hasChilds)
            flags |= ImGuiTreeNodeFlags_Leaf;

        // set a default name when node name is empty
        const char* nodeName = node->getId();
        if(!nodeName || strlen(nodeName) == 0)
            nodeName = "unnamed_node";


        ImGui::TreePush();
        bool open = ImGui::TreeNodeEx(nodeName, flags);
        //bool open = ImGui::TreeNodeEx((void*)(intptr_t)&node, flags, nodeName);

        if (ImGui::IsItemClicked())
        {
            print("Item %s is clicked [0x%x]\n", nodeName, node);
            _treeViewSelectedNode = node;

            // update selection
            _treeViewSelectionMask = (1 << _treeViewItemCount);
        }


        // make unique id for context popup based on node name
        std::string uniqueId = nodeName + std::to_string(_treeViewItemCount);

        // context menu
        ImGui::PushID(uniqueId.c_str());
        if (ImGui::BeginPopupContextItem("item context menu"))
        {
            if (ImGui::Selectable("Create child node")) _treeNodeAction = TreeNodeAction_Create;
            if (ImGui::Selectable("Delete node")) _treeNodeAction = TreeNodeAction_Delete;
            ImGui::EndPopup();

            print("Item %s is clicked [0x%x]\n", nodeName, node);

            // update selection
            _treeViewSelectedNode = node;
            _treeViewSelectionMask = (1 << _treeViewItemCount);

        }
        ImGui::PopID();

        // recurse childs
        if(hasChilds && open)
        {
            _visitNode(node);
        }
        ImGui::TreePop();
    }

    //! Recurse for all children.
    void _visitNode(Node* node)
    {
        for (Node* child = node->getFirstChild(); child != NULL; child = child->getNextSibling())
        {
            _addNodeToTreeView(child);
        }
    }

    //! Recurse for all sibling.
    void _visitScene(Scene* scene)
    {
        for (Node* node = scene->getFirstNode(); node != NULL; node = node->getNextSibling())
        {
            _addNodeToTreeView(node);
        }
    }

    //! create child node on parent or if empty on scene
    void _createChildNode(Node* parent, Scene* scene)
    {
        std::string s = "Node_";
        s.append(__generateUUID());
        Node* node = Node::create(s.c_str());

        if(parent)
        {
            // add the new node as child of the selected node
            parent->addChild(node);
        }
        else
        {
            // add the new node to scene root
            scene->addNode(node);
        }
    }

    //! remove node from scene
    void _removeNode(Node* node, Scene* scene)
    {
        if(node)
        {
            node->removeAllChildren();
            scene->removeNode(node);
        }

        _treeViewClearSelection();
    }

    //! clear tree view selection
    void _treeViewClearSelection()
    {
        _treeViewSelectionMask = 0;
        _treeViewSelectedNode = nullptr;
    }

};



//-----------------------------------------------------------------------------------------------------------------------
//
// Log Window (based on ImGui log window example)
//
//-----------------------------------------------------------------------------------------------------------------------

struct LogWindow
{
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets;        // Index to lines offset
    bool                ScrollToBottom;

    void clear() { Buf.clear(); LineOffsets.clear(); }

    void addLog(std::string text)
    {
        int old_size = Buf.size();
        Buf.appendf("%s\n", text.c_str());
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size);
        ScrollToBottom = true;
    }

    void draw(const char* title, bool* p_open = NULL)
    {
        ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }
        if (ImGui::Button("Clear")) clear();
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);
        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
        if (copy) ImGui::LogToClipboard();

        if (Filter.IsActive())
        {
            const char* buf_begin = Buf.begin();
            const char* line = buf_begin;
            for (int line_no = 0; line != NULL; line_no++)
            {
                const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
                if (Filter.PassFilter(line, line_end))
                    ImGui::TextUnformatted(line, line_end);
                line = line_end && line_end[1] ? line_end + 1 : NULL;
            }
        }
        else
        {
            ImGui::TextUnformatted(Buf.begin());
        }

        if (ScrollToBottom)
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;
        ImGui::EndChild();
        ImGui::End();
    }
};


//-----------------------------------------------------------------------------------------------------------------------
//
// LogStreamer - Redirect std stream to a log widget
//
//-----------------------------------------------------------------------------------------------------------------------

class LogStreamer : public std::basic_streambuf<char>
{
public:
    LogStreamer(std::ostream &stream, LogWindow* logWidget);
    ~LogStreamer();

protected:
    virtual int_type overflow(int_type v);
    virtual std::streamsize xsputn(const char *p, std::streamsize n);

private:
    std::ostream &_stream;
    std::streambuf * _old_buf;
    std::string _string;
    LogWindow * _logWidget;
};



LogStreamer::LogStreamer(std::ostream &stream, LogWindow* logWidget)
    : _stream(stream)
{
    _logWidget = logWidget;
    _old_buf = stream.rdbuf();
    stream.rdbuf(this);
}

LogStreamer::~LogStreamer()
{
    // output anything that is left
    if (!_string.empty())
        _logWidget->addLog(_string.c_str());
    _stream.rdbuf(_old_buf);
}

std::streambuf::int_type LogStreamer::overflow(int_type v)
{
    if (v == '\n')
    {
        _logWidget->addLog(_string.c_str());
        _string.erase(_string.begin(), _string.end());
    }
    else
        _string += v;

    return v;
}

std::streamsize LogStreamer::xsputn(const char *p, std::streamsize n)
{
    _string.append(p, p + n);

    std::string::size_type pos = 0;
    while (pos != std::string::npos && _string.length()>1)
    {
        pos = _string.find('\n');
        if (pos != std::string::npos)
        {
            std::string tmp(_string.begin(), _string.begin() + pos);
            _logWidget->addLog(tmp.c_str());
            _string.erase(_string.begin(), _string.begin() + pos + 1);
        }
    }

    return n;
}


} // end namespace inGameEditor







//-----------------------------------------------------------------------------------------------------------------------
//
// InGameEditor impl
//
//-----------------------------------------------------------------------------------------------------------------------


static gpeditor::LogWindow _logWindow;
static gpeditor::LogStreamer* _logStream;

using namespace gpeditor;


InGameEditor::InGameEditor() :
    _scene(nullptr)
  , _sceneHierarchy(new gpeditor::SceneHierarchy)
  , _nodeInspector(new gpeditor::NodeInspector)
{
    _logStream = new LogStreamer(std::cout, &_logWindow);
}

InGameEditor::~InGameEditor()
{
    delete _sceneHierarchy;
    delete _nodeInspector;
}

void InGameEditor::initialize()
{

}

void InGameEditor::resize(int width, int height)
{

}

void InGameEditor::setScene(Scene* scene)
{
    _scene = scene;

 /*   // create a node hierarchy for testing

    Node* n1 = Node::create("n1");
    Node* n2 = Node::create("n2");
    Node* n3 = Node::create("n3");
    Node* n4 = Node::create("n4");
    Node* n5 = Node::create("n5");
    Node* n6 = Node::create("n6");

    n2->addChild(n3);
    n2->addChild(n4);
    n4->addChild(n5);

    _scene->addNode(n1);
    _scene->addNode(n2);
    _scene->addNode(n6);
    _scene->addNode();



    // load a box model

    Bundle* bundle = Bundle::create("res/data/scenes/box.gpb");
    Model* modelBox = Model::create(bundle->loadMesh("box_Mesh"));
    modelBox->setMaterial("res/data/materials/box.material");
    Node* nodeBox = Node::create("nodeBox");
    nodeBox->setDrawable(modelBox);
    SAFE_RELEASE(modelBox);
    _scene->addNode(nodeBox);

    nodeBox->setScale(2,1,3);
    nodeBox->setTranslation(1,2,-3);
    Quaternion q;
    Quaternion::createFromEulerAngles(Vector3(25,-42,126), &q);
    nodeBox->setRotation(q);

    SAFE_RELEASE(nodeBox);*/
}

void InGameEditor::update(float elapsedTime)
{
    if(_scene)
    {
        //ImGui::ShowDemoWindow();

        // show scene hierarchy window
        _sceneHierarchy->draw(_scene);

        Node* currentNode =  _sceneHierarchy->getSelectedNode();

        if(currentNode)
        {
            // gizmo transform
            Camera* camera = _scene->getActiveCamera();
            _nodeInspector->gizmoTransform(*camera, currentNode);
        }

        // show inspector window
        _nodeInspector->draw(currentNode);

        // show log window
        _logWindow.draw("Log");
    }
}

