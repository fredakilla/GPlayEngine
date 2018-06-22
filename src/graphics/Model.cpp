#include "../core/Base.h"
#include "../graphics/Model.h"
#include "../graphics/MeshPart.h"
#include "../graphics/Scene.h"
#include "../graphics/Technique.h"
#include "../graphics/Pass.h"
#include "../graphics/Node.h"

namespace gplay {

Model::Model() : Drawable(),
    _mesh(NULL), _material(NULL), _partCount(0), _partMaterials(NULL), _skin(NULL)
{
}

Model::Model(Mesh* mesh) : Drawable(),
    _mesh(mesh), _material(NULL), _partCount(0), _partMaterials(NULL), _skin(NULL)
{
    GP_ASSERT(mesh);
    _partCount = mesh->getPartCount();
}

Model::~Model()
{
    SAFE_RELEASE(_material);
    if (_partMaterials)
    {
        for (unsigned int i = 0; i < _partCount; ++i)
        {
            SAFE_RELEASE(_partMaterials[i]);
        }
        SAFE_DELETE_ARRAY(_partMaterials);
    }
    SAFE_RELEASE(_mesh);
    SAFE_DELETE(_skin);
}

Model* Model::create(Mesh* mesh)
{
    GP_ASSERT(mesh);
    mesh->addRef();
    return new Model(mesh);
}

Mesh* Model::getMesh() const
{
    return _mesh;
}

unsigned int Model::getMeshPartCount() const
{
    GP_ASSERT(_mesh);
    return _mesh->getPartCount();
}

Material* Model::getMaterial(int partIndex)
{
    GP_ASSERT(partIndex == -1 || partIndex >= 0);

    Material* m = NULL;

    if (partIndex < 0)
        return _material;
    if (partIndex >= (int)_partCount)
        return NULL;

    // Look up explicitly specified part material.
    if (_partMaterials)
    {
        m = _partMaterials[partIndex];
    }
    if (m == NULL)
    {
        // Return the shared material.
         m = _material;
    }

    return m;
}

void Model::setMaterial(Material* material, int partIndex)
{
    GP_ASSERT(partIndex == -1 || (partIndex >= 0 && partIndex < (int)getMeshPartCount()));

    Material* oldMaterial = NULL;

    if (partIndex == -1)
    {
        oldMaterial = _material;

        // Set new shared material.
        if (material)
        {
            _material = material;
            _material->addRef();
        }
    }
    else if (partIndex >= 0 && partIndex < (int)getMeshPartCount())
    {
        // Ensure mesh part count is up-to-date.
        validatePartCount();

        // Release existing part material and part binding.
        if (_partMaterials)
        {
            oldMaterial = _partMaterials[partIndex];
        }
        else
        {
            // Allocate part arrays for the first time.
            if (_partMaterials == NULL)
            {
                _partMaterials = new Material*[_partCount];
                memset(_partMaterials, 0, sizeof(Material*) * _partCount);
            }
        }

        // Set new part material.
        if (material)
        {
            _partMaterials[partIndex] = material;
            material->addRef();
        }
    }

    // Release existing material and binding.
    if (oldMaterial)
    {
        SAFE_RELEASE(oldMaterial);
    }

    if (material)
    {
        // Apply node binding for the new material.
        if (_node)
        {
            setMaterialNodeBinding(material);
        }
    }
}

Material* Model::setMaterial(const char* vshPath, const char* fshPath, const char* defines, int partIndex)
{
    // Try to create a Material with the given parameters.
    Material* material = Material::create(vshPath, fshPath, defines);
    if (material == NULL)
    {
        GP_ERROR("Failed to create material for model.");
        return NULL;
    }

    // Assign the material to us.
    setMaterial(material, partIndex);

    // Release the material since we now have a reference to it.
    material->release();

    return material;
}

Material* Model::setMaterial(const char* materialPath, int partIndex)
{
    // Try to create a Material from the specified material file.
    Material* material = Material::create(materialPath);
    if (material == NULL)
    {
        GP_ERROR("Failed to create material for model.");
        return NULL;
    }

    // Assign the material to us
    setMaterial(material, partIndex);

    // Release the material since we now have a reference to it
    material->release();

    return material;
}

bool Model::hasMaterial(unsigned int partIndex) const
{
    return (partIndex < _partCount && _partMaterials && _partMaterials[partIndex]);
}

MeshSkin* Model::getSkin() const
{
    return _skin;
}

void Model::setSkin(MeshSkin* skin)
{
    if (_skin != skin)
    {
        // Free the old skin
        SAFE_DELETE(_skin);

        // Assign the new skin
        _skin = skin;
        if (_skin)
            _skin->_model = this;
    }
}

void Model::setNode(Node* node)
{
    Drawable::setNode(node);

    // Re-bind node related material parameters
    if (node)
    {
        if (_material)
        {
           setMaterialNodeBinding(_material);
        }
        if (_partMaterials)
        {
            for (unsigned int i = 0; i < _partCount; ++i)
            {
                if (_partMaterials[i])
                {
                    setMaterialNodeBinding(_partMaterials[i]);
                }
            }
        }
    }
}

unsigned int Model::draw()
{
    GP_ASSERT(_mesh);

    unsigned int partCount = _mesh->getPartCount();
    if (partCount == 0)
    {
        // No mesh parts (no index buffers).
        if (_material)
        {
            Technique* technique = _material->getTechnique();
            GP_ASSERT(technique);
            unsigned int passCount = technique->getPassCount();
            for (unsigned int i = 0; i < passCount; ++i)
            {
                Pass* pass = technique->getPassByIndex(i);
                GP_ASSERT(pass);
                pass->bind(_mesh->getPrimitiveType());
                _mesh->_vertexBuffer->bind();
                _mesh->draw();
                pass->unbind();
            }
        }
    }
    else
    {
        for (unsigned int i = 0; i < partCount; ++i)
        {
            MeshPart* part = _mesh->getPart(i);
            GP_ASSERT(part);

            // Get the material for this mesh part.
            Material* material = getMaterial(i);
            if (material)
            {
                Technique* technique = material->getTechnique();
                GP_ASSERT(technique);
                unsigned int passCount = technique->getPassCount();
                for (unsigned int j = 0; j < passCount; ++j)
                {
                    Pass* pass = technique->getPassByIndex(j);
                    GP_ASSERT(pass);
                    pass->bind(part->getPrimitiveType());                  

                    _mesh->_vertexBuffer->bind();
                    part->_indexBuffer->bind();
                    part->draw();
                    pass->unbind();
                }
            }
        }
    }
    return partCount;
}

void Model::setMaterialNodeBinding(Material *material)
{
    GP_ASSERT(material);

    if (_node)
    {
        material->setNodeBinding(getNode());
    }
}

Drawable* Model::clone(NodeCloneContext& context)
{
    Model* model = Model::create(getMesh());
    if (!model)
    {
        GP_ERROR("Failed to clone model.");
        return NULL;
    }

    if (getSkin())
    {
        model->setSkin(getSkin()->clone(context));
    }
    if (getMaterial())
    {
        Material* materialClone = getMaterial()->clone(context);
        if (!materialClone)
        {
            GP_ERROR("Failed to clone material for model.");
            return model;
        }
        model->setMaterial(materialClone);
        materialClone->release();
    }
    if (_partMaterials)
    {
        GP_ASSERT(_partCount == model->_partCount);
        for (unsigned int i = 0; i < _partCount; ++i)
        {
            if (_partMaterials[i])
            {
                Material* materialClone = _partMaterials[i]->clone(context);
                model->setMaterial(materialClone, i);
                materialClone->release();
            }
        }
    }
    return model;
}

void Model::validatePartCount()
{
    GP_ASSERT(_mesh);
    unsigned int partCount = _mesh->getPartCount();

    if (_partCount != partCount)
    {
        // Allocate new arrays and copy old items to them.
        if (_partMaterials)
        {
            Material** oldArray = _partMaterials;
            _partMaterials = new Material*[partCount];
            memset(_partMaterials, 0, sizeof(Material*) * partCount);
            if (oldArray)
            {
                for (unsigned int i = 0; i < _partCount; ++i)
                {
                    _partMaterials[i] = oldArray[i];
                }
            }
            SAFE_DELETE_ARRAY(oldArray);
        }
        // Update local part count.
        _partCount = _mesh->getPartCount();
    }
}

}
