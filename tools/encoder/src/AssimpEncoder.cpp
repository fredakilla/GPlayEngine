#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "AssimpEncoder.h"
#include "Sampler.h"

using namespace gplay;


AssimpEncoder::AssimpEncoder()
{
}

AssimpEncoder::~AssimpEncoder()
{
}

void AssimpEncoder::write(const std::string& filepath, const EncoderArguments& arguments)
{
    processFile(filepath);

    std::string outputFilePath = arguments.getOutputFilePath();
    LOG(1, "Saving binary file: %s\n", outputFilePath.c_str());

    //_gamePlayFile.adjust();
    _gamePlayFile.saveBinary(outputFilePath);
}


bool AssimpEncoder::writeMaterial(const std::string& filepath)
{

}


void AssimpEncoder::processFile(const std::string& filepath)
{
    Assimp::Importer importer;
    unsigned int importFlags =  aiProcess_Triangulate
            //| aiProcess_FlipUVs
            | aiProcess_CalcTangentSpace
            | aiProcess_JoinIdenticalVertices
            | aiProcess_GenUVCoords
            ;


    const aiScene* scene = importer.ReadFile(filepath, importFlags);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG(1, "ERROR::ASSIMP:: %s", importer.GetErrorString());
        return;
    }

    _directory = filepath.substr(0, filepath.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}


void AssimpEncoder::processNode(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for(unsigned int i=0; i<node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh* gpMesh = processMesh(mesh, scene, node);
    }

    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for(unsigned int i=0; i<node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh* AssimpEncoder::processMesh(aiMesh *mesh, const aiScene *scene, aiNode *node)
{
    const char* name = node->mName.C_Str();
    std::string meshId(name);
    meshId.append("_Mesh");

    // Check if this mesh has already been loaded.
    Mesh* outMesh = getMesh(meshId);
    if(outMesh)
    {
        return outMesh;
    }

    outMesh = new Mesh();
    outMesh->setId(meshId);



    // Position
    outMesh->addVetexAttribute(POSITION, 3);

    // Normals
    if(mesh->HasNormals())
    {
        outMesh->addVetexAttribute(NORMAL, Vertex::NORMAL_COUNT);
    }

    // Tangents && Binormals
    if(mesh->HasTangentsAndBitangents())
    {
        outMesh->addVetexAttribute(TANGENT, Vertex::TANGENT_COUNT);
        outMesh->addVetexAttribute(BINORMAL, Vertex::BINORMAL_COUNT);
    }

    // Texture Coordinates
    for (unsigned int i=0; i<MAX_UV_SETS; ++i)
    {
        if (mesh->HasTextureCoords(i))
        {
            outMesh->addVetexAttribute(TEXCOORD0 + i, Vertex::TEXCOORD_COUNT);
        }
    }

    // Vertex Color
    if(mesh->HasVertexColors(0))
    {
        outMesh->addVetexAttribute(COLOR, Vertex::DIFFUSE_COUNT);
    }

    // Skinning BlendWeights BlendIndices
    /*if(mesh->HasBones())
    {
        outMesh->addVetexAttribute(BLENDWEIGHTS, Vertex::BLEND_WEIGHTS_COUNT);
        outMesh->addVetexAttribute(BLENDINDICES, Vertex::BLEND_INDICES_COUNT);
    }*/






    // loop each mesh's vertices
    for(unsigned int i=0; i<mesh->mNumVertices; i++)
    {
        Vertex vertex;
        Vector3 vector;

        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        // normals
        if(mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
            vertex.hasNormal = true;
        }

        // tangent && bitangent
        if(mesh->HasTangentsAndBitangents())
        {
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
            vertex.hasTangent = true;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.binormal = vector;
            vertex.hasBinormal = true;
        }

        // texture coordinates
        for (unsigned int uvIndex=0; uvIndex<MAX_UV_SETS; ++uvIndex)
        {
            if(mesh->HasTextureCoords(uvIndex))
            {
                Vector2 vec;
                vec.x = mesh->mTextureCoords[uvIndex][i].x;
                vec.y = mesh->mTextureCoords[uvIndex][i].y;
                vertex.texCoord[uvIndex] = vec;
                vertex.hasTexCoord[uvIndex] = true;
            }
        }

        // color
        if(mesh->HasVertexColors(0))
        {
            Vector4 color;
            color.x = mesh->mColors[0][i].r;
            color.y = mesh->mColors[0][i].g;
            color.z = mesh->mColors[0][i].b;
            color.w = mesh->mColors[0][i].a;
            vertex.diffuse = color;
            vertex.hasDiffuse = true;
        }


        outMesh->addVertex(vertex);
    }




    // wak through each of the mesh's faces and retrieve the corresponding vertex indices.

    MeshPart* part = new MeshPart;
    for(unsigned int i=0; i<mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j=0; j<face.mNumIndices; j++)
            //indices.push_back(face.mIndices[j]);
            part->addIndex(face.mIndices[j]);
    }
    outMesh->addMeshPart(part);


    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    // 1. diffuse maps
    // vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    // textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // // 2. specular maps
    // vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    // textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // // 3. normal maps
    // std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    // textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // // 4. height maps
    // std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    // textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    // return a mesh object created from the extracted mesh data
    //return MeshPartData(vertices, indices, textures);




    //_gamePlayFile.addMesh(outMesh);


    LOG(1, "Added mesh %s\n", outMesh->getId().c_str());
    _gamePlayFile.addMesh(outMesh);
    saveMesh(outMesh->getId(), outMesh);


    return outMesh;
}

Mesh* AssimpEncoder::getMesh(std::string meshId)
{
    // Check if this mesh was already loaded.
    std::map<std::string, Mesh*>::iterator it = _meshes.find(meshId);
    if (it != _meshes.end())
    {
        return it->second;
    }
    return NULL;
}


void AssimpEncoder::saveMesh(std::string meshId, Mesh* mesh)
{
    assert(mesh);
    if (!getMesh(meshId))
    {
        _meshes[meshId] = mesh;
    }
}
