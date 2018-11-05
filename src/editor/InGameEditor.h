#ifndef INGAMEEDITOR_H
#define INGAMEEDITOR_H

namespace gpeditor {
class SceneHierarchy;
class NodeInspector;
}

namespace gplay {

class Scene;

class InGameEditor
{
public:
    InGameEditor();
    ~InGameEditor();

    void initialize();
    void resize(int width, int height);
    void update(float elapsedTime);
    void setScene(Scene* scene);

private:
    Scene* _scene;
    gpeditor::SceneHierarchy* _sceneHierarchy;
    gpeditor::NodeInspector* _nodeInspector;
};

}

#endif // INGAMEEDITOR_H
