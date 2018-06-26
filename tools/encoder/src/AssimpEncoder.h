#ifndef ASSIMPCODER_H_
#define ASSIMPCODER_H_

#include "GPBFile.h"
#include "EncoderArguments.h"

using namespace gplay;

class aiNode;
class aiScene;
class aiMesh;

/**
 * Class for binary encoding 3d files supported by assimp library
 */
class AssimpEncoder
{
public:

    /**
     * Constructor.
     */
    AssimpEncoder();

    /**
     * Destructor.
     */
    ~AssimpEncoder();

    /**
     * Writes out encoded FBX file.
     */
    void write(const std::string& filepath, const EncoderArguments& arguments);

    /**
     * Writes a material file.
     *
     * @param filepath
     *
     * @return True if successful; false otherwise.
     */
    bool writeMaterial(const std::string& filepath);

private:

    /**
     * The GPlay file that is populated while reading the FBX file.
     */
    GPBFile _gamePlayFile;







    struct VertexData
    {
        Vector3 Position;
        Vector3 Normal;
        Vector2 TexCoords;
        Vector3 Tangent;
        Vector3 Bitangent;
    };

    struct TextureData
    {
        unsigned int id;
        std::string type;
        std::string path;
    };


    class MeshPartData
    {
    public:
        std::vector<VertexData> vertices;
        std::vector<unsigned int> indices;
        std::vector<TextureData> textures;

        MeshPartData(std::vector<VertexData> vertices, std::vector<unsigned int> indices, std::vector<TextureData> textures)
        {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;
        }
    };




    std::string _directory;
    //std::vector<MeshPartData> _meshes;
    //std::vector<Mesh*> _meshes;
    std::map<std::string, Mesh*> _meshes;



    void processFile(const std::string& filepath);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh* processMesh(aiMesh* mesh, const aiScene* scene, aiNode *node);

    Mesh* getMesh(std::string meshId);
    void saveMesh(std::string meshId, Mesh* mesh);
};

#endif // ASSIMPCODER_H_
